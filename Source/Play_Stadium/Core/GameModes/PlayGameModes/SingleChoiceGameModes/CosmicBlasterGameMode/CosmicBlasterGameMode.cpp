#include "CosmicBlasterGameMode.h"

#include "Play_Stadium/Core/PlayStadiumGameInstance/PlayStadiumGameInstance.h"
#include "Play_Stadium/Maps/SingleChoiceLevels/CosmicBlasterLevel/CosmicBlasterLevel.h"
#include "Play_Stadium/Core/Questions/QuestionBase.h"
#include "Play_Stadium/Core/Questions/SingleChoiceQuestion/SingleChoiceQuestion.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogCosmicBlasterGameMode, Log, All);

void ACosmicBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeQuestionForLevel();
}

void ACosmicBlasterGameMode::InitializeQuestionForLevel()
{
	bHasReceivedAnswer = false;
	bHasActiveQuestion = false;
	UnbindBubbleDelegates();

	FSingleChoiceQuestionData QuestionData;
	if (!TryGetCurrentSingleChoiceQuestion(QuestionData))
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

bool ACosmicBlasterGameMode::TryGetCurrentSingleChoiceQuestion(FSingleChoiceQuestionData& OutQuestionData) const
{
	const UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	const TObjectPtr<UQuestionBase> CurrentQuestion = GameInstance->GetCurrentQuestion();
	if (!CurrentQuestion)
	{
		UE_LOG(LogCosmicBlasterGameMode, Warning, TEXT("Current question is not available."));
		return false;
	}

	const USingleChoiceQuestion* SingleChoiceQuestion = Cast<USingleChoiceQuestion>(CurrentQuestion.Get());
	if (!SingleChoiceQuestion)
	{
		UE_LOG(LogCosmicBlasterGameMode, Warning, TEXT("Current question is not a single choice question."));
		return false;
	}

	OutQuestionData = SingleChoiceQuestion->GetQuestionData();
	return true;
}

bool ACosmicBlasterGameMode::TryConsumeNextSingleChoiceQuestion(FSingleChoiceQuestionData& OutQuestionData)
{
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	TObjectPtr<UQuestionBase> NextQuestion;
	if (!GameInstance->TryConsumeNextQuestion(NextQuestion))
	{
		UE_LOG(LogCosmicBlasterGameMode, Warning, TEXT("Unable to consume next question."));
		return false;
	}

	USingleChoiceQuestion* SingleChoiceQuestion = Cast<USingleChoiceQuestion>(NextQuestion.Get());
	if (!SingleChoiceQuestion)
	{
		UE_LOG(LogCosmicBlasterGameMode, Warning, TEXT("Next question is not a single choice question."));
		return false;
	}

	OutQuestionData = SingleChoiceQuestion->GetQuestionData();
	return true;
}

void ACosmicBlasterGameMode::ApplyQuestionToLevel(const FSingleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex)
{
	ACosmicBlasterLevel* Level = GetCosmicBlasterLevel();
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!Level || !GameInstance)
	{
		return;
	}

	bHasActiveQuestion = true;
	bHasReceivedAnswer = false;
	CurrentQuestionData = QuestionData;

	Level->ApplyQuestionData(QuestionData, CurrentQuestionIndex, GameInstance->GetTotalQuestionsCount());
	BindBubbleDelegates(Level->GetBubbleDestroyedDelegates());
}

UPlayStadiumGameInstance* ACosmicBlasterGameMode::GetPlayStadiumGameInstance() const
{
	return GetGameInstance<UPlayStadiumGameInstance>();
}

ACosmicBlasterLevel* ACosmicBlasterGameMode::GetCosmicBlasterLevel() const
{
	if (UWorld* World = GetWorld())
	{
		return Cast<ACosmicBlasterLevel>(World->GetLevelScriptActor());
	}

	return nullptr;
}

void ACosmicBlasterGameMode::AdvanceToNextQuestion(ETargetDestroyReason DestroyReason)
{
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return;
	}

	const int32 NextQuestionIndex = GameInstance->GetNextQuestionIndex();
	const TObjectPtr<UQuestionBase> UpcomingQuestion = GameInstance->GetQuestionAtIndex(NextQuestionIndex);
	const bool bCanReuseLevel = DestroyReason == ETargetDestroyReason::FromPlayerAttack &&
		UpcomingQuestion && UpcomingQuestion->GetType() == EQuestionType::SingleChoiceQuestion;

	if (bCanReuseLevel)
	{
		FSingleChoiceQuestionData NewQuestionData;
		if (TryConsumeNextSingleChoiceQuestion(NewQuestionData))
		{
			UnbindBubbleDelegates();

			if (ACosmicBlasterLevel* Level = GetCosmicBlasterLevel())
			{
				Level->RespawnBubbles();
			}

			ApplyQuestionToLevel(NewQuestionData, GameInstance->GetNextQuestionIndex());
			return;
		}
	}

	GameInstance->HandleStartTestRequested();
}

void ACosmicBlasterGameMode::BindBubbleDelegates(const TArray<FCosmicBubbleDestroyedSignature*>& Delegates)
{
	CachedBubbleDelegates = Delegates;
	for (FCosmicBubbleDestroyedSignature* Delegate : CachedBubbleDelegates)
	{
		if (Delegate)
		{
			Delegate->AddDynamic(this, &ACosmicBlasterGameMode::HandleBubbleDestroyed);
		}
	}
}

void ACosmicBlasterGameMode::UnbindBubbleDelegates()
{
	for (FCosmicBubbleDestroyedSignature* Delegate : CachedBubbleDelegates)
	{
		if (Delegate)
		{
			Delegate->RemoveDynamic(this, &ACosmicBlasterGameMode::HandleBubbleDestroyed);
		}
	}

	CachedBubbleDelegates.Reset();
}

void ACosmicBlasterGameMode::HandleBubbleDestroyed(ETargetDestroyReason DestroyReason, bool bWasCorrectChoice)
{
	if (bHasReceivedAnswer)
	{
		return;
	}

	bHasReceivedAnswer = true;
	UnbindBubbleDelegates();

	if (bWasCorrectChoice && bHasActiveQuestion)
	{
		if (UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance())
		{
			GameInstance->AddScore(CurrentQuestionData.GetReward());
		}
	}

	AdvanceToNextQuestion(DestroyReason);
}
