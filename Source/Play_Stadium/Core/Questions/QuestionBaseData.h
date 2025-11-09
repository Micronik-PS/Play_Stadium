#pragma once

#include "CoreMinimal.h"

#include "QuestionType.h"

#include "QuestionBaseData.generated.h"


USTRUCT(BlueprintType)
struct FQuestionBaseData
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Question Base")
	int32 Reward;

	UPROPERTY(BlueprintReadWrite, Category = "Question Base")
	QuestionType Type;

	UPROPERTY(BlueprintReadWrite, Category = "Question Base")
	int32 TimeLimitInSeconds;

};