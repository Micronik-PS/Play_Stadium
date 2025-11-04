#pragma once

#include "CoreMinimal.h"
#include "MatchingQuestionData.generated.h"

USTRUCT(BlueprintType)
struct FMatchingQuestionData
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, Category = "Matching Question")
	TMap<FString, FString> Pairs;

};