#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"

class ACameraActor;

#include "PlayStadiumLevelBase.generated.h"

UCLASS(Abstract)
class PLAY_STADIUM_API APlayStadiumLevelBase : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	/** Camera to set as the player's initial view target when the level loads. */
	UPROPERTY(EditAnywhere, Category = "Camera")
	ACameraActor* LevelCameraActor = nullptr;
};
