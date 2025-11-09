#include "MatchingQuestion.h"

void UMatchingQuestion::InitializeQuestionData(const FMatchingQuestionData& InData)
{
	QuestionData = InData;
}

const FMatchingQuestionData& UMatchingQuestion::GetQuestionData() const
{
	return QuestionData;
}

QuestionType UMatchingQuestion::GetType() const
{
	return QuestionData.GetType();
}