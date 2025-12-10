#include "CosmicMeteorsGameMode.h"

#include "Engine/World.h"
#include "Play_Stadium/Core/PlayStadiumGameInstance/PlayStadiumGameInstance.h"
#include "Play_Stadium/Core/Questions/MultipleChoiceQuestion/MultipleChoiceQuestion.h"
#include "Play_Stadium/Core/Questions/QuestionBase.h"
#include "Play_Stadium/Maps/MultipleChoiceLevels/CosmicMeteorsLevel/CosmicMeteorsLevel.h"

DEFINE_LOG_CATEGORY_STATIC(LogCosmicMeteorsGameMode, Log, All);

void ACosmicMeteorsGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeQuestionForLevel();
}

void ACosmicMeteorsGameMode::InitializeQuestionForLevel()
{
	bHasActiveQuestion = false;
	bIsProcessingResult = false;
	UnbindMeteorDelegates();
	ResetProgressCounters(0);

	FMultipleChoiceQuestionData QuestionData;
	if (!TryGetCurrentMultipleChoiceQuestion(QuestionData))
	{
		return;
	}

	const UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return;
	}

	const int32 DisplayQuestionIndex = FMath::Max(1, GameInstance->GetNextQuestionIndex());
	ApplyQuestionToLevel(QuestionData, DisplayQuestionIndex);
}

bool ACosmicMeteorsGameMode::TryGetCurrentMultipleChoiceQuestion(FMultipleChoiceQuestionData& OutQuestionData) const
{
	const UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	const TObjectPtr<UQuestionBase> CurrentQuestion = GameInstance->GetCurrentQuestion();
	if (!CurrentQuestion)
	{
		UE_LOG(LogCosmicMeteorsGameMode, Warning, TEXT("Current question is not available."));
		return false;
	}

	const UMultipleChoiceQuestion* MultipleChoiceQuestion = Cast<UMultipleChoiceQuestion>(CurrentQuestion.Get());
	if (!MultipleChoiceQuestion)
	{
		UE_LOG(LogCosmicMeteorsGameMode, Warning, TEXT("Current question is not a multiple choice question."));
		return false;
	}

	OutQuestionData = MultipleChoiceQuestion->GetQuestionData();
	return true;
}

bool ACosmicMeteorsGameMode::TryConsumeNextMultipleChoiceQuestion(FMultipleChoiceQuestionData& OutQuestionData)
{
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	TObjectPtr<UQuestionBase> NextQuestion;
	if (!GameInstance->TryConsumeNextQuestion(NextQuestion))
	{
		UE_LOG(LogCosmicMeteorsGameMode, Warning, TEXT("Unable to consume next question."));
		return false;
	}

	UMultipleChoiceQuestion* MultipleChoiceQuestion = Cast<UMultipleChoiceQuestion>(NextQuestion.Get());
	if (!MultipleChoiceQuestion)
	{
		UE_LOG(LogCosmicMeteorsGameMode, Warning, TEXT("Next question is not a multiple choice question."));
		return false;
	}

	OutQuestionData = MultipleChoiceQuestion->GetQuestionData();
	return true;
}

void ACosmicMeteorsGameMode::ApplyQuestionToLevel(const FMultipleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex)
{
	ACosmicMeteorsLevel* Level = GetCosmicMeteorsLevel();
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!Level || !GameInstance)
	{
		return;
	}

	bHasActiveQuestion = true;
	bIsProcessingResult = false;
	CurrentQuestionData = QuestionData;

	Level->ApplyQuestionData(QuestionData, CurrentQuestionIndex, GameInstance->GetTotalQuestionsCount());

	const TArray<FMeteorDestroyedSignature*> Delegates = Level->GetMeteorDestroyedDelegates();
	ResetProgressCounters(Delegates.Num());
	BindMeteorDelegates(Delegates);
}

UPlayStadiumGameInstance* ACosmicMeteorsGameMode::GetPlayStadiumGameInstance() const
{
	return GetGameInstance<UPlayStadiumGameInstance>();
}

ACosmicMeteorsLevel* ACosmicMeteorsGameMode::GetCosmicMeteorsLevel() const
{
	if (UWorld* World = GetWorld())
	{
		return Cast<ACosmicMeteorsLevel>(World->GetLevelScriptActor());
	}

	return nullptr;
}

void ACosmicMeteorsGameMode::AdvanceAfterScoring(ETargetDestroyReason DestroyReason)
{
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return;
	}

	const int32 NextQuestionIndex = GameInstance->GetNextQuestionIndex();
	const TObjectPtr<UQuestionBase> UpcomingQuestion = GameInstance->GetQuestionAtIndex(NextQuestionIndex);
	const bool bCanReuseLevel = (DestroyReason == ETargetDestroyReason::FromPlayerAttack || DestroyReason == ETargetDestroyReason::FromPlayerAction) &&
		UpcomingQuestion && UpcomingQuestion->GetType() == EQuestionType::MultipleChoiceQuestion;

	if (bCanReuseLevel)
	{
		FMultipleChoiceQuestionData NewQuestionData;
		if (TryConsumeNextMultipleChoiceQuestion(NewQuestionData))
		{
			UnbindMeteorDelegates();

			if (ACosmicMeteorsLevel* Level = GetCosmicMeteorsLevel())
			{
				Level->RespawnMeteors();
				Level->ApplyQuestionData(NewQuestionData, GameInstance->GetNextQuestionIndex(), GameInstance->GetTotalQuestionsCount());

				const TArray<FMeteorDestroyedSignature*> Delegates = Level->GetMeteorDestroyedDelegates();
				ResetProgressCounters(Delegates.Num());
				BindMeteorDelegates(Delegates);

				CurrentQuestionData = NewQuestionData;
				bHasActiveQuestion = true;
				bIsProcessingResult = false;
				return;
			}
		}
	}

	GameInstance->HandleStartTestRequested();
}

void ACosmicMeteorsGameMode::BindMeteorDelegates(const TArray<FMeteorDestroyedSignature*>& Delegates)
{
	CachedMeteorDelegates = Delegates;
	for (FMeteorDestroyedSignature* Delegate : CachedMeteorDelegates)
	{
		if (Delegate)
		{
			Delegate->AddDynamic(this, &ACosmicMeteorsGameMode::HandleMeteorDestroyed);
		}
	}
}

void ACosmicMeteorsGameMode::UnbindMeteorDelegates()
{
	for (FMeteorDestroyedSignature* Delegate : CachedMeteorDelegates)
	{
		if (Delegate)
		{
			Delegate->RemoveDynamic(this, &ACosmicMeteorsGameMode::HandleMeteorDestroyed);
		}
	}

	CachedMeteorDelegates.Reset();
}

void ACosmicMeteorsGameMode::ResetProgressCounters(int32 MeteorCount)
{
	TotalMeteorCount = FMath::Max(0, MeteorCount);
	DestroyedMeteorCount = 0;
	CorrectDestroyedByPlayerCount = 0;
	bDestroyedIncorrectMeteor = false;
}

void ACosmicMeteorsGameMode::CalculateAndApplyScore(ETargetDestroyReason DestroyReason)
{
	if (bIsProcessingResult)
	{
		return;
	}

	bIsProcessingResult = true;
	UnbindMeteorDelegates();
	bHasActiveQuestion = false;

	int32 ScoreToAdd = 0;
	if (DestroyReason == ETargetDestroyReason::FromPlayerAttack || DestroyReason == ETargetDestroyReason::FromPlayerAction)
	{
		if (!bDestroyedIncorrectMeteor)
		{
			int32 TotalCorrectChoices = 0;
			for (const TPair<FString, bool>& Choice : CurrentQuestionData.Choices)
			{
				if (Choice.Value)
				{
					++TotalCorrectChoices;
				}
			}

			if (TotalCorrectChoices > 0)
			{
				const float CorrectRatio = static_cast<float>(CorrectDestroyedByPlayerCount) / static_cast<float>(TotalCorrectChoices);
				ScoreToAdd = FMath::RoundToInt(static_cast<float>(CurrentQuestionData.GetReward()) * CorrectRatio);
			}
		}
	}

	if (ScoreToAdd != 0)
	{
		if (UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance())
		{
			GameInstance->AddScore(ScoreToAdd);
		}
	}

	AdvanceAfterScoring(DestroyReason);
}

void ACosmicMeteorsGameMode::HandleMeteorDestroyed(ETargetDestroyReason DestroyReason, bool bWasCorrectChoice)
{
	if (!bHasActiveQuestion)
	{
		return;
	}

	DestroyedMeteorCount = FMath::Clamp(DestroyedMeteorCount + 1, 0, TotalMeteorCount);

	const bool bIsDirectPlayerAttack = DestroyReason == ETargetDestroyReason::FromPlayerAttack;
	if (bIsDirectPlayerAttack && !bWasCorrectChoice)
	{
		bDestroyedIncorrectMeteor = true;
	}

	if (DestroyReason == ETargetDestroyReason::FromPlayerAttack)
	{
		if (bWasCorrectChoice)
		{
			++CorrectDestroyedByPlayerCount;
		}

		if (DestroyedMeteorCount >= TotalMeteorCount)
		{
			CalculateAndApplyScore(DestroyReason);
		}

		return;
	}

	if (DestroyReason == ETargetDestroyReason::FromPlayerAction)
	{
		CalculateAndApplyScore(DestroyReason);
		return;
	}

	if (DestroyReason == ETargetDestroyReason::FromDestinationPoint)
	{
		CalculateAndApplyScore(DestroyReason);
	}
}
