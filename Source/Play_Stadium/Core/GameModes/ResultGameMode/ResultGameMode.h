#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ResultGameMode.generated.h"

class AResultPlayerController;

UCLASS()
class PLAY_STADIUM_API AResultGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	void TryShowResultMenuForPlayers();
};
