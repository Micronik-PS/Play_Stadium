#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "../MatchingQuestionPlayerControllerBase.h"
#include "LaserConnectionPlayerController.generated.h"

class APowerCable;
class UInputMappingContext;

UCLASS()
class PLAY_STADIUM_API ALaserConnectionPlayerController : public AMatchingQuestionPlayerControllerBase
{
	GENERATED_BODY()

public:
	ALaserConnectionPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	void ApplyMouseInputSettings();
	void HandleLeftMousePressed();
	void HandleLeftMouseReleased();
	void HandleTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);
	void HandleTouchMoved(ETouchIndex::Type FingerIndex, FVector Location);
	void HandleTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);
	void BeginPointerDrag();
	void EndPointerDrag();
	void UpdateActiveTouchLocation(ETouchIndex::Type FingerIndex, const FVector& Location);
	void UpdateDraggedCable();
	void TryAttachDraggedCable();
	APowerCable* GetCableUnderPointer() const;
	bool GetPointerScreenPosition(FVector2D& OutScreenPosition) const;
	bool GetPointerWorldPointOnDragPlane(FVector& OutWorldPoint) const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaserConnection|Drag", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float AttachSearchRadius = 140.0f;

	UPROPERTY()
	TWeakObjectPtr<APowerCable> DraggedCable;

	UPROPERTY()
	FVector DragGrabOffset = FVector::ZeroVector;

	UPROPERTY()
	float DragPlaneY = 0.0f;

	bool bUsingTouchPointer = false;

	ETouchIndex::Type ActiveTouchIndex = ETouchIndex::MAX_TOUCHES;

	FVector2D ActiveTouchScreenPosition = FVector2D::ZeroVector;
};
