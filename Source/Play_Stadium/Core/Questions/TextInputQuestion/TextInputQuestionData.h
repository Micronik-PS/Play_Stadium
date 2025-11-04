#pragma once

#include "CoreMinimal.h"

#include "../QuestionBaseData.h"

#include "TextInputQuestionData.generated.h"

USTRUCT(BlueprintType)
struct FTextInputQuestionData : public FQuestionBaseData
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, Category = "Text Input Question")
	FString CorrectAnswer;

};