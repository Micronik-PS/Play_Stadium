#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
#include "BlasterGunProjectile.generated.h"

class UProjectileMovementComponent;
class UPaperSprite;
class UPrimitiveComponent;

UCLASS()
class PLAY_STADIUM_API ABlasterGunProjectile : public AActor
{
    GENERATED_BODY()

public:
    ABlasterGunProjectile();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* CollisionComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UPaperSpriteComponent* ProjectileSpriteComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (AllowPrivateAccess = "true"))
    UPaperSprite* ProjectileSprite = nullptr;

    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
