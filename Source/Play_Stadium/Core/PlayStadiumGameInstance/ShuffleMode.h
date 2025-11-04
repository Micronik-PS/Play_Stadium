#pragma once

UENUM(BlueprintType)
enum class ShuffleMode : uint8
{
    ShuffleNone         UMETA(DisplayName = "ShuffleNone"),
    ShuffleRandom       UMETA(DisplayName = "Bow"),
    ShuffleTypeSort     UMETA(DisplayName = "Staff")
};