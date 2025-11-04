#pragma once

#include "CoreMinimal.h"

#include "../QuestionBaseData.h"

#include "MultipleChoiceQuestionData.generated.h"

USTRUCT(BlueprintType)
struct FMultipleChoiceQuestionData : public FQuestionBaseData
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, Category = "Multiple Choice Question")
	TMap<FString, bool> Choices;

};