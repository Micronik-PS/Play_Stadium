#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "PaperZDCharacter.h"
#include "ZD_BlasterGun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlasterGunFired);

class UInputAction;
class AActor;
class UZDA_BlasterGun;


UCLASS()
class PLAY_STADIUM_API AZD_BlasterGun : public APaperZDCharacter
{
        GENERATED_BODY()

        public:
        AZD_BlasterGun();
        virtual void BeginPlay() override;
        virtual void Tick(float DeltaSeconds) override;
        virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

        UPROPERTY(BlueprintAssignable, Category = "Combat")
        FOnBlasterGunFired OnBlasterGunFired;

        private:
        void MoveHorizontal(const FInputActionValue& Value);
        void MoveHorizontalByDirection(float Direction);
        void Fire();
        void HandleTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);
        void HandleTouchMoved(ETouchIndex::Type FingerIndex, FVector Location);
        void HandleTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);
        void StartTouchMovement(ETouchIndex::Type FingerIndex, const FVector& Location);
        void UpdateTouchMovement(const FVector& Location);
        bool IsTouchInFireArea(const FVector& Location) const;
        bool GetViewportSize(FVector2D& OutViewportSize) const;

        private:
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
        UInputAction* MoveAction = nullptr;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
        UInputAction* FireAction = nullptr;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<AActor> ProjectileClass;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
        FVector MuzzleOffset = FVector(50.0f, 0.0f, 0.0f);

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mobile Input", meta = (AllowPrivateAccess = "true", ClampMin = "0.1", ClampMax = "0.9"))
        float FireAreaScreenHeightRatio = 0.45f;

        bool bHasActiveTouchMovement = false;
        ETouchIndex::Type ActiveMovementTouchIndex = ETouchIndex::MAX_TOUCHES;
        float ActiveTouchMoveDirection = 0.0f;

};
