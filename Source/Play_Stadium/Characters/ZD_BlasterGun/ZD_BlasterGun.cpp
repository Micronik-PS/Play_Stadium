#include "ZD_BlasterGun.h"

#include "EnhancedInputComponent.h"

void AZD_BlasterGun::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AZD_BlasterGun::MoveHorizontal);
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
    SetActorRotation(FRotator(0.0f, TargetYaw, 0.0f));

    AddMovementInput(FVector::ForwardVector, Direction);
}
