#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmicButton.generated.h"

class USceneComponent;
class UBoxComponent;
class UPaperFlipbookComponent;
class UPaperZDAnimationComponent;
class UPrimitiveComponent;
class AFighterJetProjectile;
class AMeteor;

UCLASS()
class PLAY_STADIUM_API ACosmicButton : public AActor
{
	GENERATED_BODY()

public:
	ACosmicButton();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperFlipbookComponent* FlipbookComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperZDAnimationComponent* AnimationComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	FVector CollisionBoxExtent = FVector(100.0f, 100.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	FVector CollisionBoxOffset = FVector::ZeroVector;

private:
	void ActivateButton();
	void ApplyCollisionSettings() const;

	UFUNCTION()
	void HandleButtonOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
