#pragma once

#include "CoreMinimal.h"
#include "../SingleChoiceGameModeBase.h"
#include "Play_Stadium/Core/Questions/SingleChoiceQuestion/SingleChoiceQuestionData.h"
#include "Play_Stadium/Targets/TargetDestroyReason.h"
#include "CosmicBlasterGameMode.generated.h"

class ACosmicBlasterLevel;
class UPlayStadiumGameInstance;
class FCosmicBubbleDestroyedSignature;

UCLASS()
class PLAY_STADIUM_API ACosmicBlasterGameMode : public ASingleChoiceGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	void InitializeQuestionForLevel();
	bool TryGetCurrentSingleChoiceQuestion(FSingleChoiceQuestionData& OutQuestionData) const;
	bool TryConsumeNextSingleChoiceQuestion(FSingleChoiceQuestionData& OutQuestionData);
	void ApplyQuestionToLevel(const FSingleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex);
	UPlayStadiumGameInstance* GetPlayStadiumGameInstance() const;
	ACosmicBlasterLevel* GetCosmicBlasterLevel() const;
	void AdvanceToNextQuestion(ETargetDestroyReason DestroyReason);
	void BindBubbleDelegates(const TArray<FCosmicBubbleDestroyedSignature*>& Delegates);
	void UnbindBubbleDelegates();

	UFUNCTION()
	void HandleBubbleDestroyed(ETargetDestroyReason DestroyReason, bool bWasCorrectChoice);

	TArray<FCosmicBubbleDestroyedSignature*> CachedBubbleDelegates;
	FSingleChoiceQuestionData CurrentQuestionData;
	bool bHasActiveQuestion = false;
	bool bHasReceivedAnswer = false;
};
