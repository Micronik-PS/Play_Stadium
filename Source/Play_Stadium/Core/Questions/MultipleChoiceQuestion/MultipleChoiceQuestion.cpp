#include "MultipleChoiceQuestion.h"

void UMultipleChoiceQuestion::InitializeQuestionData(const FMultipleChoiceQuestionData& InData)
{
	QuestionData = InData;
}

const FMultipleChoiceQuestionData& UMultipleChoiceQuestion::GetQuestionData() const
{
	return QuestionData;
}

QuestionType UMultipleChoiceQuestion::GetType() const
{
	return QuestionData.GetType();
}
