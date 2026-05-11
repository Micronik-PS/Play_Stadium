#include "Play_Stadium/Vicinity/TextInputVicinity/CosmicStars/CosmicSectorSpawner/CosmicSectorSpawner.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Play_Stadium/Targets/CosmicSector/CosmicSector.h"

ACosmicSectorSpawner::ACosmicSectorSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SpawnRangePreview = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnRangePreview"));
	SpawnRangePreview->SetupAttachment(RootComponent);
	SpawnRangePreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnRangePreview->SetGenerateOverlapEvents(false);
	SpawnRangePreview->SetHiddenInGame(true);
	SpawnRangePreview->SetVisibility(true);
	SpawnRangePreview->ShapeColor = FColor(80, 180, 255, 180);

	UpdateSpawnRangePreview();
}

void ACosmicSectorSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateSpawnRangePreview();
}

#if WITH_EDITOR
void ACosmicSectorSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateSpawnRangePreview();
}
#endif

void ACosmicSectorSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnEnabled)
	{
		StartSpawnTimer();
	}
}

void ACosmicSectorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACosmicSectorSpawner::SetSpawnEnabled(bool bInEnabled)
{
	bSpawnEnabled = bInEnabled;

	if (bSpawnEnabled)
	{
		StartSpawnTimer();
	}
	else
	{
		StopSpawnTimer();
	}
}

void ACosmicSectorSpawner::ConfigureQuestionLetters(const FString& CorrectAnswer)
{
	TargetWord = CorrectAnswer.ToUpper();
	TargetLetters = ExtractUniqueLetters(TargetWord);
	MissingLetters = TargetLetters;
}

void ACosmicSectorSpawner::SetMissingLetters(const TArray<FString>& InMissingLetters)
{
	MissingLetters.Reset();

	for (const FString& Letter : InMissingLetters)
	{
		const FString NormalizedLetter = Letter.ToUpper();
		if (!NormalizedLetter.IsEmpty() && ContainsLetter(TargetLetters, NormalizedLetter))
		{
			MissingLetters.AddUnique(NormalizedLetter);
		}
	}
}

void ACosmicSectorSpawner::ClearSpawnedSectors()
{
	for (const TWeakObjectPtr<ACosmicSector>& SectorPtr : SpawnedSectors)
	{
		if (ACosmicSector* Sector = SectorPtr.Get())
		{
			Sector->Destroy();
		}
	}

	SpawnedSectors.Reset();
}

ACosmicSector* ACosmicSectorSpawner::SpawnSector()
{
	if (!SectorClass)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FString Letter;
	bool bIsCorrectLetter = false;
	if (!ChooseLetter(Letter, bIsCorrectLetter) || Letter.IsEmpty())
	{
		return nullptr;
	}

	const float HorizontalOffset = HorizontalSpawnRange > KINDA_SMALL_NUMBER
		? FMath::RandRange(-HorizontalSpawnRange, HorizontalSpawnRange)
		: 0.0f;

	const FVector SpawnLocation = GetActorLocation() + GetActorForwardVector().GetSafeNormal() * HorizontalOffset;
	const FTransform SpawnTransform(GetActorRotation(), SpawnLocation, GetActorScale3D());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ACosmicSector* NewSector = World->SpawnActor<ACosmicSector>(SectorClass, SpawnTransform, SpawnParams);
	if (!NewSector)
	{
		return nullptr;
	}

	NewSector->SetLetterData(Letter, bIsCorrectLetter);
	NewSector->SetMovementSpeed(SpawnedSectorMovementSpeed);
	SpawnedSectors.Add(NewSector);
	return NewSector;
}

void ACosmicSectorSpawner::UpdateSpawnRangePreview() const
{
	if (!SpawnRangePreview)
	{
		return;
	}

	const float SafeHorizontalRange = FMath::Max(0.0f, HorizontalSpawnRange);
	const float SafePreviewDepth = FMath::Max(1.0f, PreviewDepth);
	const float SafePreviewHeight = FMath::Max(1.0f, PreviewHeight);

	SpawnRangePreview->SetBoxExtent(FVector(SafeHorizontalRange, SafePreviewDepth, SafePreviewHeight));
	SpawnRangePreview->SetRelativeLocation(FVector::ZeroVector);
	SpawnRangePreview->SetRelativeRotation(FRotator::ZeroRotator);
	SpawnRangePreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnRangePreview->SetHiddenInGame(true);
	SpawnRangePreview->SetVisibility(true);
}

void ACosmicSectorSpawner::StartSpawnTimer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	StopSpawnTimer();
	SpawnSector();
	World->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ACosmicSectorSpawner::SpawnSectorFromTimer, FMath::Max(0.05f, SpawnPeriod), true);
}

void ACosmicSectorSpawner::StopSpawnTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

void ACosmicSectorSpawner::SpawnSectorFromTimer()
{
	if (bSpawnEnabled)
	{
		SpawnSector();
	}
}

bool ACosmicSectorSpawner::ChooseLetter(FString& OutLetter, bool& bOutIsCorrectLetter) const
{
	OutLetter.Reset();
	bOutIsCorrectLetter = false;

	const bool bCanSpawnCorrect = !MissingLetters.IsEmpty();
	const bool bShouldSpawnCorrect = bCanSpawnCorrect && FMath::FRand() <= FMath::Clamp(CorrectLetterProbability, 0.0f, 1.0f);

	if (bShouldSpawnCorrect && ChooseRandomCorrectLetter(OutLetter))
	{
		bOutIsCorrectLetter = true;
		return true;
	}

	if (ChooseRandomIncorrectLetter(OutLetter))
	{
		bOutIsCorrectLetter = false;
		return true;
	}

	if (bCanSpawnCorrect && ChooseRandomCorrectLetter(OutLetter))
	{
		bOutIsCorrectLetter = true;
		return true;
	}

	return false;
}

bool ACosmicSectorSpawner::ChooseRandomCorrectLetter(FString& OutLetter) const
{
	if (MissingLetters.IsEmpty())
	{
		return false;
	}

	OutLetter = MissingLetters[FMath::RandRange(0, MissingLetters.Num() - 1)].ToUpper();
	return !OutLetter.IsEmpty();
}

bool ACosmicSectorSpawner::ChooseRandomIncorrectLetter(FString& OutLetter) const
{
	TArray<FString> CandidateLetters = ExtractUniqueLetters(IncorrectLetterPool);
	for (int32 LetterIndex = CandidateLetters.Num() - 1; LetterIndex >= 0; --LetterIndex)
	{
		if (ContainsLetter(TargetLetters, CandidateLetters[LetterIndex]))
		{
			CandidateLetters.RemoveAtSwap(LetterIndex);
		}
	}

	if (CandidateLetters.IsEmpty())
	{
		return false;
	}

	OutLetter = CandidateLetters[FMath::RandRange(0, CandidateLetters.Num() - 1)].ToUpper();
	return !OutLetter.IsEmpty();
}

TArray<FString> ACosmicSectorSpawner::ExtractUniqueLetters(const FString& Source) const
{
	TArray<FString> Letters;
	Letters.Reserve(Source.Len());

	for (int32 CharacterIndex = 0; CharacterIndex < Source.Len(); ++CharacterIndex)
	{
		const TCHAR Character = Source[CharacterIndex];
		if (FChar::IsWhitespace(Character))
		{
			continue;
		}

		const FString Letter = FString::Chr(Character).ToUpper();
		if (!ContainsLetter(Letters, Letter))
		{
			Letters.Add(Letter);
		}
	}

	return Letters;
}

bool ACosmicSectorSpawner::ContainsLetter(const TArray<FString>& Letters, const FString& Letter) const
{
	for (const FString& ExistingLetter : Letters)
	{
		if (ExistingLetter.Equals(Letter, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}

	return false;
}

