#include "MultipleChoiceQuestion.h"

void UMultipleChoiceQuestion::InitializeQuestionData(const FMultipleChoiceQuestionData& InData)
{
	QuestionData = InData;
}

const FMultipleChoiceQuestionData& UMultipleChoiceQuestion::GetQuestionData() const
{
	return QuestionData;
}
