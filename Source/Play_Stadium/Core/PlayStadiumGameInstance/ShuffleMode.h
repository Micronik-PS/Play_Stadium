#pragma once

UENUM(BlueprintType)
enum class ShuffleMode : uint8
{
    ShuffleNone         UMETA(DisplayName = "ShuffleNone"),
    ShuffleRandom       UMETA(DisplayName = "ShuffleRandom"),
    ShuffleTypeSort     UMETA(DisplayName = "ShuffleTypeSort")
};