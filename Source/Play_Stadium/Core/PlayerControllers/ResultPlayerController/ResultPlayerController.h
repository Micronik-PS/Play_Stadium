#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ResultPlayerController.generated.h"

class UResultMenuWidget;

UCLASS()
class PLAY_STADIUM_API AResultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void ShowResultMenu();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UResultMenuWidget> ResultMenuWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UResultMenuWidget> ResultMenuWidgetInstance = nullptr;
};
