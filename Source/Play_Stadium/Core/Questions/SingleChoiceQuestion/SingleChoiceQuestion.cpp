#include "SingleChoiceQuestion.h"

void USingleChoiceQuestion::InitializeQuestionData(const FSingleChoiceQuestionData& InData)
{
	QuestionData = InData;
}

const FSingleChoiceQuestionData& USingleChoiceQuestion::GetQuestionData() const
{
	return QuestionData;
}

QuestionType USingleChoiceQuestion::GetType() const
{
	return QuestionData.GetType();
}
