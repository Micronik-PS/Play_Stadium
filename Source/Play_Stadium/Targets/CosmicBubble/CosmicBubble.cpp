#include "CosmicBubble.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "PaperFlipbookComponent.h"
#include "Play_Stadium/Core/UI/CosmicBubbleTextWidget/CosmicBubbleTextWidget.h"
#include "Play_Stadium/Projectiles/BlasterGunProjectile/BlasterGunProjectile.h"

ACosmicBubble::ACosmicBubble()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Root = CollisionComponent;
	SetRootComponent(CollisionComponent);
	CollisionComponent->InitSphereRadius(CollisionRadius);
	CollisionComponent->SetMobility(EComponentMobility::Movable);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACosmicBubble::HandleBubbleHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ACosmicBubble::HandleBubbleOverlap);

	BubbleFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("BubbleFlipbook"));
	BubbleFlipbookComponent->SetupAttachment(CollisionComponent);
	BubbleFlipbookComponent->SetMobility(EComponentMobility::Movable);
	BubbleFlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BubbleWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("BubbleWidget"));
	BubbleWidgetComponent->SetupAttachment(CollisionComponent);
	BubbleWidgetComponent->SetMobility(EComponentMobility::Movable);
	BubbleWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BubbleWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	BubbleWidgetComponent->SetDrawAtDesiredSize(true);
	BubbleWidgetComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	BubbleWidgetComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));
	BubbleWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	BubbleWidgetComponent->SetTranslucentSortPriority(1);

	BubbleWidgetClass = UCosmicBubbleTextWidget::StaticClass();
	BubbleWidgetComponent->SetWidgetClass(BubbleWidgetClass);
}

void ACosmicBubble::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (CollisionComponent)
	{
		CollisionComponent->SetSphereRadius(CollisionRadius);
	}

	if (BubbleFlipbookComponent && BubbleFlipbookComponent->GetAttachParent() != CollisionComponent)
	{
		BubbleFlipbookComponent->AttachToComponent(CollisionComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	if (BubbleWidgetComponent && BubbleWidgetComponent->GetAttachParent() != CollisionComponent)
	{
		BubbleWidgetComponent->AttachToComponent(CollisionComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	SetMovementSpeed(MovementSpeed);
	UpdateText();
}

void ACosmicBubble::BeginPlay()
{
	Super::BeginPlay();

	SetMovementSpeed(MovementSpeed);

	if (BubbleFlipbookComponent && BubbleFlipbookComponent->GetAttachParent() != CollisionComponent)
	{
		BubbleFlipbookComponent->AttachToComponent(CollisionComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	if (BubbleWidgetComponent && BubbleWidgetComponent->GetAttachParent() != CollisionComponent)
	{
		BubbleWidgetComponent->AttachToComponent(CollisionComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	UpdateText();
}

void ACosmicBubble::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bHasBeenDestroyed)
	{
		return;
	}

	const float ActualSpeed = FMath::Max(MovementSpeed, MinimumMovementSpeed);
	if (ActualSpeed <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	FHitResult HitResult;
	const FVector Delta = FVector(0.0f, 0.0f, -ActualSpeed * DeltaSeconds);
	AddActorWorldOffset(Delta, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		HandleBubbleImpact(HitResult.GetActor());
	}
}

#if WITH_EDITOR
void ACosmicBubble::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateText();
}
#endif

void ACosmicBubble::SetBubbleText(const FText& InText)
{
	SetChoiceData(InText, bIsCorrectChoice);
}

void ACosmicBubble::SetChoiceData(const FText& InText, bool bInIsCorrectChoice)
{
	BubbleText = InText;
	bIsCorrectChoice = bInIsCorrectChoice;
	UpdateText();
}

void ACosmicBubble::SetMovementSpeed(float InSpeed)
{
	MovementSpeed = FMath::Max(InSpeed, MinimumMovementSpeed);
}

float ACosmicBubble::GetCollisionRadius() const
{
	if (CollisionComponent)
	{
		return CollisionComponent->GetScaledSphereRadius();
	}

	return CollisionRadius;
}

void ACosmicBubble::HandleBubbleImpact(AActor* OtherActor)
{
	if (bHasBeenDestroyed)
	{
		return;
	}

	const bool bHitByProjectile = OtherActor && OtherActor->IsA(ABlasterGunProjectile::StaticClass());
	const ETargetDestroyReason DestroyReason = bHitByProjectile
		? ETargetDestroyReason::FromPlayerAttack
		: ETargetDestroyReason::FromDestinationPoint;

	OnBubbleDestroyed.Broadcast(DestroyReason, bIsCorrectChoice);
	bHasBeenDestroyed = true;
	Destroy();
}

void ACosmicBubble::HandleBubbleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HandleBubbleImpact(OtherActor);
}

void ACosmicBubble::HandleBubbleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleBubbleImpact(OtherActor);
}

void ACosmicBubble::UpdateText()
{
	if (!BubbleWidgetComponent)
	{
		return;
	}

	if (!BubbleWidgetClass)
	{
		BubbleWidgetClass = UCosmicBubbleTextWidget::StaticClass();
	}

	BubbleWidgetComponent->SetWidgetClass(BubbleWidgetClass);
	BubbleWidgetComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));
	BubbleWidgetComponent->SetTranslucentSortPriority(1);

	if (!BubbleWidgetComponent->GetWidget())
	{
		BubbleWidgetComponent->InitWidget();
	}

	if (UCosmicBubbleTextWidget* BubbleTextWidget = Cast<UCosmicBubbleTextWidget>(BubbleWidgetComponent->GetWidget()))
	{
		const int32 FontSize = FMath::Max(1, FMath::RoundToInt(TextWorldSize));
		BubbleTextWidget->SetFontSize(FontSize);
		BubbleTextWidget->SetMaxLineLength(MaxCharactersPerLine);
		BubbleTextWidget->SetBubbleText(BubbleText);
	}
}
