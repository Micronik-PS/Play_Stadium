#include "CosmicBlasterLevel.h"

#include "Algo/RandomShuffle.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"

ACosmicBlasterLevel::ACosmicBlasterLevel()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACosmicBlasterLevel::BeginPlay()
{
	Super::BeginPlay();

	CacheBubbleSpawnDataIfNeeded();
}

void ACosmicBlasterLevel::ApplyQuestionData(const FSingleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions)
{
	CacheBubbleSpawnDataIfNeeded();
	AssignChoicesToBubbles(QuestionData);
	ConfigureBubbleSpeeds(QuestionData);
	UpdateQuestionBanner(QuestionData, CurrentQuestionIndex, TotalQuestions);
}

TArray<FCosmicBubbleDestroyedSignature*> ACosmicBlasterLevel::GetBubbleDestroyedDelegates() const
{
	TArray<FCosmicBubbleDestroyedSignature*> Delegates;

	for (ACosmicBubble* Bubble : Bubbles)
	{
		if (Bubble)
		{
			Delegates.Add(&Bubble->OnBubbleDestroyed);
		}
	}

	return Delegates;
}

void ACosmicBlasterLevel::RespawnBubbles()
{
	CacheBubbleSpawnDataIfNeeded();

	for (ACosmicBubble* Bubble : Bubbles)
	{
		if (Bubble)
		{
			Bubble->Destroy();
		}
	}

	Bubbles.Reset();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (const FBubbleSpawnData& SpawnData : InitialBubbleSpawnData)
	{
		if (!SpawnData.BubbleClass)
		{
			continue;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (ACosmicBubble* NewBubble = World->SpawnActor<ACosmicBubble>(SpawnData.BubbleClass, SpawnData.Transform, SpawnParams))
		{
			Bubbles.Add(NewBubble);
		}
	}
}

void ACosmicBlasterLevel::CacheBubbleSpawnDataIfNeeded()
{
	if (!InitialBubbleSpawnData.IsEmpty())
	{
		return;
	}

	for (ACosmicBubble* Bubble : Bubbles)
	{
		if (!Bubble)
		{
			continue;
		}

		FBubbleSpawnData SpawnData;
		SpawnData.BubbleClass = Bubble->GetClass();
		SpawnData.Transform = Bubble->GetActorTransform();
		InitialBubbleSpawnData.Add(SpawnData);
	}
}

void ACosmicBlasterLevel::AssignChoicesToBubbles(const FSingleChoiceQuestionData& QuestionData)
{
	TArray<TPair<FString, bool>> ChoicePairs;
	ChoicePairs.Reserve(QuestionData.Choices.Num());
	for (const TPair<FString, bool>& Pair : QuestionData.Choices)
	{
		ChoicePairs.Add(Pair);
	}
	Algo::RandomShuffle(ChoicePairs);

	int32 ChoiceIndex = 0;
	for (ACosmicBubble* Bubble : Bubbles)
	{
		if (!Bubble)
		{
			continue;
		}

		if (ChoiceIndex < ChoicePairs.Num())
		{
			const TPair<FString, bool>& Choice = ChoicePairs[ChoiceIndex];
			++ChoiceIndex;
			Bubble->SetChoiceData(FText::FromString(Choice.Key), Choice.Value);
		}
		else
		{
			Bubble->SetChoiceData(FText::GetEmpty(), false);
		}
	}
}

void ACosmicBlasterLevel::UpdateQuestionBanner(const FSingleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions)
{
	if (!QuestionBanner)
	{
		return;
	}

	QuestionBanner->SetQuestionText(FText::FromString(QuestionData.QuestionText));
	QuestionBanner->SetCounter(CurrentQuestionIndex, TotalQuestions);
}

void ACosmicBlasterLevel::ConfigureBubbleSpeeds(const FSingleChoiceQuestionData& QuestionData)
{
	const int32 TimeLimit = FMath::Max(1, QuestionData.GetTimeLimitInSeconds());
	bool bBaseSpeedAssigned = false;

	for (ACosmicBubble* Bubble : Bubbles)
	{
		if (!Bubble)
		{
			continue;
		}

		const float BaseSpeed = CalculateBaseSpeedFromTimeLimit(Bubble, TimeLimit);
		float TargetSpeed = BaseSpeed;

		if (bBaseSpeedAssigned)
		{
			const float SpeedOffset = FMath::RandRange(0.0f, SpeedDeviationRange);
			TargetSpeed = BaseSpeed - SpeedOffset;
			if (TargetSpeed < 0.0f)
			{
				TargetSpeed = 0.0f;
			}
		}
		else
		{
			bBaseSpeedAssigned = true;
		}

		Bubble->SetMovementSpeed(TargetSpeed);
	}
}

float ACosmicBlasterLevel::CalculateBaseSpeedFromTimeLimit(ACosmicBubble* Bubble, int32 TimeLimitInSeconds) const
{
	const float DistanceToSurface = FindDistanceToSurfaceBelow(Bubble);
	const float CollisionRadius = Bubble ? Bubble->GetCollisionRadius() : 0.0f;
	const float TravelDistance = FMath::Max(0.0f, DistanceToSurface - CollisionRadius);

	if (TimeLimitInSeconds <= 0)
	{
		return TravelDistance;
	}

	return TravelDistance / static_cast<float>(TimeLimitInSeconds);
}

float ACosmicBlasterLevel::FindDistanceToSurfaceBelow(const ACosmicBubble* Bubble) const
{
	if (!Bubble)
	{
		return FallbackDistance;
	}

	if (MaxTraceDistance <= KINDA_SMALL_NUMBER)
	{
		return FallbackDistance;
	}

	const FVector StartLocation = Bubble->GetActorLocation();
	const FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, MaxTraceDistance);

	FHitResult HitResult;
	if (const UWorld* World = GetWorld())
	{
		const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CosmicBlasterLevelTrace), false, Bubble);
		if (World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, QueryParams))
		{
			return FMath::Max(KINDA_SMALL_NUMBER, StartLocation.Z - HitResult.ImpactPoint.Z);
		}
	}

	return FallbackDistance;
}
