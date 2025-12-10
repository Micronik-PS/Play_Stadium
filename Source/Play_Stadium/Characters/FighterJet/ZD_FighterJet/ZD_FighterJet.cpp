#include "ZD_FighterJet.h"

#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Play_Stadium/Characters/FighterJet/ZDA_FighterJet/ZDA_FighterJet.h"
#include "Play_Stadium/Projectiles/FighterJetProjectile/FighterJetProjectile.h"

AZD_FighterJet::AZD_FighterJet()
{
	PrimaryActorTick.bCanEverTick = true;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
		Movement->GravityScale = 0.0f;
	}
}

void AZD_FighterJet::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
		Movement->GravityScale = 0.0f;
	}
}

void AZD_FighterJet::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AZD_FighterJet::Fire);
		}

		if (RotateAction)
		{
			EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &AZD_FighterJet::Rotate);
		}
	}
}

void AZD_FighterJet::Fire()
{
	if (!CanFire() || !ProjectileClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	
	const FVector FireDirection = GetActorForwardVector(); // fire along jet nose (local +X)
	const FRotator SpawnRotation = FireDirection.Rotation();
	const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	if (AActor* SpawnedProjectile = World->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams))
	{
		if (AFighterJetProjectile* FighterJetProjectile = Cast<AFighterJetProjectile>(SpawnedProjectile))
		{
			FighterJetProjectile->SetFireDirection(FireDirection);
		}

		bCanFire = false;

		if (FireCooldown > KINDA_SMALL_NUMBER)
		{
			World->GetTimerManager().SetTimer(FireCooldownTimerHandle, this, &AZD_FighterJet::ResetFireCooldown, FireCooldown, false);
		}
		else
		{
			ResetFireCooldown();
		}

		TriggerAnimFireEvent();
	}
}

void AZD_FighterJet::Rotate(const FInputActionValue& Value)
{
	const float InputValue = Value.Get<float>();
	if (FMath::IsNearlyZero(InputValue))
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		const float DeltaPitch = -InputValue * RotationDegreesPerSecond * World->GetDeltaSeconds();
		AddActorLocalRotation(FRotator(DeltaPitch, 0.0f, 0.0f));

		if (AController* LocalController = GetController())
		{
			LocalController->SetControlRotation(GetActorRotation());
		}
	}
}

void AZD_FighterJet::ResetFireCooldown()
{
	bCanFire = true;
}

void AZD_FighterJet::TriggerAnimFireEvent()
{
	if (UPaperZDAnimInstance* ZDAnimInstance = GetAnimInstance())
	{
		if (UZDA_FighterJet* FighterJetAnimInstance = Cast<UZDA_FighterJet>(ZDAnimInstance))
		{
			FighterJetAnimInstance->HandleFighterJetFired();
		}
	}
}

bool AZD_FighterJet::CanFire() const
{
	return bCanFire;
}
