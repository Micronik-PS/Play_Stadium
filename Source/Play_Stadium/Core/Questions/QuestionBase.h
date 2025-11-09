#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "QuestionType.h"

#include "QuestionBase.generated.h"


UCLASS(Abstract)
class PLAY_STADIUM_API UQuestionBase : public UObject
{
	GENERATED_BODY()

public:
	virtual QuestionType GetType() const PURE_VIRTUAL(UQuestionBase::GetType, return QuestionType::ErrorQuestion;);
};
