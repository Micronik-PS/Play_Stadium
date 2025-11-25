#include "ZD_BlasterGun.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/Controller.h"

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
    const FRotator FacingRotation(0.0f, TargetYaw, 0.0f);
    SetActorRotation(FacingRotation);

    if (AController* Controller = GetController())
    {
        Controller->SetControlRotation(FacingRotation);
    }

    AddMovementInput(FacingRotation.Vector(), FMath::Abs(Direction));
}
