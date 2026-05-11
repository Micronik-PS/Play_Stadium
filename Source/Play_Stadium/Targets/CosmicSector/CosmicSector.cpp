#include "Play_Stadium/Targets/CosmicSector/CosmicSector.h"

#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInterface.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ACosmicSector::ACosmicSector()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetBoxExtent(CollisionBoxExtent);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACosmicSector::HandleSectorHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ACosmicSector::HandleSectorOverlap);

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetupAttachment(CollisionComponent);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BreakingFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("BreakingFlipbook"));
	BreakingFlipbookComponent->SetupAttachment(CollisionComponent);
	BreakingFlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BreakingFlipbookComponent->SetLooping(false);
	BreakingFlipbookComponent->SetHiddenInGame(true);
	BreakingFlipbookComponent->SetVisibility(false);

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text"));
	TextComponent->SetupAttachment(CollisionComponent);
	TextComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	TextComponent->SetTextRenderColor(TextColor);
	TextComponent->SetWorldSize(TextWorldSize);
	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	TextComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));
	TextComponent->SetHiddenInGame(false);
	TextComponent->SetVisibility(true);
	TextComponent->SetTranslucentSortPriority(TextTranslucentSortPriority);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> UnlitTextMat(TEXT("/Engine/EngineMaterials/DefaultTextMaterialOpaque"));
	if (UnlitTextMat.Succeeded())
	{
		TextComponent->SetTextMaterial(UnlitTextMat.Object);
	}

	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> BreakingFlipbookAsset(TEXT("/Game/Play_Stadium/Targets/CosmicSectors/Animation/Breaking/FB_CosmicSector_Breaking.FB_CosmicSector_Breaking"));
	if (BreakingFlipbookAsset.Succeeded())
	{
		BreakingFlipbook = BreakingFlipbookAsset.Object;
	}
}

void ACosmicSector::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyCollisionSettings();
	ApplyRandomSprite();
	ApplyBreakingFlipbook();
	ApplyTextLayout();
	UpdateText();
}

void ACosmicSector::BeginPlay()
{
	Super::BeginPlay();

	bHasBeenDestroyed = false;
	SetActorTickEnabled(true);
	ApplyCollisionSettings();
	ApplyRandomSprite();
	ApplyBreakingFlipbook();
	ApplyTextLayout();
	UpdateText();
}

void ACosmicSector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasBeenDestroyed || MovementSpeed <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	FHitResult HitResult;
	const FVector Delta = FVector(0.0f, 0.0f, -MovementSpeed * DeltaTime);
	AddActorWorldOffset(Delta, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		HandleSectorImpact(HitResult.GetActor());
	}
}

void ACosmicSector::SetSectorText(const FText& InText)
{
	SectorText = InText;
	Letter = InText.ToString();
	UpdateText();
}

void ACosmicSector::SetLetterData(const FString& InLetter, bool bInIsCorrectLetter)
{
	Letter = InLetter;
	SectorText = FText::FromString(InLetter);
	bIsCorrectLetter = bInIsCorrectLetter;
	UpdateText();
}

void ACosmicSector::SetMovementSpeed(float InSpeed)
{
	MovementSpeed = FMath::Max(0.0f, InSpeed);
	SetActorTickEnabled(MovementSpeed > KINDA_SMALL_NUMBER && !bHasBeenDestroyed);
}

void ACosmicSector::TriggerDestruction(ETargetDestroyReason DestroyReason)
{
	if (bHasBeenDestroyed)
	{
		return;
	}

	bHasBeenDestroyed = true;
	SetActorTickEnabled(false);

	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (SpriteComponent)
	{
		SpriteComponent->SetHiddenInGame(true);
		SpriteComponent->SetVisibility(false);
	}

	if (TextComponent)
	{
		TextComponent->SetHiddenInGame(true);
		TextComponent->SetVisibility(false);
	}

	OnSectorDestroyed.Broadcast(DestroyReason, bIsCorrectLetter, Letter);

	float DestroyDelay = FMath::Max(0.0f, FallbackDestroyDelay);
	if (BreakingFlipbookComponent && BreakingFlipbook)
	{
		BreakingFlipbookComponent->SetFlipbook(BreakingFlipbook);
		BreakingFlipbookComponent->SetLooping(false);
		BreakingFlipbookComponent->SetHiddenInGame(false);
		BreakingFlipbookComponent->SetVisibility(true);
		BreakingFlipbookComponent->PlayFromStart();
		DestroyDelay = FMath::Max(DestroyDelay, BreakingFlipbook->GetTotalDuration());
	}

	if (DestroyDelay <= KINDA_SMALL_NUMBER)
	{
		FinishDestruction();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ACosmicSector::FinishDestruction, DestroyDelay, false);
	}
	else
	{
		Destroy();
	}
}

float ACosmicSector::GetCollisionHalfHeight() const
{
	if (CollisionComponent)
	{
		return CollisionComponent->GetScaledBoxExtent().Z;
	}

	return CollisionBoxExtent.Z * GetActorScale3D().Z;
}

void ACosmicSector::ApplyCollisionSettings() const
{
	if (CollisionComponent)
	{
		CollisionComponent->SetBoxExtent(CollisionBoxExtent);
	}
}

void ACosmicSector::ApplyRandomSprite()
{
	if (!SpriteComponent || SectorSprites.IsEmpty())
	{
		return;
	}

	const int32 SpriteIndex = FMath::RandRange(0, SectorSprites.Num() - 1);
	if (UPaperSprite* SelectedSprite = SectorSprites[SpriteIndex])
	{
		SpriteComponent->SetSprite(SelectedSprite);
	}
}

void ACosmicSector::ApplyTextLayout() const
{
	if (!TextComponent)
	{
		return;
	}

	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	TextComponent->SetTextRenderColor(TextColor);
	TextComponent->SetWorldSize(FMath::Max(1.0f, TextWorldSize));
	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	TextComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));
	TextComponent->SetTranslucentSortPriority(TextTranslucentSortPriority);

	if (!bHasBeenDestroyed)
	{
		TextComponent->SetHiddenInGame(false);
		TextComponent->SetVisibility(true);
	}
}

void ACosmicSector::ApplyBreakingFlipbook() const
{
	if (!BreakingFlipbookComponent)
	{
		return;
	}

	BreakingFlipbookComponent->SetFlipbook(BreakingFlipbook);
	BreakingFlipbookComponent->SetLooping(false);

	if (!bHasBeenDestroyed)
	{
		BreakingFlipbookComponent->SetHiddenInGame(true);
		BreakingFlipbookComponent->SetVisibility(false);
	}
}

void ACosmicSector::UpdateText()
{
	if (TextComponent)
	{
		ApplyTextLayout();
		TextComponent->SetText(SectorText);
		TextComponent->MarkRenderStateDirty();
	}
}

void ACosmicSector::HandleSectorImpact(AActor* OtherActor)
{
	if (OtherActor == this || bHasBeenDestroyed)
	{
		return;
	}

	TriggerDestruction(ETargetDestroyReason::FromDestinationPoint);
}

void ACosmicSector::FinishDestruction()
{
	Destroy();
}

void ACosmicSector::HandleSectorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HandleSectorImpact(OtherActor);
}

void ACosmicSector::HandleSectorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleSectorImpact(OtherActor);
}
