#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StartPlayerController.generated.h"

class UStartMenuWidget;

UCLASS()
class PLAY_STADIUM_API AStartPlayerController : public APlayerController
{
        GENERATED_BODY()

public:
        virtual void BeginPlay() override;

private:
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UStartMenuWidget> StartMenuWidgetClass;

        UPROPERTY(Transient)
        TObjectPtr<UStartMenuWidget> StartMenuWidgetInstance = nullptr;
};
