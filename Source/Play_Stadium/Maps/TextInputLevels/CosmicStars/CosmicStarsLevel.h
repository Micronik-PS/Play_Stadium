#pragma once

#include "CoreMinimal.h"
#include "Play_Stadium/Maps/PlayStadiumLevelBase.h"
#include "Play_Stadium/Core/Questions/TextInputQuestion/TextInputQuestionData.h"
#include "CosmicStarsLevel.generated.h"

class ACameraActor;
class ACosmicBlasterQuestionBanner;
class ACosmicSectorSpawner;
class ACosmicWordFrame;
class ACosmicWordFrameSpawner;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCosmicStarsQuestionResolvedSignature);

UCLASS()
class PLAY_STADIUM_API ACosmicStarsLevel : public APlayStadiumLevelBase
{
	GENERATED_BODY()

public:
	ACosmicStarsLevel();

	UFUNCTION(BlueprintCallable, Category = "CosmicStars")
	void ApplyQuestionData(const FTextInputQuestionData& QuestionData, int32 CurrentQuestionIndex = 1, int32 TotalQuestions = 1);

	UFUNCTION(BlueprintCallable, Category = "CosmicStars")
	bool HandleSelectedLetter(const FString& Letter);

	UFUNCTION(BlueprintCallable, Category = "CosmicStars")
	void StopQuestionActors();

	UFUNCTION(BlueprintPure, Category = "CosmicStars")
	bool IsWordCompleted() const;

	UFUNCTION(BlueprintPure, Category = "CosmicStars")
	TArray<ACosmicWordFrame*> GetWordFrames() const;

	UPROPERTY(BlueprintAssignable, Category = "CosmicStars|Events")
	FCosmicStarsQuestionResolvedSignature OnQuestionCompleted;

	UPROPERTY(BlueprintAssignable, Category = "CosmicStars|Events")
	FCosmicStarsQuestionResolvedSignature OnQuestionFailed;

protected:
	virtual void BeginPlay() override;

private:
	void ResolveSpawnerReferences();
	void ApplyStartupCamera() const;
	void ConfigureSectorSpawners(const FString& CorrectAnswer);
	void UpdateQuestionBanner(const FTextInputQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions);
	void RefreshSectorSpawnerMissingLetters();
	void SetSectorSpawnersEnabled(bool bEnabled);
	void ClearSectorSpawners();
	void ResolveQuestion(bool bCompleted);

	UFUNCTION()
	void HandleWordFrameReachedDestination(ACosmicWordFrame* WordFrame);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicStars|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACameraActor> CosmicStarsCameraActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicStars|Spawners", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACosmicWordFrameSpawner> WordFrameSpawner = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicStars|Spawners", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<ACosmicSectorSpawner>> SectorSpawners;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicStars|Spawners", meta = (AllowPrivateAccess = "true"))
	bool bAutoFindSpawnersOnLevel = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicStars|UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACosmicBlasterQuestionBanner> QuestionBanner = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicStars|Question", meta = (AllowPrivateAccess = "true"))
	FString CurrentCorrectAnswer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicStars|Question", meta = (AllowPrivateAccess = "true"))
	int32 CurrentQuestionNumber = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicStars|Question", meta = (AllowPrivateAccess = "true"))
	int32 TotalQuestionCount = 1;

	UPROPERTY()
	bool bHasActiveQuestion = false;

	UPROPERTY()
	bool bQuestionResolved = false;
	
};
