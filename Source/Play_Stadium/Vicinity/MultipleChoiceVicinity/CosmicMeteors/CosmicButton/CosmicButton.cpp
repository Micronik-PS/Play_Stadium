#include "CosmicButton.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperZDAnimationComponent.h"
#include "Play_Stadium/Projectiles/FighterJetProjectile/FighterJetProjectile.h"
#include "Play_Stadium/Targets/Meteor/Meteor.h"
#include "Play_Stadium/Targets/TargetDestroyReason.h"
#include "EngineUtils.h"
#include "ZDA_CosmicButton/ZDA_CosmicButton.h"

ACosmicButton::ACosmicButton()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetBoxExtent(CollisionBoxExtent);
	CollisionComponent->SetRelativeLocation(CollisionBoxOffset);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ACosmicButton::HandleButtonOverlap);

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	FlipbookComponent->SetupAttachment(RootComponent);
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AnimationComponent = CreateDefaultSubobject<UPaperZDAnimationComponent>(TEXT("Animation"));
}

void ACosmicButton::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyCollisionSettings();
}

void ACosmicButton::ActivateButton()
{
	TArray<AMeteor*> TargetMeteors;
	if (const UWorld* World = GetWorld())
	{
		for (TActorIterator<AMeteor> It(World); It; ++It)
		{
			if (AMeteor* MeteorActor = *It)
			{
				if (!MeteorActor->IsActorBeingDestroyed() && !MeteorActor->IsPendingKill())
				{
					TargetMeteors.Add(MeteorActor);
				}
			}
		}
	}

	for (AMeteor* Meteor : TargetMeteors)
	{
		if (Meteor)
		{
			Meteor->TriggerDestruction(ETargetDestroyReason::FromPlayerAction);
		}
	}
}

void ACosmicButton::ApplyCollisionSettings() const
{
	if (CollisionComponent)
	{
		CollisionComponent->SetBoxExtent(CollisionBoxExtent);
		CollisionComponent->SetRelativeLocation(CollisionBoxOffset);
	}
}

void ACosmicButton::HandleButtonOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(AFighterJetProjectile::StaticClass()))
	{
		if (AnimationComponent)
		{
			if (UPaperZDAnimInstance* AnimInstance = AnimationComponent->GetAnimInstance())
			{
				if (UZDA_CosmicButton* ButtonAnimInstance = Cast<UZDA_CosmicButton>(AnimInstance))
				{
					ButtonAnimInstance->HandleButtonPressed();
				}
			}
		}

		ActivateButton();
	}
}
