#include "Play_Stadium/Maps/TextInputLevels/CosmicStars/CosmicStarsLevel.h"

#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Play_Stadium/Vicinity/SingleChoiceVicinity/CosmicBlaster/CosmicBlasterQuestionBanner/CosmicBlasterQuestionBanner.h"
#include "Play_Stadium/Vicinity/TextInputVicinity/CosmicStars/CosmicSectorSpawner/CosmicSectorSpawner.h"
#include "Play_Stadium/Vicinity/TextInputVicinity/CosmicStars/CosmicWordFrame/CosmicWordFrame.h"
#include "Play_Stadium/Vicinity/TextInputVicinity/CosmicStars/CosmicWordFrameSpawner/CosmicWordFrameSpawner.h"

DEFINE_LOG_CATEGORY_STATIC(LogCosmicStarsLevel, Log, All);

ACosmicStarsLevel::ACosmicStarsLevel()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACosmicStarsLevel::BeginPlay()
{
	Super::BeginPlay();

	ResolveSpawnerReferences();
	ApplyStartupCamera();
}

void ACosmicStarsLevel::ApplyQuestionData(const FTextInputQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions)
{
	ResolveSpawnerReferences();
	StopQuestionActors();

	CurrentCorrectAnswer = QuestionData.CorrectAnswer;
	CurrentQuestionNumber = FMath::Max(1, CurrentQuestionIndex);
	TotalQuestionCount = FMath::Max(1, TotalQuestions);
	bHasActiveQuestion = true;
	bQuestionResolved = false;

	if (WordFrameSpawner)
	{
		WordFrameSpawner->OnWordFrameReachedDestination.RemoveDynamic(this, &ACosmicStarsLevel::HandleWordFrameReachedDestination);
		WordFrameSpawner->OnWordFrameReachedDestination.AddUniqueDynamic(this, &ACosmicStarsLevel::HandleWordFrameReachedDestination);
		WordFrameSpawner->SpawnWordFrames(CurrentCorrectAnswer, QuestionData.GetTimeLimitInSeconds());
	}
	else
	{
		UE_LOG(LogCosmicStarsLevel, Warning, TEXT("CosmicStarsLevel has no WordFrameSpawner. Place BP_CosmicWordFrameSpawner on the map or assign it in Details."));
	}

	if (SectorSpawners.IsEmpty())
	{
		UE_LOG(LogCosmicStarsLevel, Warning, TEXT("CosmicStarsLevel has no SectorSpawners. Place BP_CosmicSectorSpawner actors on the map or assign them in Details."));
	}

	ConfigureSectorSpawners(CurrentCorrectAnswer);
	RefreshSectorSpawnerMissingLetters();
	UpdateQuestionBanner(QuestionData, CurrentQuestionNumber, TotalQuestionCount);
	SetSectorSpawnersEnabled(true);
}

bool ACosmicStarsLevel::HandleSelectedLetter(const FString& Letter)
{
	if (!bHasActiveQuestion || bQuestionResolved || !WordFrameSpawner)
	{
		return false;
	}

	const int32 RevealedCount = WordFrameSpawner->RevealLetter(Letter);
	if (RevealedCount <= 0)
	{
		ResolveQuestion(false);
		return false;
	}

	if (WordFrameSpawner->IsWordCompleted())
	{
		ResolveQuestion(true);
	}
	else
	{
		RefreshSectorSpawnerMissingLetters();
	}

	return true;
}

void ACosmicStarsLevel::StopQuestionActors()
{
	bHasActiveQuestion = false;
	SetSectorSpawnersEnabled(false);
	ClearSectorSpawners();

	if (WordFrameSpawner)
	{
		WordFrameSpawner->OnWordFrameReachedDestination.RemoveDynamic(this, &ACosmicStarsLevel::HandleWordFrameReachedDestination);
		WordFrameSpawner->ClearSpawnedFrames();
	}
}

bool ACosmicStarsLevel::IsWordCompleted() const
{
	return WordFrameSpawner && WordFrameSpawner->IsWordCompleted();
}

TArray<ACosmicWordFrame*> ACosmicStarsLevel::GetWordFrames() const
{
	return WordFrameSpawner ? WordFrameSpawner->GetSpawnedFrames() : TArray<ACosmicWordFrame*>();
}

void ACosmicStarsLevel::ResolveSpawnerReferences()
{
	if (!bAutoFindSpawnersOnLevel)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (!WordFrameSpawner)
	{
		for (TActorIterator<ACosmicWordFrameSpawner> It(World); It; ++It)
		{
			ACosmicWordFrameSpawner* CandidateSpawner = *It;
			if (IsValid(CandidateSpawner) && !CandidateSpawner->IsActorBeingDestroyed())
			{
				WordFrameSpawner = CandidateSpawner;
				break;
			}
		}
	}

	if (!QuestionBanner)
	{
		for (TActorIterator<ACosmicBlasterQuestionBanner> It(World); It; ++It)
		{
			ACosmicBlasterQuestionBanner* CandidateBanner = *It;
			if (IsValid(CandidateBanner) && !CandidateBanner->IsActorBeingDestroyed())
			{
				QuestionBanner = CandidateBanner;
				break;
			}
		}
	}

	for (int32 SpawnerIndex = SectorSpawners.Num() - 1; SpawnerIndex >= 0; --SpawnerIndex)
	{
		if (!IsValid(SectorSpawners[SpawnerIndex]))
		{
			SectorSpawners.RemoveAtSwap(SpawnerIndex);
		}
	}

	for (TActorIterator<ACosmicSectorSpawner> It(World); It; ++It)
	{
		ACosmicSectorSpawner* CandidateSpawner = *It;
		if (IsValid(CandidateSpawner) && !CandidateSpawner->IsActorBeingDestroyed())
		{
			SectorSpawners.AddUnique(CandidateSpawner);
		}
	}
}

void ACosmicStarsLevel::UpdateQuestionBanner(const FTextInputQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions)
{
	if (!QuestionBanner)
	{
		UE_LOG(LogCosmicStarsLevel, Warning, TEXT("CosmicStarsLevel has no QuestionBanner. Place BP_CosmicBlasterQuestionBanner on the map or assign it in Details."));
		return;
	}

	QuestionBanner->SetQuestionText(FText::FromString(QuestionData.QuestionText));
	QuestionBanner->SetCounter(CurrentQuestionIndex, TotalQuestions);
}

void ACosmicStarsLevel::ApplyStartupCamera() const
{
	if (!CosmicStarsCameraActor)
	{
		return;
	}

	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PlayerController)
	{
		return;
	}

	FViewTargetTransitionParams TransitionParams;
	PlayerController->SetViewTarget(CosmicStarsCameraActor, TransitionParams);
}

void ACosmicStarsLevel::ConfigureSectorSpawners(const FString& CorrectAnswer)
{
	for (ACosmicSectorSpawner* SectorSpawner : SectorSpawners)
	{
		if (SectorSpawner)
		{
			SectorSpawner->ConfigureQuestionLetters(CorrectAnswer);
		}
	}
}

void ACosmicStarsLevel::RefreshSectorSpawnerMissingLetters()
{
	const TArray<FString> MissingLetters = WordFrameSpawner ? WordFrameSpawner->GetMissingLetters() : TArray<FString>();

	for (ACosmicSectorSpawner* SectorSpawner : SectorSpawners)
	{
		if (SectorSpawner)
		{
			SectorSpawner->SetMissingLetters(MissingLetters);
		}
	}
}

void ACosmicStarsLevel::SetSectorSpawnersEnabled(bool bEnabled)
{
	for (ACosmicSectorSpawner* SectorSpawner : SectorSpawners)
	{
		if (SectorSpawner)
		{
			SectorSpawner->SetSpawnEnabled(bEnabled);
		}
	}
}

void ACosmicStarsLevel::ClearSectorSpawners()
{
	for (ACosmicSectorSpawner* SectorSpawner : SectorSpawners)
	{
		if (SectorSpawner)
		{
			SectorSpawner->ClearSpawnedSectors();
		}
	}
}

void ACosmicStarsLevel::ResolveQuestion(bool bCompleted)
{
	if (bQuestionResolved)
	{
		return;
	}

	bQuestionResolved = true;
	bHasActiveQuestion = false;
	SetSectorSpawnersEnabled(false);

	if (bCompleted)
	{
		OnQuestionCompleted.Broadcast();
	}
	else
	{
		OnQuestionFailed.Broadcast();
	}
}

void ACosmicStarsLevel::HandleWordFrameReachedDestination(ACosmicWordFrame* WordFrame)
{
	if (!bHasActiveQuestion || bQuestionResolved)
	{
		return;
	}

	ResolveQuestion(false);
}
