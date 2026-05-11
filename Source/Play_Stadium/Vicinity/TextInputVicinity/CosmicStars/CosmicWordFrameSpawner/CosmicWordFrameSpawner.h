#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmicWordFrameSpawner.generated.h"

class ACosmicWordFrame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCosmicWordFrameSpawnerDestinationSignature, ACosmicWordFrame*, WordFrame);

UCLASS()
class PLAY_STADIUM_API ACosmicWordFrameSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ACosmicWordFrameSpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "CosmicWordFrameSpawner")
	void SpawnWordFrames(const FString& Word, int32 TimeLimitInSeconds);

	UFUNCTION(BlueprintCallable, Category = "CosmicWordFrameSpawner")
	void ClearSpawnedFrames();

	UFUNCTION(BlueprintCallable, Category = "CosmicWordFrameSpawner")
	int32 RevealLetter(const FString& Letter);

	UFUNCTION(BlueprintPure, Category = "CosmicWordFrameSpawner")
	bool IsWordCompleted() const;

	UFUNCTION(BlueprintPure, Category = "CosmicWordFrameSpawner")
	TArray<FString> GetMissingLetters() const;

	UFUNCTION(BlueprintPure, Category = "CosmicWordFrameSpawner")
	TArray<ACosmicWordFrame*> GetSpawnedFrames() const;

	UPROPERTY(BlueprintAssignable, Category = "CosmicWordFrameSpawner|Events")
	FCosmicWordFrameSpawnerDestinationSignature OnWordFrameReachedDestination;

private:
	TArray<FString> ExtractLetters(const FString& Word) const;
	float CalculateMovementSpeed(ACosmicWordFrame* ReferenceFrame, int32 TimeLimitInSeconds) const;
	float FindDistanceToSurfaceBelow(const ACosmicWordFrame* ReferenceFrame) const;

	UFUNCTION()
	void HandleWordFrameReachedDestination(ACosmicWordFrame* WordFrame);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrameSpawner", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACosmicWordFrame> WordFrameClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrameSpawner|Layout", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float FrameSpacing = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrameSpawner|Layout", meta = (AllowPrivateAccess = "true"))
	bool bCenterRowOnSpawner = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrameSpawner|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float MaxTraceDistance = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrameSpawner|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float FallbackTravelDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrameSpawner|Movement", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_WorldStatic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicWordFrameSpawner", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<ACosmicWordFrame>> SpawnedFrames;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicWordFrameSpawner", meta = (AllowPrivateAccess = "true"))
	FString CurrentWord;
};
