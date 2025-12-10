#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimInstance.h"
#include "ZDA_FighterJet.generated.h"

UCLASS()
class PLAY_STADIUM_API UZDA_FighterJet : public UPaperZDAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void HandleFighterJetFired();
};

