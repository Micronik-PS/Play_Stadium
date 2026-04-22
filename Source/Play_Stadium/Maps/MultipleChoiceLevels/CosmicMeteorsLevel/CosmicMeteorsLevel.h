#pragma once

#include "CoreMinimal.h"
#include "Play_Stadium/Maps/PlayStadiumLevelBase.h"
#include "Play_Stadium/Core/Questions/MultipleChoiceQuestion/MultipleChoiceQuestionData.h"
#include "Play_Stadium/Targets/Meteor/Meteor.h"
#include "Play_Stadium/Vicinity/SingleChoiceVicinity/CosmicBlaster/CosmicBlasterQuestionBanner/CosmicBlasterQuestionBanner.h"

#include "CosmicMeteorsLevel.generated.h"

struct FMeteorSpawnData
{
	TSubclassOf<AMeteor> MeteorClass = nullptr;
	FTransform Transform = FTransform::Identity;
	FTransform WidgetTransform = FTransform::Identity;
};

UCLASS()
class PLAY_STADIUM_API ACosmicMeteorsLevel : public APlayStadiumLevelBase
{
	GENERATED_BODY()

public:
	ACosmicMeteorsLevel();

	UFUNCTION(BlueprintCallable, Category = "CosmicMeteors")
	void ApplyQuestionData(const FMultipleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex = 1, int32 TotalQuestions = 1);

	TArray<AMeteor*> GetMeteors() const;

	UFUNCTION(BlueprintCallable, Category = "CosmicMeteors")
	void RespawnMeteors();

protected:
	virtual void BeginPlay() override;

private:
	void CacheMeteorSpawnDataIfNeeded();
	void AssignChoicesToMeteors(const FMultipleChoiceQuestionData& QuestionData);
	void UpdateQuestionBanner(const FMultipleChoiceQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions);
	void ConfigureMeteorSpeeds(const FMultipleChoiceQuestionData& QuestionData);
	float CalculateBaseSpeedFromTimeLimit(AMeteor* Meteor, int32 TimeLimitInSeconds) const;
	float FindDistanceAhead(const AMeteor* Meteor) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicMeteors", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<AMeteor>> Meteors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicMeteors", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACosmicBlasterQuestionBanner> QuestionBanner = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicMeteors|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float SpeedDeviationRange = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicMeteors|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float MaxTraceDistance = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicMeteors|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float FallbackDistance = 500.0f;

	TArray<FMeteorSpawnData> InitialMeteorSpawnData;
};
