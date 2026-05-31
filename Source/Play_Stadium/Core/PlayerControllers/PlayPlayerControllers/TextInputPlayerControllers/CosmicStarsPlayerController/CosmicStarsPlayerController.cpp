#include "CosmicStarsPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Play_Stadium/Core/PixelStreaming/PixelStreamingCursorUtils.h"
#include "Play_Stadium/Core/GameModes/PlayGameModes/TextInputGameModes/CosmicStarsGameMode/CosmicStarsGameMode.h"
#include "Play_Stadium/Targets/CosmicSector/CosmicSector.h"

ACosmicStarsPlayerController::ACosmicStarsPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ACosmicStarsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ApplyMouseInputSettings();
}

void ACosmicStarsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ACosmicStarsPlayerController::HandleLeftMousePressed);
		InputComponent->BindTouch(IE_Pressed, this, &ACosmicStarsPlayerController::HandleTouchPressed);
	}
}

void ACosmicStarsPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		Subsystem->ClearAllMappings();

		if (InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	ApplyMouseInputSettings();
}

void ACosmicStarsPlayerController::ApplyMouseInputSettings()
{
	PlayStadium::PixelStreamingCursor::EnsureSoftwareCursors(GetWorld());

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CurrentMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void ACosmicStarsPlayerController::HandleLeftMousePressed()
{
	ApplyMouseInputSettings();

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	HandlePointerPressedAtScreenPosition(FVector2D(MouseX, MouseY));
}

void ACosmicStarsPlayerController::HandleTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
	ApplyMouseInputSettings();
	SetMouseLocation(FMath::RoundToInt(Location.X), FMath::RoundToInt(Location.Y));
	HandlePointerPressedAtScreenPosition(FVector2D(Location.X, Location.Y));
}

void ACosmicStarsPlayerController::HandlePointerPressedAtScreenPosition(const FVector2D& ScreenPosition)
{
	ACosmicSector* SelectedSector = GetSectorAtScreenPosition(ScreenPosition);
	if (!SelectedSector || !SelectedSector->CanBeSelected())
	{
		return;
	}

	ACosmicStarsGameMode* CosmicStarsGameMode = GetWorld()
		? Cast<ACosmicStarsGameMode>(GetWorld()->GetAuthGameMode())
		: nullptr;

	if (CosmicStarsGameMode)
	{
		CosmicStarsGameMode->HandleSectorSelected(SelectedSector);
	}

	ApplyMouseInputSettings();
}

ACosmicSector* ACosmicStarsPlayerController::GetSectorAtScreenPosition(const FVector2D& ScreenPosition) const
{
	FHitResult HitResult;
	if (!GetHitResultAtScreenPosition(ScreenPosition, ECC_Visibility, false, HitResult))
	{
		return nullptr;
	}

	ACosmicSector* SelectedSector = Cast<ACosmicSector>(HitResult.GetActor());
	if (!SelectedSector && HitResult.GetComponent())
	{
		SelectedSector = Cast<ACosmicSector>(HitResult.GetComponent()->GetOwner());
	}

	return SelectedSector;
}
