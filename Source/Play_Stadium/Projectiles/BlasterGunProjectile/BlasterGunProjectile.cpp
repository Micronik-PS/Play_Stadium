#include "BlasterGunProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

ABlasterGunProjectile::ABlasterGunProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

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
