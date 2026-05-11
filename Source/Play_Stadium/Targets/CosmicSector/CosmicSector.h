#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Play_Stadium/Targets/TargetDestroyReason.h"
#include "TimerManager.h"
#include "CosmicSector.generated.h"

class UBoxComponent;
class UPaperFlipbook;
class UPaperFlipbookComponent;
class UPaperSprite;
class UPaperSpriteComponent;
class UPrimitiveComponent;
class UTextRenderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCosmicSectorDestroyedSignature, ETargetDestroyReason, DestroyReason, bool, bWasCorrectLetter, FString, Letter);

UCLASS()
class PLAY_STADIUM_API ACosmicSector : public AActor
{
	GENERATED_BODY()
	
public:	
	ACosmicSector();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "CosmicSector|Content")
	void SetSectorText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "CosmicSector|Content")
	void SetLetterData(const FString& InLetter, bool bInIsCorrectLetter);

	UFUNCTION(BlueprintCallable, Category = "CosmicSector|Movement")
	void SetMovementSpeed(float InSpeed);

	UFUNCTION(BlueprintCallable, Category = "CosmicSector|Destruction")
	void TriggerDestruction(ETargetDestroyReason DestroyReason);

	UFUNCTION(BlueprintPure, Category = "CosmicSector|Content")
	FString GetLetter() const { return Letter; }

	UFUNCTION(BlueprintPure, Category = "CosmicSector|Content")
	bool IsCorrectLetter() const { return bIsCorrectLetter; }

	UFUNCTION(BlueprintPure, Category = "CosmicSector|Selection")
	bool CanBeSelected() const { return !bHasBeenDestroyed; }

	UFUNCTION(BlueprintPure, Category = "CosmicSector|Movement")
	float GetCollisionHalfHeight() const;

	UPROPERTY(BlueprintAssignable, Category = "CosmicSector|Events")
	FCosmicSectorDestroyedSignature OnSectorDestroyed;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* SpriteComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperFlipbookComponent* BreakingFlipbookComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* TextComponent = nullptr;

private:
	void ApplyCollisionSettings() const;
	void ApplyRandomSprite();
	void ApplyTextLayout() const;
	void ApplyBreakingFlipbook() const;
	void UpdateText();
	void HandleSectorImpact(AActor* OtherActor);
	void FinishDestruction();

	UFUNCTION()
	void HandleSectorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleSectorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSector|Visual", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UPaperSprite>> SectorSprites;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSector|Visual", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPaperFlipbook> BreakingFlipbook = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSector|Content", meta = (AllowPrivateAccess = "true"))
	FText SectorText = FText::FromString(TEXT("A"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSector|Content", meta = (AllowPrivateAccess = "true"))
	float TextWorldSize = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSector|Content", meta = (AllowPrivateAccess = "true"))
	float TextDepthOffset = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSector|Content", meta = (AllowPrivateAccess = "true"))
	FColor TextColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSector|Content", meta = (AllowPrivateAccess = "true"))
	int32 TextTranslucentSortPriority = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSector|Collision", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	FVector CollisionBoxExtent = FVector(50.0f, 10.0f, 50.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSector|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MovementSpeed = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicSector|Destruction", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float FallbackDestroyDelay = 0.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicSector|Content", meta = (AllowPrivateAccess = "true"))
	FString Letter = TEXT("A");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicSector|Content", meta = (AllowPrivateAccess = "true"))
	bool bIsCorrectLetter = false;

	UPROPERTY()
	bool bHasBeenDestroyed = false;

	FTimerHandle DestroyTimerHandle;
};
