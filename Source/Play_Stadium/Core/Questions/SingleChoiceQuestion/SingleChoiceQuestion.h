#pragma once

#include "CoreMinimal.h"
#include "../QuestionBase.h"

#include "SingleChoiceQuestionData.h"

#include "SingleChoiceQuestion.generated.h"


UCLASS(Blueprintable, BlueprintType)
class PLAY_STADIUM_API USingleChoiceQuestion : public UQuestionBase
{
	GENERATED_BODY()

public:
	void InitializeQuestionData(const FSingleChoiceQuestionData& InData);
	const FSingleChoiceQuestionData& GetQuestionData() const;
	virtual QuestionType GetType() const override;

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Single Choice Question")
	FSingleChoiceQuestionData QuestionData;
	
};
