#pragma once

UENUM(BlueprintType)
enum class QuestionType : uint8
{
    SingleChoiceQuestion    UMETA(DisplayName = "SingleChoiceQuestion"),
    MultipleChoiceQuestion  UMETA(DisplayName = "MultipleChoiceQuestion"),
    TextInputQuestion       UMETA(DisplayName = "TextInputQuestion"),
    MatchingQuestion        UMETA(DisplayName = "MatchingQuestion")
};