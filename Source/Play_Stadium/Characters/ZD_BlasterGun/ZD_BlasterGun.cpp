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

    AddMovementInput(FVector::ForwardVector, Direction);
}
