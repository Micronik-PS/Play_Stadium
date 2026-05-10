#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmicSector.generated.h"

UCLASS()
class PLAY_STADIUM_API ACosmicSector : public AActor
{
	GENERATED_BODY()
	
public:	
	ACosmicSector();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
