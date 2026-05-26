#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerSocket.generated.h"

class APowerCable;
class UBoxComponent;
class UPaperSpriteComponent;
class USceneComponent;
class UTextRenderComponent;

UCLASS()
class PLAY_STADIUM_API APowerSocket : public AActor
{
	GENERATED_BODY()
	
public:
	APowerSocket();

	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
#endif
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "PowerSocket|Content")
	void SetSocketText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "PowerSocket|Question")
	void SetSocketValue(const FString& InSocketValue);

	UFUNCTION(BlueprintPure, Category = "PowerSocket|Question")
	const FString& GetSocketValue() const { return SocketValue; }

	UFUNCTION(BlueprintPure, Category = "PowerSocket|Connection")
	bool IsConnected() const { return ConnectedCable.IsValid(); }

	UFUNCTION(BlueprintPure, Category = "PowerSocket|Connection")
	bool CanAcceptCable() const { return !ConnectedCable.IsValid(); }

	UFUNCTION(BlueprintPure, Category = "PowerSocket|Connection")
	APowerCable* GetConnectedCable() const { return ConnectedCable.Get(); }

	UFUNCTION(BlueprintCallable, Category = "PowerSocket|Connection")
	bool AttachCable(APowerCable* Cable);

	UFUNCTION(BlueprintCallable, Category = "PowerSocket|Connection")
	void ClearConnection();

	UFUNCTION(BlueprintPure, Category = "PowerSocket|Connection")
	FVector GetCableAttachLocation() const;

	UFUNCTION(BlueprintPure, Category = "PowerSocket|Connection")
	float GetConnectionRadius() const { return ConnectionRadius; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerSocket|Content", meta = (DisplayName = "Text"))
	FText Text = FText::FromString(TEXT("TEXT"));

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPaperSpriteComponent> SocketSpriteComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextRenderComponent> SocketTextComponent = nullptr;

private:
	void UpdateVisuals();
	void ApplyLayout();
	void ApplyCollisionSettings() const;
#if WITH_EDITOR
	void UpdateEditorPreviewInstances();
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerSocket|Question", meta = (AllowPrivateAccess = "true"))
	FString SocketValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerSocket|Layout", meta = (AllowPrivateAccess = "true"))
	FVector TextOffset = FVector(-110.0f, 1.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerSocket|Layout", meta = (AllowPrivateAccess = "true"))
	FVector CableAttachOffset = FVector(90.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerSocket|Layout", meta = (AllowPrivateAccess = "true"))
	float TextWorldSize = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerSocket|Collision", meta = (AllowPrivateAccess = "true"))
	FVector CollisionExtent = FVector(60.0f, 10.0f, 60.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerSocket|Connection", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float ConnectionRadius = 90.0f;

	UPROPERTY()
	TWeakObjectPtr<APowerCable> ConnectedCable;

};
