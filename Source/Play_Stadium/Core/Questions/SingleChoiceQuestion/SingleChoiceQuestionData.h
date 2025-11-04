#pragma once

#include "CoreMinimal.h"

#include "../QuestionBaseData.h"

#include "SingleChoiceQuestionData.generated.h"

USTRUCT(BlueprintType)
struct FSingleChoiceQuestionData : public FQuestionBaseData
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, Category = "Single Choice Question")
	TMap<FString, bool> Choices;

};