#include "StartPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/EngineTypes.h"
#include "Play_Stadium/Core/PixelStreaming/PixelStreamingCursorUtils.h"
#include "Play_Stadium/Core/UI/StartMenuWidget/StartMenuWidget.h"


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
		PlayStadium::PixelStreamingCursor::EnsureSoftwareCursors(GetWorld());
		StartMenuWidgetInstance->AddToViewport();

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(StartMenuWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
		DefaultMouseCursor = EMouseCursor::Default;
		CurrentMouseCursor = EMouseCursor::Default;
	}
}
