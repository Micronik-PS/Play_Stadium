#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "PlayStadiumLevelBase.generated.h"

class ACameraActor;

UCLASS(Abstract)
class PLAY_STADIUM_API APlayStadiumLevelBase : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Camera")
	ACameraActor* LevelCameraActor = nullptr;
};
