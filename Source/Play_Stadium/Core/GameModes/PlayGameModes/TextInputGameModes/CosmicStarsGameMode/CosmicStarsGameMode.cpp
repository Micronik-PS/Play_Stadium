#include "CosmicStarsGameMode.h"

#include "Engine/World.h"
#include "Play_Stadium/Core/PlayStadiumGameInstance/PlayStadiumGameInstance.h"
#include "Play_Stadium/Core/Questions/QuestionBase.h"
#include "Play_Stadium/Core/Questions/TextInputQuestion/TextInputQuestion.h"
#include "Play_Stadium/Maps/TextInputLevels/CosmicStars/CosmicStarsLevel.h"
#include "Play_Stadium/Targets/CosmicSector/CosmicSector.h"

DEFINE_LOG_CATEGORY_STATIC(LogCosmicStarsGameMode, Log, All);

void ACosmicStarsGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeQuestionForLevel();
}

void ACosmicStarsGameMode::HandleSectorSelected(ACosmicSector* SelectedSector)
{
	if (!bHasActiveQuestion || bIsProcessingResult || !IsValid(SelectedSector) || !SelectedSector->CanBeSelected())
	{
		return;
	}

	const FString SelectedLetter = SelectedSector->GetLetter();
	SelectedSector->TriggerDestruction(ETargetDestroyReason::FromPlayerAttack);

	if (SelectedLetter.IsEmpty())
	{
		return;
	}

	if (ACosmicStarsLevel* Level = GetCosmicStarsLevel())
	{
		Level->HandleSelectedLetter(SelectedLetter);
	}
}

void ACosmicStarsGameMode::InitializeQuestionForLevel()
{
	bHasActiveQuestion = false;
	bIsProcessingResult = false;
	UnbindLevelDelegates();

	FTextInputQuestionData QuestionData;
	if (!TryGetCurrentTextInputQuestion(QuestionData))
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

bool ACosmicStarsGameMode::TryGetCurrentTextInputQuestion(FTextInputQuestionData& OutQuestionData) const
{
	const UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	const TObjectPtr<UQuestionBase> CurrentQuestion = GameInstance->GetCurrentQuestion();
	if (!CurrentQuestion)
	{
		UE_LOG(LogCosmicStarsGameMode, Warning, TEXT("Current question is not available."));
		return false;
	}

	const UTextInputQuestion* TextInputQuestion = Cast<UTextInputQuestion>(CurrentQuestion.Get());
	if (!TextInputQuestion)
	{
		UE_LOG(LogCosmicStarsGameMode, Warning, TEXT("Current question is not a text input question."));
		return false;
	}

	OutQuestionData = TextInputQuestion->GetQuestionData();
	return true;
}

bool ACosmicStarsGameMode::TryConsumeNextTextInputQuestion(FTextInputQuestionData& OutQuestionData)
{
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	TObjectPtr<UQuestionBase> NextQuestion;
	if (!GameInstance->TryConsumeNextQuestion(NextQuestion))
	{
		UE_LOG(LogCosmicStarsGameMode, Warning, TEXT("Unable to consume next question."));
		return false;
	}

	UTextInputQuestion* TextInputQuestion = Cast<UTextInputQuestion>(NextQuestion.Get());
	if (!TextInputQuestion)
	{
		UE_LOG(LogCosmicStarsGameMode, Warning, TEXT("Next question is not a text input question."));
		return false;
	}

	OutQuestionData = TextInputQuestion->GetQuestionData();
	return true;
}

void ACosmicStarsGameMode::ApplyQuestionToLevel(const FTextInputQuestionData& QuestionData, int32 CurrentQuestionIndex)
{
	ACosmicStarsLevel* Level = GetCosmicStarsLevel();
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!Level || !GameInstance)
	{
		return;
	}

	UnbindLevelDelegates();
	CurrentQuestionData = QuestionData;
	bHasActiveQuestion = true;
	bIsProcessingResult = false;

	Level->ApplyQuestionData(QuestionData, CurrentQuestionIndex, GameInstance->GetTotalQuestionsCount());
	BindLevelDelegates(Level);
}

void ACosmicStarsGameMode::AdvanceToNextQuestion()
{
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return;
	}

	const int32 NextQuestionIndex = GameInstance->GetNextQuestionIndex();
	const TObjectPtr<UQuestionBase> UpcomingQuestion = GameInstance->GetQuestionAtIndex(NextQuestionIndex);
	const bool bCanReuseLevel = UpcomingQuestion && UpcomingQuestion->GetType() == EQuestionType::TextInputQuestion;

	if (bCanReuseLevel)
	{
		FTextInputQuestionData NewQuestionData;
		if (TryConsumeNextTextInputQuestion(NewQuestionData))
		{
			ApplyQuestionToLevel(NewQuestionData, GameInstance->GetNextQuestionIndex());
			return;
		}
	}

	UnbindLevelDelegates();
	GameInstance->HandleStartTestRequested();
}

UPlayStadiumGameInstance* ACosmicStarsGameMode::GetPlayStadiumGameInstance() const
{
	return GetGameInstance<UPlayStadiumGameInstance>();
}

ACosmicStarsLevel* ACosmicStarsGameMode::GetCosmicStarsLevel() const
{
	if (UWorld* World = GetWorld())
	{
		return Cast<ACosmicStarsLevel>(World->GetLevelScriptActor());
	}

	return nullptr;
}

void ACosmicStarsGameMode::BindLevelDelegates(ACosmicStarsLevel* Level)
{
	if (!Level)
	{
		return;
	}

	CachedLevel = Level;
	Level->OnQuestionCompleted.AddUniqueDynamic(this, &ACosmicStarsGameMode::HandleQuestionCompleted);
	Level->OnQuestionFailed.AddUniqueDynamic(this, &ACosmicStarsGameMode::HandleQuestionFailed);
}

void ACosmicStarsGameMode::UnbindLevelDelegates()
{
	if (ACosmicStarsLevel* Level = CachedLevel.Get())
	{
		Level->OnQuestionCompleted.RemoveDynamic(this, &ACosmicStarsGameMode::HandleQuestionCompleted);
		Level->OnQuestionFailed.RemoveDynamic(this, &ACosmicStarsGameMode::HandleQuestionFailed);
	}

	CachedLevel.Reset();
}

void ACosmicStarsGameMode::HandleQuestionCompleted()
{
	if (!bHasActiveQuestion || bIsProcessingResult)
	{
		return;
	}

	bIsProcessingResult = true;
	bHasActiveQuestion = false;

	if (UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance())
	{
		GameInstance->AddScore(CurrentQuestionData.GetReward());
	}

	AdvanceToNextQuestion();
}

void ACosmicStarsGameMode::HandleQuestionFailed()
{
	if (!bHasActiveQuestion || bIsProcessingResult)
	{
		return;
	}

	bIsProcessingResult = true;
	bHasActiveQuestion = false;
	AdvanceToNextQuestion();
}
