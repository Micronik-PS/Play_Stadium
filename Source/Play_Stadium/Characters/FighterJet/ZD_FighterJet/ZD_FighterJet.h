#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
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
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Fire();
	void Rotate(const FInputActionValue& Value);
	void RotateByDirection(float InputValue);
	void HandleTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);
	void HandleTouchMoved(ETouchIndex::Type FingerIndex, FVector Location);
	void HandleTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);
	void StartTouchRotation(ETouchIndex::Type FingerIndex, const FVector& Location);
	void UpdateTouchRotation(const FVector& Location);
	void ResetFireCooldown();
	void TriggerAnimFireEvent();
	bool CanFire() const;
	bool IsTouchInFireArea(const FVector& Location) const;
	bool GetViewportSize(FVector2D& OutViewportSize) const;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mobile Input", meta = (AllowPrivateAccess = "true", ClampMin = "0.1", ClampMax = "0.9"))
	float FireAreaScreenHeightRatio = 0.45f;

	bool bHasActiveTouchRotation = false;
	ETouchIndex::Type ActiveRotationTouchIndex = ETouchIndex::MAX_TOUCHES;
	float ActiveTouchRotateDirection = 0.0f;
};
