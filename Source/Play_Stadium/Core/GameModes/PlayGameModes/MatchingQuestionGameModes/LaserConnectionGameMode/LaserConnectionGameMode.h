#pragma once

#include "CoreMinimal.h"
#include "../MatchingQuestionGameModeBase.h"
#include "Play_Stadium/Core/Questions/MatchingQuestion/MatchingQuestionData.h"
#include "LaserConnectionGameMode.generated.h"

class ALaserConnectionLevel;
class UPlayStadiumGameInstance;

UCLASS()
class PLAY_STADIUM_API ALaserConnectionGameMode : public AMatchingQuestionGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	void InitializeQuestionForLevel();
	bool TryGetCurrentMatchingQuestion(FMatchingQuestionData& OutQuestionData) const;
	bool TryConsumeNextMatchingQuestion(FMatchingQuestionData& OutQuestionData);
	void ApplyQuestionToLevel(const FMatchingQuestionData& QuestionData, int32 CurrentQuestionIndex);
	void AdvanceToNextQuestion();
	UPlayStadiumGameInstance* GetPlayStadiumGameInstance() const;
	ALaserConnectionLevel* GetLaserConnectionLevel() const;
	void BindLevelDelegates(ALaserConnectionLevel* Level);
	void UnbindLevelDelegates();

	UFUNCTION()
	void HandleQuestionAnswered(bool bAllConnectionsCorrect);

private:
	TWeakObjectPtr<ALaserConnectionLevel> CachedLevel;
	FMatchingQuestionData CurrentQuestionData;
	bool bHasActiveQuestion = false;
	bool bIsProcessingResult = false;
};
