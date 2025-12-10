#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimInstance.h"
#include "ZDA_CosmicButton.generated.h"

UCLASS()
class PLAY_STADIUM_API UZDA_CosmicButton : public UPaperZDAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "CosmicButton")
	void HandleButtonPressed();
};

