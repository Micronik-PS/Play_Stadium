#pragma once

#include "CoreMinimal.h"
#include "Play_Stadium/Maps/PlayStadiumLevelBase.h"
#include "Play_Stadium/Core/Questions/MatchingQuestion/MatchingQuestionData.h"

#include "LaserConnectionLevel.generated.h"

class ACameraActor;
class ACosmicBlasterQuestionBanner;
class APowerCable;
class APowerSocket;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLaserConnectionQuestionAnsweredSignature, bool, bAllConnectionsCorrect);

UCLASS()
class PLAY_STADIUM_API ALaserConnectionLevel : public APlayStadiumLevelBase
{
	GENERATED_BODY()

public:
	ALaserConnectionLevel();

	UFUNCTION(BlueprintCallable, Category = "LaserConnection")
	void ApplyQuestionData(const FMatchingQuestionData& QuestionData, int32 CurrentQuestionIndex = 1, int32 TotalQuestions = 1);

	UFUNCTION(BlueprintPure, Category = "LaserConnection")
	TArray<APowerCable*> GetPowerCables() const;

	UFUNCTION(BlueprintPure, Category = "LaserConnection")
	TArray<APowerSocket*> GetPowerSockets() const;

	UFUNCTION(BlueprintCallable, Category = "LaserConnection")
	APowerSocket* FindClosestAvailableSocket(const FVector& WorldLocation, float MaxDistance = -1.0f) const;

	UFUNCTION(BlueprintCallable, Category = "LaserConnection")
	bool TryConnectCableToSocket(APowerCable* Cable, APowerSocket* Socket);

	UFUNCTION(BlueprintPure, Category = "LaserConnection")
	bool AreAllCablesConnected() const;

	UFUNCTION(BlueprintPure, Category = "LaserConnection")
	bool AreAllConnectionsCorrect() const;

	UPROPERTY(BlueprintAssignable, Category = "LaserConnection|Events")
	FLaserConnectionQuestionAnsweredSignature OnQuestionAnswered;

protected:
	virtual void BeginPlay() override;

private:
	void ResolveActorReferences();
	void ApplyStartupCamera() const;
	void AssignPairsToActors(const FMatchingQuestionData& QuestionData);
	void ResetCableConnections();
	void UpdateQuestionBanner(const FMatchingQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions);
	void CheckForCompletedConnections();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaserConnection|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACameraActor> LaserConnectionCameraActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaserConnection|Actors", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<APowerCable>> PowerCables;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaserConnection|Actors", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<APowerSocket>> PowerSockets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaserConnection|Actors", meta = (AllowPrivateAccess = "true"))
	bool bAutoFindActorsOnLevel = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaserConnection|UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACosmicBlasterQuestionBanner> QuestionBanner = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaserConnection|Connection", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float DefaultConnectionRadius = 110.0f;

	UPROPERTY()
	bool bHasActiveQuestion = false;

	UPROPERTY()
	bool bQuestionAnswered = false;
};
