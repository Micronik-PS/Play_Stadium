#pragma once

#include "CoreMinimal.h"
#include "../MultipleChoicePlayerControllerBase.h"
#include "CosmicMeteorsPlayerController.generated.h"


class UInputMappingContext;

UCLASS()
class PLAY_STADIUM_API ACosmicMeteorsPlayerController : public AMultipleChoicePlayerControllerBase
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> InputMappingContext;
};
