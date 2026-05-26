#include "LaserConnectionGameMode.h"

#include "Engine/World.h"
#include "Play_Stadium/Core/PlayStadiumGameInstance/PlayStadiumGameInstance.h"
#include "Play_Stadium/Core/Questions/MatchingQuestion/MatchingQuestion.h"
#include "Play_Stadium/Core/Questions/QuestionBase.h"
#include "Play_Stadium/Maps/MatchingQuestionLevels/LaserConnection/LaserConnectionLevel.h"

DEFINE_LOG_CATEGORY_STATIC(LogLaserConnectionGameMode, Log, All);

void ALaserConnectionGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeQuestionForLevel();
}

void ALaserConnectionGameMode::InitializeQuestionForLevel()
{
	bHasActiveQuestion = false;
	bIsProcessingResult = false;
	UnbindLevelDelegates();

	FMatchingQuestionData QuestionData;
	if (!TryGetCurrentMatchingQuestion(QuestionData))
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

bool ALaserConnectionGameMode::TryGetCurrentMatchingQuestion(FMatchingQuestionData& OutQuestionData) const
{
	const UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	const TObjectPtr<UQuestionBase> CurrentQuestion = GameInstance->GetCurrentQuestion();
	if (!CurrentQuestion)
	{
		UE_LOG(LogLaserConnectionGameMode, Warning, TEXT("Current question is not available."));
		return false;
	}

	const UMatchingQuestion* MatchingQuestion = Cast<UMatchingQuestion>(CurrentQuestion.Get());
	if (!MatchingQuestion)
	{
		UE_LOG(LogLaserConnectionGameMode, Warning, TEXT("Current question is not a matching question."));
		return false;
	}

	OutQuestionData = MatchingQuestion->GetQuestionData();
	return true;
}

bool ALaserConnectionGameMode::TryConsumeNextMatchingQuestion(FMatchingQuestionData& OutQuestionData)
{
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	TObjectPtr<UQuestionBase> NextQuestion;
	if (!GameInstance->TryConsumeNextQuestion(NextQuestion))
	{
		UE_LOG(LogLaserConnectionGameMode, Warning, TEXT("Unable to consume next question."));
		return false;
	}

	UMatchingQuestion* MatchingQuestion = Cast<UMatchingQuestion>(NextQuestion.Get());
	if (!MatchingQuestion)
	{
		UE_LOG(LogLaserConnectionGameMode, Warning, TEXT("Next question is not a matching question."));
		return false;
	}

	OutQuestionData = MatchingQuestion->GetQuestionData();
	return true;
}

void ALaserConnectionGameMode::ApplyQuestionToLevel(const FMatchingQuestionData& QuestionData, int32 CurrentQuestionIndex)
{
	ALaserConnectionLevel* Level = GetLaserConnectionLevel();
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

void ALaserConnectionGameMode::AdvanceToNextQuestion()
{
	UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance();
	if (!GameInstance)
	{
		return;
	}

	const int32 NextQuestionIndex = GameInstance->GetNextQuestionIndex();
	const TObjectPtr<UQuestionBase> UpcomingQuestion = GameInstance->GetQuestionAtIndex(NextQuestionIndex);
	const bool bCanReuseLevel = UpcomingQuestion && UpcomingQuestion->GetType() == EQuestionType::MatchingQuestion;

	if (bCanReuseLevel)
	{
		FMatchingQuestionData NewQuestionData;
		if (TryConsumeNextMatchingQuestion(NewQuestionData))
		{
			ApplyQuestionToLevel(NewQuestionData, GameInstance->GetNextQuestionIndex());
			return;
		}
	}

	UnbindLevelDelegates();
	GameInstance->HandleStartTestRequested();
}

UPlayStadiumGameInstance* ALaserConnectionGameMode::GetPlayStadiumGameInstance() const
{
	return GetGameInstance<UPlayStadiumGameInstance>();
}

ALaserConnectionLevel* ALaserConnectionGameMode::GetLaserConnectionLevel() const
{
	if (UWorld* World = GetWorld())
	{
		return Cast<ALaserConnectionLevel>(World->GetLevelScriptActor());
	}

	return nullptr;
}

void ALaserConnectionGameMode::BindLevelDelegates(ALaserConnectionLevel* Level)
{
	if (!Level)
	{
		return;
	}

	CachedLevel = Level;
	Level->OnQuestionAnswered.AddUniqueDynamic(this, &ALaserConnectionGameMode::HandleQuestionAnswered);
}

void ALaserConnectionGameMode::UnbindLevelDelegates()
{
	if (ALaserConnectionLevel* Level = CachedLevel.Get())
	{
		Level->OnQuestionAnswered.RemoveDynamic(this, &ALaserConnectionGameMode::HandleQuestionAnswered);
	}

	CachedLevel.Reset();
}

void ALaserConnectionGameMode::HandleQuestionAnswered(bool bAllConnectionsCorrect)
{
	if (!bHasActiveQuestion || bIsProcessingResult)
	{
		return;
	}

	bIsProcessingResult = true;
	bHasActiveQuestion = false;

	if (bAllConnectionsCorrect)
	{
		if (UPlayStadiumGameInstance* GameInstance = GetPlayStadiumGameInstance())
		{
			GameInstance->AddScore(CurrentQuestionData.GetReward());
		}
	}

	AdvanceToNextQuestion();
}
