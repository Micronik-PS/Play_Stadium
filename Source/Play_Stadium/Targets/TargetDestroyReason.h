#pragma once

#include "CoreMinimal.h"
#include "TargetDestroyReason.generated.h"

UENUM(BlueprintType)
enum class ETargetDestroyReason : uint8
{
	FromDestinationPoint UMETA(DisplayName = "FromDestinationPoint"),
	FromPlayerAttack     UMETA(DisplayName = "FromPlayerAttack")
};
