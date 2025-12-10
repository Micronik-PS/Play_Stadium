#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FighterJetProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class USceneComponent;
class UPrimitiveComponent;
class UPaperSpriteComponent;
class UPaperSprite;

UCLASS()
class PLAY_STADIUM_API AFighterJetProjectile : public AActor
{
	GENERATED_BODY()

public:
	AFighterJetProjectile();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	void SetFireDirection(const FVector& InDirection);

private:
	void ApplyCollisionSettings() const;
	void ApplyInitialVelocity() const;
	void DestroySelf();

	UFUNCTION()
	void HandleProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* SpriteComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (AllowPrivateAccess = "true"))
	UPaperSprite* ProjectileSprite = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float CollisionRadius = 32.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	FVector CollisionOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float InitialSpeed = 1500.0f;

	UPROPERTY()
	bool bHasImpacted = false;

	UPROPERTY()
	FVector FireDirectionOverride = FVector::ZeroVector;
};
