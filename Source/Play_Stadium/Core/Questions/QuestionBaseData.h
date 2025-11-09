#pragma once

#include "CoreMinimal.h"

#include "QuestionType.h"

#include "QuestionBaseData.generated.h"


USTRUCT(BlueprintType)
struct FQuestionBaseData
{
	GENERATED_BODY()

public:
	FQuestionBaseData();

	void SetReward(int32 InReward);
	void SetType(QuestionType InType);
	void SetTimeLimitInSeconds(int32 InTimeLimit);

	int32 GetReward() const;
	QuestionType GetType() const;
	int32 GetTimeLimitInSeconds() const;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Question Base")
	int32 Reward;

	UPROPERTY(BlueprintReadWrite, Category = "Question Base")
	QuestionType Type;

	UPROPERTY(BlueprintReadWrite, Category = "Question Base")
	int32 TimeLimitInSeconds;
};
