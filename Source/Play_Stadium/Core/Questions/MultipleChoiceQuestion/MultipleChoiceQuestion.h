#pragma once

#include "CoreMinimal.h"
#include "../QuestionBase.h"

#include "MultipleChoiceQuestionData.h"

#include "MultipleChoiceQuestion.generated.h"


UCLASS(Blueprintable, BlueprintType)
class PLAY_STADIUM_API UMultipleChoiceQuestion : public UQuestionBase
{
	GENERATED_BODY()

public:
	void InitializeQuestionData(const FMultipleChoiceQuestionData& InData);
	const FMultipleChoiceQuestionData& GetQuestionData() const;
	virtual QuestionType GetType() const override;

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Multiple Choice Question")
	FMultipleChoiceQuestionData QuestionData;
	
};
