#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "CosmicSectorSpawner.generated.h"

class ACosmicSector;
class UBoxComponent;
class USceneComponent;

UCLASS()
class PLAY_STADIUM_API ACosmicSectorSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ACosmicSectorSpawner();

	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "CosmicSectorSpawner")
	void SetSpawnEnabled(bool bInEnabled);

	UFUNCTION(BlueprintPure, Category = "CosmicSectorSpawner")
	bool IsSpawnEnabled() const { return bSpawnEnabled; }

	UFUNCTION(BlueprintCallable, Category = "CosmicSectorSpawner")
	void ConfigureQuestionLetters(const FString& CorrectAnswer);

	UFUNCTION(BlueprintCallable, Category = "CosmicSectorSpawner")
	void SetMissingLetters(const TArray<FString>& InMissingLetters);

	UFUNCTION(BlueprintCallable, Category = "CosmicSectorSpawner")
	void ClearSpawnedSectors();

	UFUNCTION(BlueprintCallable, Category = "CosmicSectorSpawner")
	ACosmicSector* SpawnSector();

private:
	void UpdateSpawnRangePreview() const;
	void StartSpawnTimer();
	void StopSpawnTimer();
	void SpawnSectorFromTimer();
	bool ChooseLetter(FString& OutLetter, bool& bOutIsCorrectLetter) const;
	bool ChooseRandomCorrectLetter(FString& OutLetter) const;
	bool ChooseRandomIncorrectLetter(FString& OutLetter) const;
	TArray<FString> ExtractUniqueLetters(const FString& Source) const;
	bool ContainsLetter(const TArray<FString>& Letters, const FString& Letter) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> SpawnRangePreview = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACosmicSector> SectorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner", meta = (AllowPrivateAccess = "true"))
	bool bSpawnEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float HorizontalSpawnRange = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner|Editor Preview", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float PreviewDepth = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner|Editor Preview", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float PreviewHeight = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner", meta = (AllowPrivateAccess = "true", ClampMin = "0.05"))
	float SpawnPeriod = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner|Letters", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float CorrectLetterProbability = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner|Letters", meta = (AllowPrivateAccess = "true"))
	FString IncorrectLetterPool = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float SpawnedSectorMovementSpeed = 120.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner|Letters", meta = (AllowPrivateAccess = "true"))
	FString TargetWord;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner|Letters", meta = (AllowPrivateAccess = "true"))
	TArray<FString> TargetLetters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicSectorSpawner|Letters", meta = (AllowPrivateAccess = "true"))
	TArray<FString> MissingLetters;

	UPROPERTY()
	TArray<TWeakObjectPtr<ACosmicSector>> SpawnedSectors;

	FTimerHandle SpawnTimerHandle;
};
