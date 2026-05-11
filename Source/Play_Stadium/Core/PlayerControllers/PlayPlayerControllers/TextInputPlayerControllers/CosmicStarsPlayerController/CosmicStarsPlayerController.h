#pragma once

#include "CoreMinimal.h"
#include "../TextInputPlayerControllerBase.h"
#include "CosmicStarsPlayerController.generated.h"

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

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
};
