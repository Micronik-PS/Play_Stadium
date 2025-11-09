#pragma once

#include "CoreMinimal.h"
#include "../QuestionBase.h"

#include "MatchingQuestionData.h"

#include "MatchingQuestion.generated.h"


UCLASS(Blueprintable, BlueprintType)
class PLAY_STADIUM_API UMatchingQuestion : public UQuestionBase
{
	GENERATED_BODY()

public:
	void InitializeQuestionData(const FMatchingQuestionData& InData);
	const FMatchingQuestionData& GetQuestionData() const;

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Matching Question")
	FMatchingQuestionData QuestionData;

};
