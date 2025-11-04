#pragma once

#include "CoreMinimal.h"
#include "QuestionBaseData.generated.h"

USTRUCT(BlueprintType)
struct FQuestionBaseData
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, Category = "Question Base")
	int Reward;

};