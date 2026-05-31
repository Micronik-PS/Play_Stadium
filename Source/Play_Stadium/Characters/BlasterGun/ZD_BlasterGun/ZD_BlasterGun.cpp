#include "ZD_BlasterGun.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Math/RotationMatrix.h"
#include "../ZDA_BlasterGun/ZDA_BlasterGun.h"

AZD_BlasterGun::AZD_BlasterGun()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AZD_BlasterGun::BeginPlay()
{
    Super::BeginPlay();

    if (UPaperZDAnimInstance* ZDAnimInstance = GetAnimInstance())
    {
        if (UZDA_BlasterGun* BlasterGunAnimInstance = Cast<UZDA_BlasterGun>(ZDAnimInstance))
        {
            if (!OnBlasterGunFired.IsAlreadyBound(BlasterGunAnimInstance, &UZDA_BlasterGun::HandleBlasterGunFired))
            {
                OnBlasterGunFired.AddDynamic(BlasterGunAnimInstance, &UZDA_BlasterGun::HandleBlasterGunFired);
            }
        }
    }
}

void AZD_BlasterGun::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bHasActiveTouchMovement)
    {
        MoveHorizontalByDirection(ActiveTouchMoveDirection);
    }
}

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

    if (PlayerInputComponent)
    {
        PlayerInputComponent->BindTouch(IE_Pressed, this, &AZD_BlasterGun::HandleTouchPressed);
        PlayerInputComponent->BindTouch(IE_Repeat, this, &AZD_BlasterGun::HandleTouchMoved);
        PlayerInputComponent->BindTouch(IE_Released, this, &AZD_BlasterGun::HandleTouchReleased);
    }
}

void AZD_BlasterGun::MoveHorizontal(const FInputActionValue& Value)
{
    MoveHorizontalByDirection(Value.Get<float>());
}

void AZD_BlasterGun::MoveHorizontalByDirection(float Direction)
{
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

        if (AActor* SpawnedProjectile = World->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams))
        {
            OnBlasterGunFired.Broadcast();
        }
    }
}

void AZD_BlasterGun::HandleTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
    if (bHasActiveTouchMovement && ActiveMovementTouchIndex != FingerIndex)
    {
        Fire();
        return;
    }

    if (IsTouchInFireArea(Location))
    {
        Fire();
        return;
    }

    StartTouchMovement(FingerIndex, Location);
}

void AZD_BlasterGun::HandleTouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
    if (bHasActiveTouchMovement && ActiveMovementTouchIndex == FingerIndex)
    {
        UpdateTouchMovement(Location);
    }
}

void AZD_BlasterGun::HandleTouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{
    if (bHasActiveTouchMovement && ActiveMovementTouchIndex == FingerIndex)
    {
        bHasActiveTouchMovement = false;
        ActiveMovementTouchIndex = ETouchIndex::MAX_TOUCHES;
        ActiveTouchMoveDirection = 0.0f;
    }
}

void AZD_BlasterGun::StartTouchMovement(ETouchIndex::Type FingerIndex, const FVector& Location)
{
    bHasActiveTouchMovement = true;
    ActiveMovementTouchIndex = FingerIndex;
    UpdateTouchMovement(Location);
}

void AZD_BlasterGun::UpdateTouchMovement(const FVector& Location)
{
    FVector2D ViewportSize;
    if (!GetViewportSize(ViewportSize) || ViewportSize.X <= 0.0f)
    {
        ActiveTouchMoveDirection = 0.0f;
        return;
    }

    ActiveTouchMoveDirection = Location.X < ViewportSize.X * 0.5f ? -1.0f : 1.0f;
}

bool AZD_BlasterGun::IsTouchInFireArea(const FVector& Location) const
{
    FVector2D ViewportSize;
    if (!GetViewportSize(ViewportSize) || ViewportSize.Y <= 0.0f)
    {
        return false;
    }

    return Location.Y <= ViewportSize.Y * FireAreaScreenHeightRatio;
}

bool AZD_BlasterGun::GetViewportSize(FVector2D& OutViewportSize) const
{
    const APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController)
    {
        return false;
    }

    int32 ViewportWidth = 0;
    int32 ViewportHeight = 0;
    PlayerController->GetViewportSize(ViewportWidth, ViewportHeight);
    if (ViewportWidth <= 0 || ViewportHeight <= 0)
    {
        return false;
    }

    OutViewportSize = FVector2D(ViewportWidth, ViewportHeight);
    return true;
}
