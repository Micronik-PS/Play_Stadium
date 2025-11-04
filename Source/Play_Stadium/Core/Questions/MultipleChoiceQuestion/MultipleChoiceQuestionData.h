#pragma once

#include "CoreMinimal.h"
#include "MultipleChoiceQuestionData.generated.h"

USTRUCT(BlueprintType)
struct FMultipleChoiceQuestionData
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, Category = "Multiple Choice Question")
	TMap<FString, bool> Choices;

};