#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Play_Stadium/Targets/TargetDestroyReason.h"
#include "CosmicBubble.generated.h"

class UPaperFlipbookComponent;
class USceneComponent;
class UWidgetComponent;
class UCosmicBubbleTextWidget;
class UUserWidget;
class USphereComponent;
class UPrimitiveComponent;
class ABlasterGunProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCosmicBubbleDestroyedSignature, ETargetDestroyReason, DestroyReason, bool, bWasCorrectChoice);

UCLASS()
class PLAY_STADIUM_API ACosmicBubble : public AActor
{
	GENERATED_BODY()

public:
	ACosmicBubble();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable, Category = "Bubble")
	void SetBubbleText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "Bubble")
	void SetChoiceData(const FText& InText, bool bInIsCorrectChoice);

	UFUNCTION(BlueprintCallable, Category = "Bubble|Movement")
	void SetMovementSpeed(float InSpeed);

	UFUNCTION(BlueprintPure, Category = "Bubble|Answer")
	bool IsCorrectChoice() const { return bIsCorrectChoice; }

	UFUNCTION(BlueprintPure, Category = "Bubble|Collision")
	float GetCollisionRadius() const;

	UPROPERTY(BlueprintAssignable, Category = "Bubble|Events")
	FCosmicBubbleDestroyedSignature OnBubbleDestroyed;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperFlipbookComponent* BubbleFlipbookComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* BubbleWidgetComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Content", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BubbleWidgetClass;

private:
	void UpdateText();
	void HandleBubbleImpact(AActor* OtherActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Content", meta = (AllowPrivateAccess = "true"))
	FText BubbleText = FText::FromString(TEXT("TEXT"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Layout", meta = (AllowPrivateAccess = "true"))
	float TextDepthOffset = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Layout", meta = (AllowPrivateAccess = "true"))
	float TextWorldSize = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Layout", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 MaxCharactersPerLine = 24;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Collision", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float CollisionRadius = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MovementSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MinimumMovementSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Answer", meta = (AllowPrivateAccess = "true"))
	bool bIsCorrectChoice = false;

	bool bHasBeenDestroyed = false;

	UFUNCTION()
	void HandleBubbleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleBubbleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
