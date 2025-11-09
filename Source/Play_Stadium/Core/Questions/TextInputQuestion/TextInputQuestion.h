#pragma once

#include "CoreMinimal.h"
#include "../QuestionBase.h"

#include "TextInputQuestionData.h"

#include "TextInputQuestion.generated.h"


UCLASS(Blueprintable, BlueprintType)
class PLAY_STADIUM_API UTextInputQuestion : public UQuestionBase
{
	GENERATED_BODY()

public:
	void InitializeQuestionData(const FTextInputQuestionData& InData);
	const FTextInputQuestionData& GetQuestionData() const;

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Text Input Question")
	FTextInputQuestionData QuestionData;
	
};
