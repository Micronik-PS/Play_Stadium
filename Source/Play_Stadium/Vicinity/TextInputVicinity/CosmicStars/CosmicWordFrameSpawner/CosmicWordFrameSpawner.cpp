#include "Play_Stadium/Vicinity/TextInputVicinity/CosmicStars/CosmicWordFrameSpawner/CosmicWordFrameSpawner.h"

#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Play_Stadium/Vicinity/TextInputVicinity/CosmicStars/CosmicWordFrame/CosmicWordFrame.h"

ACosmicWordFrameSpawner::ACosmicWordFrameSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACosmicWordFrameSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ACosmicWordFrameSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACosmicWordFrameSpawner::SpawnWordFrames(const FString& Word, int32 TimeLimitInSeconds)
{
	ClearSpawnedFrames();
	CurrentWord = Word.ToUpper();

	UWorld* World = GetWorld();
	if (!World || !WordFrameClass)
	{
		return;
	}

	const TArray<FString> Letters = ExtractLetters(CurrentWord);
	if (Letters.IsEmpty())
	{
		return;
	}

	const FVector RowDirection = GetActorForwardVector().GetSafeNormal().IsNearlyZero()
		? FVector::XAxisVector
		: GetActorForwardVector().GetSafeNormal();
	const float FirstOffset = bCenterRowOnSpawner ? -0.5f * FrameSpacing * static_cast<float>(Letters.Num() - 1) : 0.0f;

	SpawnedFrames.Reserve(Letters.Num());

	for (int32 LetterIndex = 0; LetterIndex < Letters.Num(); ++LetterIndex)
	{
		const FVector SpawnLocation = GetActorLocation() + RowDirection * (FirstOffset + FrameSpacing * static_cast<float>(LetterIndex));
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation, GetActorScale3D());

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ACosmicWordFrame* NewFrame = World->SpawnActor<ACosmicWordFrame>(WordFrameClass, SpawnTransform, SpawnParams);
		if (!NewFrame)
		{
			continue;
		}

		NewFrame->InitializeFrameLetter(Letters[LetterIndex], false);
		NewFrame->OnReachedDestination.AddUniqueDynamic(this, &ACosmicWordFrameSpawner::HandleWordFrameReachedDestination);
		SpawnedFrames.Add(NewFrame);
	}

	if (SpawnedFrames.IsEmpty())
	{
		return;
	}

	const float MovementSpeed = CalculateMovementSpeed(SpawnedFrames[0], TimeLimitInSeconds);
	for (ACosmicWordFrame* Frame : SpawnedFrames)
	{
		if (Frame)
		{
			Frame->SetMovementSpeed(MovementSpeed);
		}
	}
}

void ACosmicWordFrameSpawner::ClearSpawnedFrames()
{
	for (ACosmicWordFrame* Frame : SpawnedFrames)
	{
		if (Frame)
		{
			Frame->OnReachedDestination.RemoveDynamic(this, &ACosmicWordFrameSpawner::HandleWordFrameReachedDestination);
			Frame->Destroy();
		}
	}

	SpawnedFrames.Reset();
}

int32 ACosmicWordFrameSpawner::RevealLetter(const FString& Letter)
{
	int32 RevealedCount = 0;

	for (ACosmicWordFrame* Frame : SpawnedFrames)
	{
		if (Frame && Frame->RevealIfMatches(Letter))
		{
			++RevealedCount;
		}
	}

	return RevealedCount;
}

bool ACosmicWordFrameSpawner::IsWordCompleted() const
{
	if (SpawnedFrames.IsEmpty())
	{
		return false;
	}

	for (const ACosmicWordFrame* Frame : SpawnedFrames)
	{
		if (IsValid(Frame) && !Frame->IsRevealed())
		{
			return false;
		}
	}

	return true;
}

TArray<FString> ACosmicWordFrameSpawner::GetMissingLetters() const
{
	TArray<FString> MissingLetters;

	for (const ACosmicWordFrame* Frame : SpawnedFrames)
	{
		if (!IsValid(Frame) || Frame->IsRevealed())
		{
			continue;
		}

		const FString Letter = Frame->GetExpectedLetter().ToUpper();
		if (!Letter.IsEmpty())
		{
			MissingLetters.AddUnique(Letter);
		}
	}

	return MissingLetters;
}

TArray<ACosmicWordFrame*> ACosmicWordFrameSpawner::GetSpawnedFrames() const
{
	TArray<ACosmicWordFrame*> ValidFrames;

	for (ACosmicWordFrame* Frame : SpawnedFrames)
	{
		if (IsValid(Frame))
		{
			ValidFrames.Add(Frame);
		}
	}

	return ValidFrames;
}

TArray<FString> ACosmicWordFrameSpawner::ExtractLetters(const FString& Word) const
{
	TArray<FString> Letters;
	Letters.Reserve(Word.Len());

	for (int32 CharacterIndex = 0; CharacterIndex < Word.Len(); ++CharacterIndex)
	{
		const TCHAR Character = Word[CharacterIndex];
		if (FChar::IsWhitespace(Character))
		{
			continue;
		}

		Letters.Add(FString::Chr(Character).ToUpper());
	}

	return Letters;
}

float ACosmicWordFrameSpawner::CalculateMovementSpeed(ACosmicWordFrame* ReferenceFrame, int32 TimeLimitInSeconds) const
{
	const int32 SafeTimeLimit = FMath::Max(1, TimeLimitInSeconds);
	const float DistanceToSurface = FindDistanceToSurfaceBelow(ReferenceFrame);
	const float HalfHeight = ReferenceFrame ? ReferenceFrame->GetCollisionHalfHeight() : 0.0f;
	const float TravelDistance = FMath::Max(0.0f, DistanceToSurface - HalfHeight);
	return TravelDistance / static_cast<float>(SafeTimeLimit);
}

float ACosmicWordFrameSpawner::FindDistanceToSurfaceBelow(const ACosmicWordFrame* ReferenceFrame) const
{
	if (!ReferenceFrame || MaxTraceDistance <= KINDA_SMALL_NUMBER)
	{
		return FallbackTravelDistance;
	}

	const FVector StartLocation = ReferenceFrame->GetActorLocation();
	const FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, MaxTraceDistance);

	if (const UWorld* World = GetWorld())
	{
		FHitResult HitResult;
		const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CosmicWordFrameSpawnerTrace), false, ReferenceFrame);
		if (World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, TraceChannel, QueryParams))
		{
			return FMath::Max(KINDA_SMALL_NUMBER, StartLocation.Z - HitResult.ImpactPoint.Z);
		}
	}

	return FallbackTravelDistance;
}

void ACosmicWordFrameSpawner::HandleWordFrameReachedDestination(ACosmicWordFrame* WordFrame)
{
	OnWordFrameReachedDestination.Broadcast(WordFrame);

}
