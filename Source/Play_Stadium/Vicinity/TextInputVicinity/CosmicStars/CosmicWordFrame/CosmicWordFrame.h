#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmicWordFrame.generated.h"

UCLASS()
class PLAY_STADIUM_API ACosmicWordFrame : public AActor
{
	GENERATED_BODY()
	
public:	
	ACosmicWordFrame();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
