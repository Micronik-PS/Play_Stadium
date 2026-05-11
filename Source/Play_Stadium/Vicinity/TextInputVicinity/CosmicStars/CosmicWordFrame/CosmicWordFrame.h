#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmicWordFrame.generated.h"

class UBoxComponent;
class UPaperSprite;
class UPaperSpriteComponent;
class UUserWidget;
class UWidgetComponent;
class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCosmicWordFrameReachedDestinationSignature, ACosmicWordFrame*, WordFrame);

UCLASS()
class PLAY_STADIUM_API ACosmicWordFrame : public AActor
{
	GENERATED_BODY()
	
public:	
	ACosmicWordFrame();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "CosmicWordFrame|Content")
	void SetFrameText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "CosmicWordFrame|Content")
	void InitializeFrameLetter(const FString& InExpectedLetter, bool bRevealImmediately = false);

	UFUNCTION(BlueprintCallable, Category = "CosmicWordFrame|Content")
	bool RevealIfMatches(const FString& InLetter);

	UFUNCTION(BlueprintCallable, Category = "CosmicWordFrame|Movement")
	void SetMovementSpeed(float InSpeed);

	UFUNCTION(BlueprintPure, Category = "CosmicWordFrame|Movement")
	float GetCollisionHalfHeight() const;

	UFUNCTION(BlueprintPure, Category = "CosmicWordFrame|Content")
	bool IsRevealed() const { return bIsRevealed; }

	UFUNCTION(BlueprintPure, Category = "CosmicWordFrame|Content")
	FString GetExpectedLetter() const { return ExpectedLetter; }

	UPROPERTY(BlueprintAssignable, Category = "CosmicWordFrame|Events")
	FCosmicWordFrameReachedDestinationSignature OnReachedDestination;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* SpriteComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* TextWidgetComponent = nullptr;

private:
	void ApplyCollisionSettings() const;
	void ApplyRandomSprite();
	void ApplyTextLayout();
	void UpdateText();
	void HandleDestinationImpact(AActor* OtherActor);

	UFUNCTION()
	void HandleFrameHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleFrameOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Visual", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UPaperSprite>> FrameSprites;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Visual", meta = (AllowPrivateAccess = "true"))
	FVector FrameSpriteScale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true"))
	FText FrameText = FText::FromString(TEXT("TEXT"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true"))
	float TextWorldSize = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true"))
	float TextDepthOffset = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true"))
	FColor TextColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true"))
	int32 TextTranslucentSortPriority = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 MaxCharactersPerLine = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true"))
	FRotator TextWidgetRotation = FRotator(0.0f, 90.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true"))
	FVector2D TextWidgetDrawSize = FVector2D(160.0f, 80.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> TextWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Collision", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	FVector CollisionBoxExtent = FVector(50.0f, 10.0f, 50.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MovementSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Movement", meta = (AllowPrivateAccess = "true"))
	bool bMoveOnBeginPlay = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true"))
	FString ExpectedLetter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CosmicWordFrame|Content", meta = (AllowPrivateAccess = "true"))
	bool bIsRevealed = false;

	UPROPERTY()
	bool bHasReachedDestination = false;
};
