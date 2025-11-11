#pragma once

#include "CoreMinimal.h"
#include "QuestionType.generated.h"


UENUM(BlueprintType)
enum class EQuestionType : uint8
{
	ErrorQuestion		    UMETA(DisplayName = "Error"),
    SingleChoiceQuestion    UMETA(DisplayName = "SingleChoiceQuestion"),
    MultipleChoiceQuestion  UMETA(DisplayName = "MultipleChoiceQuestion"),
    TextInputQuestion       UMETA(DisplayName = "TextInputQuestion"),
    MatchingQuestion        UMETA(DisplayName = "MatchingQuestion")
};