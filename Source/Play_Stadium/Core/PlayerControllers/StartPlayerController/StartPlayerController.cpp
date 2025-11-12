#include "StartPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Core/UI/StartMenuWidget.h"

void AStartPlayerController::BeginPlay()
{
        Super::BeginPlay();

        if (!IsLocalController())
        {
                return;
        }

        if (!StartMenuWidgetClass)
        {
                return;
        }

        StartMenuWidgetInstance = CreateWidget<UStartMenuWidget>(this, StartMenuWidgetClass);
        if (StartMenuWidgetInstance)
        {
                StartMenuWidgetInstance->AddToViewport();
        }
}
