#pragma once

#include "CoreMinimal.h"
#include "../QuestionBase.h"

#include "MatchingQuestionData.h"

#include "MatchingQuestion.generated.h"


UCLASS(Blueprintable, BlueprintType)
class PLAY_STADIUM_API UMatchingQuestion : public UQuestionBase
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Matching Question")
	FMatchingQuestionData QuestionData;

};
