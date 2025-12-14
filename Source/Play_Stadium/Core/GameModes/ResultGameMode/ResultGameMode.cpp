#include "ResultGameMode.h"

#include "Play_Stadium/Core/PlayerControllers/ResultPlayerController/ResultPlayerController.h"


void AResultGameMode::BeginPlay()
{
	Super::BeginPlay();

	TryShowResultMenuForPlayers();
}

void AResultGameMode::TryShowResultMenuForPlayers()
{
	if (!GetWorld())
	{
		return;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AResultPlayerController* ResultController = Cast<AResultPlayerController>(*It))
		{
			ResultController->ShowResultMenu();
		}
	}
}
