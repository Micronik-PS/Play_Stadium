#include "ResultPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/EngineTypes.h"

#include "Play_Stadium/Core/PixelStreaming/PixelStreamingCursorUtils.h"
#include "Play_Stadium/Core/UI/ResultMenuWidget/ResultMenuWidget.h"


void AResultPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	ShowResultMenu();
}

void AResultPlayerController::ShowResultMenu()
{
	if (!IsLocalController() || ResultMenuWidgetInstance)
	{
		return;
	}

	if (!ResultMenuWidgetClass)
	{
		return;
	}

	ResultMenuWidgetInstance = CreateWidget<UResultMenuWidget>(this, ResultMenuWidgetClass);
	if (!ResultMenuWidgetInstance)
	{
		return;
	}

	PlayStadium::PixelStreamingCursor::EnsureSoftwareCursors(GetWorld());
	ResultMenuWidgetInstance->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ResultMenuWidgetInstance->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CurrentMouseCursor = EMouseCursor::Default;
}
