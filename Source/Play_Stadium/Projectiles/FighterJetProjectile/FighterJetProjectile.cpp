#include "FighterJetProjectile.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PaperSpriteComponent.h"

AFighterJetProjectile::AFighterJetProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->InitSphereRadius(CollisionRadius);
	CollisionComponent->SetRelativeLocation(CollisionOffset);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &AFighterJetProjectile::HandleProjectileHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AFighterJetProjectile::HandleProjectileOverlap);

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetupAttachment(CollisionComponent);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
}

void AFighterJetProjectile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyCollisionSettings();

	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->InitialSpeed = InitialSpeed;
		ProjectileMovementComponent->MaxSpeed = InitialSpeed;
	}
}

void AFighterJetProjectile::BeginPlay()
{
	Super::BeginPlay();

	ApplyCollisionSettings();

	ApplyInitialVelocity();

	if (ProjectileSprite && SpriteComponent)
	{
		SpriteComponent->SetSprite(ProjectileSprite);
	}
}

void AFighterJetProjectile::ApplyCollisionSettings() const
{
	if (CollisionComponent)
	{
		CollisionComponent->SetSphereRadius(CollisionRadius);
		CollisionComponent->SetRelativeLocation(CollisionOffset);
	}
}

void AFighterJetProjectile::DestroySelf()
{
	if (bHasImpacted)
	{
		return;
	}

	bHasImpacted = true;
	Destroy();
}

void AFighterJetProjectile::HandleProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	DestroySelf();
}

void AFighterJetProjectile::HandleProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	DestroySelf();
}

void AFighterJetProjectile::SetFireDirection(const FVector& InDirection)
{
	FireDirectionOverride = InDirection.GetSafeNormal();
	ApplyInitialVelocity();
}

void AFighterJetProjectile::ApplyInitialVelocity() const
{
	if (!ProjectileMovementComponent)
	{
		return;
	}

	const FVector Direction = FireDirectionOverride.IsNearlyZero()
		? FVector::XAxisVector
		: FireDirectionOverride;

	ProjectileMovementComponent->Velocity = Direction * InitialSpeed;
}
