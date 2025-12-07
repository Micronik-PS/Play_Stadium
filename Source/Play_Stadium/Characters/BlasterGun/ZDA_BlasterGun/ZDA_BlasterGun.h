#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimInstance.h"
#include "ZDA_BlasterGun.generated.h"


UCLASS()
class PLAY_STADIUM_API UZDA_BlasterGun : public UPaperZDAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void HandleBlasterGunFired();
};
