#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "ShuffleMode.h"

#include "PlayStadiumGameInstance.generated.h"


UCLASS()
class PLAY_STADIUM_API UPlayStadiumGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	ShuffleMode QuestionsShuffleMode;

};
