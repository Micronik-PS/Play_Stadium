#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmicWordFrameSpawner.generated.h"

UCLASS()
class PLAY_STADIUM_API ACosmicWordFrameSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ACosmicWordFrameSpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
