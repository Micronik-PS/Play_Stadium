#include "Meteor.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PaperFlipbookComponent.h"
#include "Play_Stadium/Characters/FighterJet/ZD_FighterJet/ZD_FighterJet.h"
#include "Play_Stadium/Core/UI/CosmicBubbleTextWidget/CosmicBubbleTextWidget.h"
#include "Play_Stadium/Projectiles/FighterJetProjectile/FighterJetProjectile.h"
#include "EngineUtils.h"

AMeteor::AMeteor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->InitSphereRadius(CollisionRadius);
	CollisionComponent->SetRelativeLocation(CollisionOffset);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &AMeteor::HandleMeteorHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMeteor::HandleMeteorOverlap);

	MeteorFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	MeteorFlipbookComponent->SetupAttachment(CollisionComponent);
	MeteorFlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TextWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TextWidget"));
	TextWidgetComponent->SetupAttachment(CollisionComponent);
	TextWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TextWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	TextWidgetComponent->SetDrawAtDesiredSize(true);
	TextWidgetComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	TextWidgetComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));
	TextWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	TextWidgetComponent->SetTranslucentSortPriority(1);

	TextWidgetClass = UCosmicBubbleTextWidget::StaticClass();
	TextWidgetComponent->SetWidgetClass(TextWidgetClass);

	CacheTextWidgetRelativeTransform();
}

void AMeteor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyCollisionSettings();

	if (MeteorFlipbookComponent && MeteorFlipbookComponent->GetAttachParent() != CollisionComponent)
	{
		MeteorFlipbookComponent->AttachToComponent(CollisionComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	if (TextWidgetComponent && TextWidgetComponent->GetAttachParent() != CollisionComponent)
	{
		TextWidgetComponent->AttachToComponent(CollisionComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	CacheTextWidgetRelativeTransform();
	UpdateText();
}

void AMeteor::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(true);
	ApplyCollisionSettings();

	if (MeteorFlipbookComponent && MeteorFlipbookComponent->GetAttachParent() != CollisionComponent)
	{
		MeteorFlipbookComponent->AttachToComponent(CollisionComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	if (TextWidgetComponent && TextWidgetComponent->GetAttachParent() != CollisionComponent)
	{
		TextWidgetComponent->AttachToComponent(CollisionComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	CacheTextWidgetRelativeTransform();
	UpdateText();
}

void AMeteor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bHasBeenDestroyed)
	{
		return;
	}

	FVector MoveDirection = GetActorForwardVector().GetSafeNormal();
	if (MoveDirection.IsNearlyZero())
	{
		MoveDirection = FVector::XAxisVector;
	}

	if (const UWorld* World = GetWorld())
	{
		float ClosestDistanceSq = MAX_FLT;
		for (TActorIterator<AZD_FighterJet> It(World); It; ++It)
		{
			const AZD_FighterJet* FighterJet = *It;
			if (!FighterJet)
			{
				continue;
			}

			const float DistanceSq = FVector::DistSquared(FighterJet->GetActorLocation(), GetActorLocation());
			if (DistanceSq < ClosestDistanceSq)
			{
				const FVector DirectionToFighter = (FighterJet->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				if (!DirectionToFighter.IsNearlyZero())
				{
					ClosestDistanceSq = DistanceSq;
					MoveDirection = DirectionToFighter;
				}
			}
		}
	}

	if (MoveDirection.IsNearlyZero())
	{
		return;
	}

	const float ActualSpeed = GetSafeMovementSpeed();
	if (ActualSpeed <= 0.0f)
	{
		return;
	}

	FHitResult HitResult;
	const FVector Delta = MoveDirection * ActualSpeed * DeltaSeconds;
	AddActorWorldOffset(Delta, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->IsA(AFighterJetProjectile::StaticClass()))
		{
			HandleMeteorImpact(HitActor, ETargetDestroyReason::FromPlayerAttack);
		}
		else if (HitActor && HitActor->IsA(AZD_FighterJet::StaticClass()))
		{
			HandleMeteorImpact(HitActor, ETargetDestroyReason::FromDestinationPoint);
		}
		else
		{
			HandleMeteorImpact(HitActor, ETargetDestroyReason::FromDestinationPoint);
		}
	}
}

#if WITH_EDITOR
void AMeteor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ApplyCollisionSettings();
	CacheTextWidgetRelativeTransform();
	UpdateText();
}
#endif

void AMeteor::SetMeteorText(const FText& InText)
{
	SetChoiceData(InText, bIsCorrectChoice);
}

void AMeteor::SetChoiceData(const FText& InText, bool bInIsCorrectChoice)
{
	MeteorText = InText;
	bIsCorrectChoice = bInIsCorrectChoice;
	UpdateText();
}

void AMeteor::SetMovementSpeed(float InSpeed)
{
	const float SafeMinimum = GetMinimumMovementSpeed();
	MovementSpeed = FMath::Max(InSpeed, SafeMinimum);
	MinimumMovementSpeed = SafeMinimum;
	SetActorTickEnabled(true);
}

float AMeteor::GetMinimumMovementSpeed() const
{
	return FMath::Max(MinimumMovementSpeed, 1.0f);
}

FTransform AMeteor::GetTextWidgetRelativeTransform() const
{
	if (TextWidgetComponent)
	{
		return TextWidgetComponent->GetRelativeTransform();
	}

	return CachedTextWidgetRelativeTransform;
}

void AMeteor::SetTextWidgetRelativeTransform(const FTransform& InTransform)
{
	CachedTextWidgetRelativeTransform = InTransform;
	TextDepthOffset = InTransform.GetLocation().Y;

	if (TextWidgetComponent)
	{
		TextWidgetComponent->SetRelativeTransform(InTransform);
	}
}

void AMeteor::TriggerDestruction(ETargetDestroyReason DestroyReason)
{
	if (bHasBeenDestroyed)
	{
		return;
	}

	bHasBeenDestroyed = true;
	OnMeteorDestroyed.Broadcast(DestroyReason, bIsCorrectChoice);
	Destroy();
}

void AMeteor::UpdateText()
{
	if (!TextWidgetComponent)
	{
		return;
	}

	if (!TextWidgetClass)
	{
		TextWidgetClass = UCosmicBubbleTextWidget::StaticClass();
	}

	TextWidgetComponent->SetWidgetClass(TextWidgetClass);
	TextWidgetComponent->SetRelativeTransform(CachedTextWidgetRelativeTransform);
	TextWidgetComponent->SetTranslucentSortPriority(1);

	if (!TextWidgetComponent->GetWidget())
	{
		TextWidgetComponent->InitWidget();
	}

	if (UCosmicBubbleTextWidget* BubbleTextWidget = Cast<UCosmicBubbleTextWidget>(TextWidgetComponent->GetWidget()))
	{
		const int32 FontSize = FMath::Max(1, FMath::RoundToInt(TextWorldSize));
		BubbleTextWidget->SetFontSize(FontSize);
		BubbleTextWidget->SetMaxLineLength(MaxCharactersPerLine);
		BubbleTextWidget->SetBubbleText(MeteorText);
	}
}

void AMeteor::HandleMeteorImpact(AActor* OtherActor, ETargetDestroyReason DestroyReason)
{
	if (!OtherActor || bHasBeenDestroyed)
	{
		return;
	}

	TriggerDestruction(DestroyReason);
}

void AMeteor::ApplyCollisionSettings() const
{
	if (CollisionComponent)
	{
		CollisionComponent->SetSphereRadius(CollisionRadius);
		CollisionComponent->SetRelativeLocation(CollisionOffset);
	}
}

void AMeteor::HandleMeteorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (OtherActor->IsA(AFighterJetProjectile::StaticClass()))
	{
		HandleMeteorImpact(OtherActor, ETargetDestroyReason::FromPlayerAttack);
		return;
	}

	if (OtherActor->IsA(AZD_FighterJet::StaticClass()))
	{
		HandleMeteorImpact(OtherActor, ETargetDestroyReason::FromDestinationPoint);
		return;
	}

	HandleMeteorImpact(OtherActor, ETargetDestroyReason::FromDestinationPoint);
}

void AMeteor::CacheTextWidgetRelativeTransform()
{
	if (!TextWidgetComponent)
	{
		return;
	}

	CachedTextWidgetRelativeTransform = TextWidgetComponent->GetRelativeTransform();
	TextDepthOffset = CachedTextWidgetRelativeTransform.GetLocation().Y;
}

float AMeteor::GetSafeMovementSpeed() const
{
	return FMath::Max(MovementSpeed, GetMinimumMovementSpeed());
}

void AMeteor::HandleMeteorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (OtherActor->IsA(AFighterJetProjectile::StaticClass()))
	{
		HandleMeteorImpact(OtherActor, ETargetDestroyReason::FromPlayerAttack);
		return;
	}

	if (OtherActor->IsA(AZD_FighterJet::StaticClass()))
	{
		HandleMeteorImpact(OtherActor, ETargetDestroyReason::FromDestinationPoint);
		return;
	}

	HandleMeteorImpact(OtherActor, ETargetDestroyReason::FromDestinationPoint);
}
