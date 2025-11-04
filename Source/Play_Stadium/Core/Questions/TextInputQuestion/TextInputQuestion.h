#pragma once

#include "CoreMinimal.h"
#include "../QuestionBase.h"

#include "TextInputQuestionData.h"

#include "TextInputQuestion.generated.h"


UCLASS(Blueprintable, BlueprintType)
class PLAY_STADIUM_API UTextInputQuestion : public UQuestionBase
{
	GENERATED_BODY()


protected:

	UPROPERTY(BlueprintReadWrite, Category = "Text Input Question")
	FTextInputQuestionData QuestionData;
	
};
