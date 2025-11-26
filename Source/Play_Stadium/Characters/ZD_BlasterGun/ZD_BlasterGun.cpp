#include "ZD_BlasterGun.h"

#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Math/RotationMatrix.h"

void AZD_BlasterGun::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AZD_BlasterGun::MoveHorizontal);
        }

        if (FireAction)
        {
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AZD_BlasterGun::Fire);
        }
    }
}

void AZD_BlasterGun::MoveHorizontal(const FInputActionValue& Value)
{
    const float Direction = Value.Get<float>();
    if (FMath::IsNearlyZero(Direction))
    {
        return;
    }

    const float TargetYaw = Direction > 0.0f ? 0.0f : 180.0f;
    const FRotator FacingRotation(0.0f, TargetYaw, 0.0f);
    SetActorRotation(FacingRotation);

    if (AController* LocalController = GetController())
    {
        LocalController->SetControlRotation(FacingRotation);
    }

    AddMovementInput(FacingRotation.Vector(), FMath::Abs(Direction));
}

void AZD_BlasterGun::Fire()
{
    if (!ProjectileClass)
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        const FRotator SpawnRotation = FRotationMatrix::MakeFromX(FVector::UpVector).Rotator();
        const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();

        World->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
    }
}
