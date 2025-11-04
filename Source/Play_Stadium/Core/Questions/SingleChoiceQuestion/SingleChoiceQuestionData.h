#pragma once

#include "CoreMinimal.h"
#include "SingleChoiceQuestionData.generated.h"

USTRUCT(BlueprintType)
struct FSingleChoiceQuestionData
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, Category = "Single Choice Question")
	TMap<FString, bool> Choices;

};