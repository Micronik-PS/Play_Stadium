#pragma once

#include "CoreMinimal.h"
#include "../SingleChoicePlayerControllerBase.h"
#include "CosmicBlasterPlayerController.generated.h"


class UInputMappingContext;


UCLASS()
class PLAY_STADIUM_API ACosmicBlasterPlayerController : public ASingleChoicePlayerControllerBase
{
        GENERATED_BODY()

protected:

        virtual void OnPossess(APawn* InPawn) override;

private:

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
        TObjectPtr<UInputMappingContext> InputMappingContext;
	
};
