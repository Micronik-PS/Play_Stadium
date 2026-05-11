#pragma once

#include "CoreMinimal.h"
#include "../TextInputGameModeBase.h"
#include "Play_Stadium/Core/Questions/TextInputQuestion/TextInputQuestionData.h"
#include "CosmicStarsGameMode.generated.h"

class ACosmicSector;
class ACosmicStarsLevel;
class UPlayStadiumGameInstance;

UCLASS()
class PLAY_STADIUM_API ACosmicStarsGameMode : public ATextInputGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "CosmicStars")
	void HandleSectorSelected(ACosmicSector* SelectedSector);

private:
	void InitializeQuestionForLevel();
	bool TryGetCurrentTextInputQuestion(FTextInputQuestionData& OutQuestionData) const;
	bool TryConsumeNextTextInputQuestion(FTextInputQuestionData& OutQuestionData);
	void ApplyQuestionToLevel(const FTextInputQuestionData& QuestionData, int32 CurrentQuestionIndex);
	void AdvanceToNextQuestion();
	UPlayStadiumGameInstance* GetPlayStadiumGameInstance() const;
	ACosmicStarsLevel* GetCosmicStarsLevel() const;
	void BindLevelDelegates(ACosmicStarsLevel* Level);
	void UnbindLevelDelegates();

	UFUNCTION()
	void HandleQuestionCompleted();

	UFUNCTION()
	void HandleQuestionFailed();

private:
	TWeakObjectPtr<ACosmicStarsLevel> CachedLevel;
	FTextInputQuestionData CurrentQuestionData;
	bool bHasActiveQuestion = false;
	bool bIsProcessingResult = false;
	
};
