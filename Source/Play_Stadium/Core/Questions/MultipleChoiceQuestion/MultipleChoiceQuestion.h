#pragma once

#include "CoreMinimal.h"
#include "../QuestionBase.h"

#include "MultipleChoiceQuestionData.h"

#include "MultipleChoiceQuestion.generated.h"


UCLASS(Blueprintable, BlueprintType)
class PLAY_STADIUM_API UMultipleChoiceQuestion : public UQuestionBase
{
	GENERATED_BODY()


protected:

	UPROPERTY(BlueprintReadWrite, Category = "Multiple Choice Question")
	FMultipleChoiceQuestionData QuestionData;
	
};
