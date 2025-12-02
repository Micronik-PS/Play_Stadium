#pragma once

#include "CoreMinimal.h"
#include "Play_Stadium/Maps/PlayStadiumLevelBase.h"
#include "Play_Stadium/Targets/CosmicBubble/CosmicBubble.h"
#include "Play_Stadium/Vicinity/SingleChoiceVicinity/CosmicBlaster/CosmicBlasterQuestionBanner/CosmicBlasterQuestionBanner.h"
#include "Play_Stadium/Core/Questions/SingleChoiceQuestion/SingleChoiceQuestionData.h"

#include "CosmicBlasterLevel.generated.h"

struct FBubbleSpawnData
{
	TSubclassOf<ACosmicBubble> BubbleClass = nullptr;
	FTransform Transform = FTransform::Identity;
};

UCLASS()
class PLAY_STADIUM_API ACosmicBlasterLevel : public APlayStadiumLevelBase
{
	GENERATED_BODY()

public:
	ACosmicBlasterLevel();

	UFUNCTION(BlueprintCallable, Category = "CosmicBlaster")
	void ApplyQuestionData(const FSingleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex = 1, int32 TotalQuestions = 1);

	TArray<FCosmicBubbleDestroyedSignature*> GetBubbleDestroyedDelegates() const;

	UFUNCTION(BlueprintCallable, Category = "CosmicBlaster")
	void RespawnBubbles();

protected:
	virtual void BeginPlay() override;

private:
	void CacheBubbleSpawnDataIfNeeded();
	void AssignChoicesToBubbles(const FSingleChoiceQuestionData& QuestionData);
	void UpdateQuestionBanner(const FSingleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions);
	void ConfigureBubbleSpeeds(const FSingleChoiceQuestionData& QuestionData);
	float CalculateBaseSpeedFromTimeLimit(ACosmicBubble* Bubble, int32 TimeLimitInSeconds) const;
	float FindDistanceToSurfaceBelow(const ACosmicBubble* Bubble) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicBlaster", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<ACosmicBubble>> Bubbles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicBlaster", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACosmicBlasterQuestionBanner> QuestionBanner = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicBlaster|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float SpeedDeviationRange = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicBlaster|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float MaxTraceDistance = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicBlaster|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float FallbackDistance = 500.0f;

	TArray<FBubbleSpawnData> InitialBubbleSpawnData;
};
