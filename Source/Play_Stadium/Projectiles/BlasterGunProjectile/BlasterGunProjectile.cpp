#include "BlasterGunProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

ABlasterGunProjectile::ABlasterGunProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    CollisionComponent->SetBoxExtent(FVector(8.0f, 8.0f, 8.0f));
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->SetNotifyRigidBodyCollision(true);
    CollisionComponent->SetGenerateOverlapEvents(true);
    SetRootComponent(CollisionComponent);
    CollisionComponent->OnComponentHit.AddDynamic(this, &ABlasterGunProjectile::OnProjectileHit);

    ProjectileSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
    ProjectileSpriteComponent->SetupAttachment(RootComponent);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComponent->InitialSpeed = 1500.0f;
    ProjectileMovementComponent->MaxSpeed = 1500.0f;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
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

void ABlasterGunProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this || !OtherComp)
    {
        return;
    }

    Destroy();
}
