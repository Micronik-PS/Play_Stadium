#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Play_Stadium/Targets/TargetDestroyReason.h"
#include "Meteor.generated.h"

class USceneComponent;
class USphereComponent;
class UPaperFlipbookComponent;
class UWidgetComponent;
class UCosmicBubbleTextWidget;
class UPrimitiveComponent;
class AFighterJetProjectile;
class AZD_FighterJet;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMeteorDestroyedSignature, ETargetDestroyReason, DestroyReason, bool, bWasCorrectChoice);

UCLASS()
class PLAY_STADIUM_API AMeteor : public AActor
{
	GENERATED_BODY()

public:
	AMeteor();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable, Category = "Meteor|Content")
	void SetMeteorText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "Meteor|Content")
	void SetChoiceData(const FText& InText, bool bInIsCorrectChoice);

	UFUNCTION(BlueprintCallable, Category = "Meteor|Movement")
	void SetMovementSpeed(float InSpeed);

	UFUNCTION(BlueprintPure, Category = "Meteor|Movement")
	float GetMinimumMovementSpeed() const;

	FTransform GetTextWidgetRelativeTransform() const;
	void SetTextWidgetRelativeTransform(const FTransform& InTransform);

	UFUNCTION(BlueprintCallable, Category = "Meteor|Destruction")
	void TriggerDestruction(ETargetDestroyReason DestroyReason);

	UPROPERTY(BlueprintAssignable, Category = "Meteor|Events")
	FMeteorDestroyedSignature OnMeteorDestroyed;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperFlipbookComponent* MeteorFlipbookComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* TextWidgetComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meteor|Content", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> TextWidgetClass;

private:
	void UpdateText();
	void HandleMeteorImpact(AActor* OtherActor, ETargetDestroyReason DestroyReason);
	void ApplyCollisionSettings() const;

	UFUNCTION()
	void HandleMeteorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleMeteorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meteor|Content", meta = (AllowPrivateAccess = "true"))
	FText MeteorText = FText::FromString(TEXT("TEXT"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meteor|Content", meta = (AllowPrivateAccess = "true"))
	bool bIsCorrectChoice = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meteor|Layout", meta = (AllowPrivateAccess = "true"))
	float TextDepthOffset = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meteor|Layout", meta = (AllowPrivateAccess = "true"))
	float TextWorldSize = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meteor|Layout", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 MaxCharactersPerLine = 24;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meteor|Collision", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float CollisionRadius = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meteor|Collision", meta = (AllowPrivateAccess = "true"))
	FVector CollisionOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meteor|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MovementSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meteor|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MinimumMovementSpeed = 3.0f;

	UPROPERTY()
	bool bHasBeenDestroyed = false;

	UPROPERTY()
	FTransform CachedTextWidgetRelativeTransform = FTransform::Identity;

private:
	void CacheTextWidgetRelativeTransform();
	float GetSafeMovementSpeed() const;
};
