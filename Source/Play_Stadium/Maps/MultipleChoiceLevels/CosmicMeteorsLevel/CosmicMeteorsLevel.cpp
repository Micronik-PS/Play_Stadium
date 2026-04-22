#include "CosmicMeteorsLevel.h"

#include "Algo/RandomShuffle.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Play_Stadium/Characters/FighterJet/ZD_FighterJet/ZD_FighterJet.h"

ACosmicMeteorsLevel::ACosmicMeteorsLevel()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACosmicMeteorsLevel::BeginPlay()
{
	Super::BeginPlay();

	CacheMeteorSpawnDataIfNeeded();
}

void ACosmicMeteorsLevel::ApplyQuestionData(const FMultipleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions)
{
	CacheMeteorSpawnDataIfNeeded();
	AssignChoicesToMeteors(QuestionData);
	ConfigureMeteorSpeeds(QuestionData);
	UpdateQuestionBanner(QuestionData, CurrentQuestionIndex, TotalQuestions);
}

TArray<AMeteor*> ACosmicMeteorsLevel::GetMeteors() const
{
	TArray<AMeteor*> AliveMeteors;

	for (AMeteor* Meteor : Meteors)
	{
		if (IsValid(Meteor))
		{
			AliveMeteors.Add(Meteor);
		}
	}

	return AliveMeteors;
}

void ACosmicMeteorsLevel::RespawnMeteors()
{
	CacheMeteorSpawnDataIfNeeded();

	for (AMeteor* Meteor : Meteors)
	{
		if (Meteor)
		{
			Meteor->Destroy();
		}
	}

	Meteors.Reset();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (const FMeteorSpawnData& SpawnData : InitialMeteorSpawnData)
	{
		if (!SpawnData.MeteorClass)
		{
			continue;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (AMeteor* NewMeteor = World->SpawnActor<AMeteor>(SpawnData.MeteorClass, SpawnData.Transform, SpawnParams))
		{
			NewMeteor->SetTextWidgetRelativeTransform(SpawnData.WidgetTransform);
			Meteors.Add(NewMeteor);
		}
	}
}

void ACosmicMeteorsLevel::CacheMeteorSpawnDataIfNeeded()
{
	if (!InitialMeteorSpawnData.IsEmpty())
	{
		return;
	}

	for (AMeteor* Meteor : Meteors)
	{
		if (!Meteor)
		{
			continue;
		}

		FMeteorSpawnData SpawnData;
		SpawnData.MeteorClass = Meteor->GetClass();
		SpawnData.Transform = Meteor->GetActorTransform();
		SpawnData.WidgetTransform = Meteor->GetTextWidgetRelativeTransform();
		InitialMeteorSpawnData.Add(SpawnData);
	}
}

void ACosmicMeteorsLevel::AssignChoicesToMeteors(const FMultipleChoiceQuestionData& QuestionData)
{
	TArray<TPair<FString, bool>> ChoicePairs;
	ChoicePairs.Reserve(QuestionData.Choices.Num());
	for (const TPair<FString, bool>& Pair : QuestionData.Choices)
	{
		ChoicePairs.Add(Pair);
	}
	Algo::RandomShuffle(ChoicePairs);

	int32 ChoiceIndex = 0;
	for (AMeteor* Meteor : Meteors)
	{
		if (!Meteor)
		{
			continue;
		}

		if (ChoiceIndex < ChoicePairs.Num())
		{
			const TPair<FString, bool>& Choice = ChoicePairs[ChoiceIndex];
			++ChoiceIndex;
			Meteor->SetChoiceData(FText::FromString(Choice.Key), Choice.Value);
		}
		else
		{
			Meteor->SetChoiceData(FText::GetEmpty(), false);
		}
	}
}

void ACosmicMeteorsLevel::UpdateQuestionBanner(const FMultipleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions)
{
	if (!QuestionBanner)
	{
		return;
	}

	QuestionBanner->SetQuestionText(FText::FromString(QuestionData.QuestionText));
	QuestionBanner->SetCounter(CurrentQuestionIndex, TotalQuestions);
}

void ACosmicMeteorsLevel::ConfigureMeteorSpeeds(const FMultipleChoiceQuestionData& QuestionData)
{
	const int32 TimeLimit = FMath::Max(1, QuestionData.GetTimeLimitInSeconds());
	bool bBaseSpeedAssigned = false;

	for (AMeteor* Meteor : Meteors)
	{
		if (!Meteor)
		{
			continue;
		}

		const float BaseSpeed = CalculateBaseSpeedFromTimeLimit(Meteor, TimeLimit);
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

		Meteor->SetMovementSpeed(TargetSpeed);
	}
}

float ACosmicMeteorsLevel::CalculateBaseSpeedFromTimeLimit(AMeteor* Meteor, int32 TimeLimitInSeconds) const
{
	const float DistanceAhead = FindDistanceAhead(Meteor);
	const float TravelDistance = FMath::Max(0.0f, DistanceAhead);

	if (TimeLimitInSeconds <= 0)
	{
		return TravelDistance;
	}

	return TravelDistance / static_cast<float>(TimeLimitInSeconds);
}

float ACosmicMeteorsLevel::FindDistanceAhead(const AMeteor* Meteor) const
{
	if (!Meteor)
	{
		return FallbackDistance;
	}

	if (MaxTraceDistance <= KINDA_SMALL_NUMBER)
	{
		return FallbackDistance;
	}

	const FVector StartLocation = Meteor->GetActorLocation();
	const FVector Direction = Meteor->GetActorForwardVector().GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		return FallbackDistance;
	}

	if (const UWorld* World = GetWorld())
	{
		float ClosestFighterDistance = MaxTraceDistance;
		bool bHasFighterDistance = false;

		for (TActorIterator<AZD_FighterJet> It(World); It; ++It)
		{
			const AZD_FighterJet* FighterJet = *It;
			if (!FighterJet)
			{
				continue;
			}

			const float ProjectedDistance = FVector::DotProduct(FighterJet->GetActorLocation() - StartLocation, Direction);
			if (ProjectedDistance > 0.0f && ProjectedDistance < ClosestFighterDistance)
			{
				ClosestFighterDistance = ProjectedDistance;
				bHasFighterDistance = true;
			}
		}

		if (bHasFighterDistance)
		{
			return FMath::Max(KINDA_SMALL_NUMBER, ClosestFighterDistance);
		}

		const FVector EndLocation = StartLocation + Direction * MaxTraceDistance;

		FHitResult HitResult;
		const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CosmicMeteorsLevelTrace), false, Meteor);
		if (World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, QueryParams))
		{
			return FMath::Max(KINDA_SMALL_NUMBER, HitResult.Distance);
		}
	}

	return FallbackDistance;
}
