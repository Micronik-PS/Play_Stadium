#include "QuestionBaseData.h"


FQuestionBaseData::FQuestionBaseData()
	: Reward(0)
	, Type(QuestionType::SingleChoiceQuestion)
	, TimeLimitInSeconds(0)
{
}

void FQuestionBaseData::SetReward(int32 InReward)
{
	Reward = InReward;
}

void FQuestionBaseData::SetType(QuestionType InType)
{
	Type = InType;
}

void FQuestionBaseData::SetTimeLimitInSeconds(int32 InTimeLimit)
{
	TimeLimitInSeconds = InTimeLimit;
}

int32 FQuestionBaseData::GetReward() const
{
	return Reward;
}

QuestionType FQuestionBaseData::GetType() const
{
	return Type;
}

int32 FQuestionBaseData::GetTimeLimitInSeconds() const
{
	return TimeLimitInSeconds;
}
