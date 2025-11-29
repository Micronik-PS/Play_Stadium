#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
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
        virtual void BeginPlay() override;
        virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

        UPROPERTY(BlueprintAssignable, Category = "Combat")
        FOnBlasterGunFired OnBlasterGunFired;

        private:
        void MoveHorizontal(const FInputActionValue& Value);
        void Fire();

        private:
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
        UInputAction* MoveAction = nullptr;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
        UInputAction* FireAction = nullptr;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<AActor> ProjectileClass;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
        FVector MuzzleOffset = FVector(50.0f, 0.0f, 0.0f);

};
