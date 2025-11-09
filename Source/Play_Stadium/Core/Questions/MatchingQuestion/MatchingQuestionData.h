#pragma once

#include "CoreMinimal.h"

#include "../QuestionBaseData.h"

#include "MatchingQuestionData.generated.h"


USTRUCT(BlueprintType)
struct FMatchingQuestionData : public FQuestionBaseData
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, Category = "Matching Question")
	TMap<FString, FString> Pairs;

};