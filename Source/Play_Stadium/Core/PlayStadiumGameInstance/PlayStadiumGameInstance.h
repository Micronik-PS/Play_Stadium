#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "ShuffleMode.h"
#include "Play_Stadium/Core/Questions/QuestionBase.h"

#include "PlayStadiumGameInstance.generated.h"


UCLASS()
class PLAY_STADIUM_API UPlayStadiumGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:

	ShuffleMode QuestionsShuffleMode;

	TArray<UQuestionBase> Questions;

};
