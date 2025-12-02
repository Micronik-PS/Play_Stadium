#include "BlasterGunProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABlasterGunProjectile::ABlasterGunProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Root = CollisionComponent;
	SetRootComponent(CollisionComponent);
	CollisionComponent->InitSphereRadius(CollisionRadius);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABlasterGunProjectile::HandleProjectileHit);

	ProjectileSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	ProjectileSpriteComponent->SetupAttachment(RootComponent);
	ProjectileSpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->InitialSpeed = 1500.0f;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
}

void ABlasterGunProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileSprite && ProjectileSpriteComponent)
	{
		ProjectileSpriteComponent->SetSprite(ProjectileSprite);
	}

	if (ProjectileMovementComponent)
	{
		const FVector Direction = GetActorForwardVector();
		ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
	}
}

void ABlasterGunProjectile::HandleProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bHasImpacted)
	{
		return;
	}

	bHasImpacted = true;
	OnProjectileHit.Broadcast();

	if (bDestroyOnImpact)
	{
		Destroy();
	}
}
