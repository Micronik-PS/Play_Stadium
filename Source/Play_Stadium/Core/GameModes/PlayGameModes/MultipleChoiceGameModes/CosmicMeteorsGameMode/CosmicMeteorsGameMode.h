#pragma once

#include "CoreMinimal.h"
#include "../MultipleChoiceGameModeBase.h"
#include "Play_Stadium/Core/Questions/MultipleChoiceQuestion/MultipleChoiceQuestionData.h"
#include "Play_Stadium/Targets/TargetDestroyReason.h"
#include "CosmicMeteorsGameMode.generated.h"

class ACosmicMeteorsLevel;
class AMeteor;
class UPlayStadiumGameInstance;


UCLASS()
class PLAY_STADIUM_API ACosmicMeteorsGameMode : public AMultipleChoiceGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	void InitializeQuestionForLevel();
	bool TryGetCurrentMultipleChoiceQuestion(FMultipleChoiceQuestionData& OutQuestionData) const;
	bool TryConsumeNextMultipleChoiceQuestion(FMultipleChoiceQuestionData& OutQuestionData);
	void ApplyQuestionToLevel(const FMultipleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex);
	UPlayStadiumGameInstance* GetPlayStadiumGameInstance() const;
	ACosmicMeteorsLevel* GetCosmicMeteorsLevel() const;
	void AdvanceAfterScoring(ETargetDestroyReason DestroyReason);
	void BindMeteorDelegates(const TArray<AMeteor*>& Meteors);
	void UnbindMeteorDelegates();
	void ResetProgressCounters(int32 MeteorCount);
	void CalculateAndApplyScore(ETargetDestroyReason DestroyReason);

	UFUNCTION()
	void HandleMeteorDestroyed(ETargetDestroyReason DestroyReason, bool bWasCorrectChoice);

private:
	TArray<TWeakObjectPtr<AMeteor>> CachedMeteors;
	FMultipleChoiceQuestionData CurrentQuestionData;
	int32 TotalMeteorCount = 0;
	int32 DestroyedMeteorCount = 0;
	int32 CorrectDestroyedByPlayerCount = 0;
	bool bDestroyedIncorrectMeteor = false;
	bool bHasActiveQuestion = false;
	bool bIsProcessingResult = false;
};
