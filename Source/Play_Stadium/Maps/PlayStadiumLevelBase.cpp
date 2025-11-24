#include "PlayStadiumLevelBase.h"

#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"

void APlayStadiumLevelBase::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (PlayerController && LevelCameraActor)
	{
		FViewTargetTransitionParams TransitionParams;
		PlayerController->SetViewTarget(LevelCameraActor, TransitionParams);
	}
}
