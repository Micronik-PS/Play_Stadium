#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "../TextInputPlayerControllerBase.h"
#include "CosmicStarsPlayerController.generated.h"

class ACosmicSector;
class UInputMappingContext;

UCLASS()
class PLAY_STADIUM_API ACosmicStarsPlayerController : public ATextInputPlayerControllerBase
{
	GENERATED_BODY()

public:
	ACosmicStarsPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	void ApplyMouseInputSettings();
	void HandleLeftMousePressed();
	void HandleTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);
	void HandlePointerPressedAtScreenPosition(const FVector2D& ScreenPosition);
	ACosmicSector* GetSectorAtScreenPosition(const FVector2D& ScreenPosition) const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
};
