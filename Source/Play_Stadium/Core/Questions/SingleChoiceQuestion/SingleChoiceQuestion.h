#pragma once

#include "CoreMinimal.h"
#include "../QuestionBase.h"

#include "SingleChoiceQuestionData.h"

#include "SingleChoiceQuestion.generated.h"


UCLASS(Blueprintable, BlueprintType)
class PLAY_STADIUM_API USingleChoiceQuestion : public UQuestionBase
{
	GENERATED_BODY()


protected:

	UPROPERTY(BlueprintReadWrite, Category = "Single Choice Question")
	FSingleChoiceQuestionData QuestionData;
	
};
