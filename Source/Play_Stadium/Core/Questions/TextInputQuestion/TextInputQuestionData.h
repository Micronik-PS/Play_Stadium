#pragma once

#include "CoreMinimal.h"
#include "TextInputQuestionData.generated.h"

USTRUCT(BlueprintType)
struct FTextInputQuestionData
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, Category = "Text Input Question")
	FString CorrectAnswer;

};