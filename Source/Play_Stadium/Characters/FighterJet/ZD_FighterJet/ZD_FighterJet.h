#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "PaperZDCharacter.h"
#include "ZD_FighterJet.generated.h"

class UInputAction;
class AActor;

UCLASS()
class PLAY_STADIUM_API AZD_FighterJet : public APaperZDCharacter
{
	GENERATED_BODY()

public:
	AZD_FighterJet();

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Fire();
	void Rotate(const FInputActionValue& Value);
	void ResetFireCooldown();
	void TriggerAnimFireEvent();
	bool CanFire() const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FVector MuzzleOffset = FVector(100.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float FireCooldown = 0.3f;

	UPROPERTY()
	bool bCanFire = true;

	FTimerHandle FireCooldownTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RotationDegreesPerSecond = 45.0f;
};
