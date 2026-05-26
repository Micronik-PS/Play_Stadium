#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerCable.generated.h"

class APowerSocket;
class UBoxComponent;
class UPaperSpriteComponent;
class USceneComponent;
class UTextRenderComponent;

UCLASS()
class PLAY_STADIUM_API APowerCable : public AActor
{
	GENERATED_BODY()
	
public:
	APowerCable();

	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
#endif
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "PowerCable|Content")
	void SetCableText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "PowerCable|Question")
	void SetPairData(const FString& InCableValue, const FString& InExpectedSocketValue);

	UFUNCTION(BlueprintPure, Category = "PowerCable|Question")
	const FString& GetCableValue() const { return CableValue; }

	UFUNCTION(BlueprintPure, Category = "PowerCable|Question")
	const FString& GetExpectedSocketValue() const { return ExpectedSocketValue; }

	UFUNCTION(BlueprintPure, Category = "PowerCable|Connection")
	bool IsConnected() const { return bIsConnected; }

	UFUNCTION(BlueprintPure, Category = "PowerCable|Connection")
	bool CanBeDragged() const { return !bIsConnected; }

	UFUNCTION(BlueprintPure, Category = "PowerCable|Connection")
	APowerSocket* GetConnectedSocket() const { return ConnectedSocket.Get(); }

	UFUNCTION(BlueprintPure, Category = "PowerCable|Connection")
	FVector GetConnectionPointLocation() const;

	UFUNCTION(BlueprintCallable, Category = "PowerCable|Connection")
	bool LockToSocket(APowerSocket* Socket);

	UFUNCTION(BlueprintCallable, Category = "PowerCable|Connection")
	void ResetConnection();

	UFUNCTION(BlueprintPure, Category = "PowerCable|Connection")
	bool IsCorrectlyConnected() const;

	void CacheInitialTransform();
	void RestoreInitialTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerCable|Content", meta = (DisplayName = "Text"))
	FText Text = FText::FromString(TEXT("TEXT"));

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPaperSpriteComponent> CableSpriteComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextRenderComponent> CableTextComponent = nullptr;

private:
	void UpdateVisuals();
	void ApplyLayout();
	void ApplyCollisionSettings() const;
	FVector GetEffectiveConnectionPointOffset() const;
#if WITH_EDITOR
	void UpdateEditorPreviewInstances();
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerCable|Question", meta = (AllowPrivateAccess = "true"))
	FString CableValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerCable|Question", meta = (AllowPrivateAccess = "true"))
	FString ExpectedSocketValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerCable|Layout", meta = (AllowPrivateAccess = "true"))
	FVector TextOffset = FVector(0.0f, 1.0f, 80.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerCable|Layout", meta = (AllowPrivateAccess = "true"))
	float TextWorldSize = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerCable|Collision", meta = (AllowPrivateAccess = "true"))
	FVector CollisionExtent = FVector(60.0f, 10.0f, 60.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerCable|Connection", meta = (AllowPrivateAccess = "true"))
	FVector ConnectionPointOffset = FVector(-60.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerCable|Connection", meta = (AllowPrivateAccess = "true"))
	bool bUseCollisionLeftEdgeAsConnectionPoint = true;

	UPROPERTY()
	TWeakObjectPtr<APowerSocket> ConnectedSocket;

	UPROPERTY()
	bool bIsConnected = false;

	UPROPERTY()
	FTransform InitialTransform = FTransform::Identity;

	UPROPERTY()
	bool bHasCachedInitialTransform = false;

};
