#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "PaperZDCharacter.h"
#include "ZD_BlasterGun.generated.h"

class UInputAction;
class AActor;


UCLASS()
class PLAY_STADIUM_API AZD_BlasterGun : public APaperZDCharacter
{
        GENERATED_BODY()

        public:
        virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

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
