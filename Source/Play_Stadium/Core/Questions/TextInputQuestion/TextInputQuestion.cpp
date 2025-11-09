#include "TextInputQuestion.h"

void UTextInputQuestion::InitializeQuestionData(const FTextInputQuestionData& InData)
{
	QuestionData = InData;
}

const FTextInputQuestionData& UTextInputQuestion::GetQuestionData() const
{
	return QuestionData;
}
