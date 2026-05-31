#include "LaserConnectionPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Play_Stadium/Core/PixelStreaming/PixelStreamingCursorUtils.h"
#include "Play_Stadium/Maps/MatchingQuestionLevels/LaserConnection/LaserConnectionLevel.h"
#include "Play_Stadium/Targets/PowerCable/PowerCable.h"

ALaserConnectionPlayerController::ALaserConnectionPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ALaserConnectionPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ApplyMouseInputSettings();
}

void ALaserConnectionPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (DraggedCable.IsValid())
	{
		UpdateDraggedCable();
		TryAttachDraggedCable();
	}
}

void ALaserConnectionPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ALaserConnectionPlayerController::HandleLeftMousePressed);
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ALaserConnectionPlayerController::HandleLeftMouseReleased);
		InputComponent->BindTouch(IE_Pressed, this, &ALaserConnectionPlayerController::HandleTouchPressed);
		InputComponent->BindTouch(IE_Repeat, this, &ALaserConnectionPlayerController::HandleTouchMoved);
		InputComponent->BindTouch(IE_Released, this, &ALaserConnectionPlayerController::HandleTouchReleased);
	}
}

void ALaserConnectionPlayerController::OnPossess(APawn* InPawn)
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

void ALaserConnectionPlayerController::ApplyMouseInputSettings()
{
	PlayStadium::PixelStreamingCursor::EnsureSoftwareCursors(GetWorld());

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CurrentMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void ALaserConnectionPlayerController::HandleLeftMousePressed()
{
	bUsingTouchPointer = false;
	ActiveTouchIndex = ETouchIndex::MAX_TOUCHES;
	ApplyMouseInputSettings();
	BeginPointerDrag();
}

void ALaserConnectionPlayerController::HandleLeftMouseReleased()
{
	EndPointerDrag();
}

void ALaserConnectionPlayerController::HandleTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (ActiveTouchIndex != ETouchIndex::MAX_TOUCHES && ActiveTouchIndex != FingerIndex)
	{
		return;
	}

	bUsingTouchPointer = true;
	UpdateActiveTouchLocation(FingerIndex, Location);
	ApplyMouseInputSettings();
	BeginPointerDrag();
}

void ALaserConnectionPlayerController::HandleTouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
	UpdateActiveTouchLocation(FingerIndex, Location);
}

void ALaserConnectionPlayerController::HandleTouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (ActiveTouchIndex != FingerIndex)
	{
		return;
	}

	UpdateActiveTouchLocation(FingerIndex, Location);
	EndPointerDrag();
	bUsingTouchPointer = false;
	ActiveTouchIndex = ETouchIndex::MAX_TOUCHES;
	ActiveTouchScreenPosition = FVector2D::ZeroVector;
}

void ALaserConnectionPlayerController::BeginPointerDrag()
{
	APowerCable* Cable = GetCableUnderPointer();
	if (!Cable || !Cable->CanBeDragged())
	{
		return;
	}

	DraggedCable = Cable;
	DragPlaneY = Cable->GetActorLocation().Y;

	FVector MouseWorldPoint;
	if (GetPointerWorldPointOnDragPlane(MouseWorldPoint))
	{
		DragGrabOffset = Cable->GetActorLocation() - MouseWorldPoint;
		DragGrabOffset.Y = 0.0f;
	}
	else
	{
		DragGrabOffset = FVector::ZeroVector;
	}
}

void ALaserConnectionPlayerController::EndPointerDrag()
{
	if (DraggedCable.IsValid())
	{
		TryAttachDraggedCable();
	}

	DraggedCable.Reset();
	DragGrabOffset = FVector::ZeroVector;
	ApplyMouseInputSettings();
}

void ALaserConnectionPlayerController::UpdateActiveTouchLocation(ETouchIndex::Type FingerIndex, const FVector& Location)
{
	if (!bUsingTouchPointer || (ActiveTouchIndex != ETouchIndex::MAX_TOUCHES && ActiveTouchIndex != FingerIndex))
	{
		return;
	}

	ActiveTouchIndex = FingerIndex;
	ActiveTouchScreenPosition = FVector2D(Location.X, Location.Y);
	SetMouseLocation(FMath::RoundToInt(Location.X), FMath::RoundToInt(Location.Y));
}

void ALaserConnectionPlayerController::UpdateDraggedCable()
{
	APowerCable* Cable = DraggedCable.Get();
	if (!Cable || !Cable->CanBeDragged())
	{
		DraggedCable.Reset();
		return;
	}

	FVector MouseWorldPoint;
	if (!GetPointerWorldPointOnDragPlane(MouseWorldPoint))
	{
		return;
	}

	FVector NewLocation = Cable->GetActorLocation();
	NewLocation.X = MouseWorldPoint.X + DragGrabOffset.X;
	NewLocation.Y = DragPlaneY;
	NewLocation.Z = MouseWorldPoint.Z + DragGrabOffset.Z;
	Cable->SetActorLocation(NewLocation);
}

void ALaserConnectionPlayerController::TryAttachDraggedCable()
{
	APowerCable* Cable = DraggedCable.Get();
	if (!Cable || !Cable->CanBeDragged())
	{
		DraggedCable.Reset();
		return;
	}

	ALaserConnectionLevel* Level = GetWorld()
		? Cast<ALaserConnectionLevel>(GetWorld()->GetLevelScriptActor())
		: nullptr;

	if (!Level)
	{
		return;
	}

	APowerSocket* Socket = Level->FindClosestAvailableSocket(Cable->GetConnectionPointLocation(), AttachSearchRadius);
	if (Socket && Level->TryConnectCableToSocket(Cable, Socket))
	{
		DraggedCable.Reset();
		DragGrabOffset = FVector::ZeroVector;
		ApplyMouseInputSettings();
	}
}

APowerCable* ALaserConnectionPlayerController::GetCableUnderPointer() const
{
	FVector2D ScreenPosition;
	FHitResult HitResult;
	if (!GetPointerScreenPosition(ScreenPosition) || !GetHitResultAtScreenPosition(ScreenPosition, ECC_Visibility, false, HitResult))
	{
		return nullptr;
	}

	APowerCable* Cable = Cast<APowerCable>(HitResult.GetActor());
	if (!Cable && HitResult.GetComponent())
	{
		Cable = Cast<APowerCable>(HitResult.GetComponent()->GetOwner());
	}

	return Cable;
}

bool ALaserConnectionPlayerController::GetPointerScreenPosition(FVector2D& OutScreenPosition) const
{
	if (bUsingTouchPointer && ActiveTouchIndex != ETouchIndex::MAX_TOUCHES)
	{
		double TouchX = 0.0;
		double TouchY = 0.0;
		bool bTouchPressed = false;
		GetInputTouchState(ActiveTouchIndex, TouchX, TouchY, bTouchPressed);
		if (bTouchPressed)
		{
			OutScreenPosition = FVector2D(TouchX, TouchY);
			return true;
		}

		OutScreenPosition = ActiveTouchScreenPosition;
		return true;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!GetMousePosition(MouseX, MouseY))
	{
		return false;
	}

	OutScreenPosition = FVector2D(MouseX, MouseY);
	return true;
}

bool ALaserConnectionPlayerController::GetPointerWorldPointOnDragPlane(FVector& OutWorldPoint) const
{
	FVector WorldLocation;
	FVector WorldDirection;
	FVector2D ScreenPosition;
	if (!GetPointerScreenPosition(ScreenPosition) || !DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, WorldLocation, WorldDirection))
	{
		return false;
	}

	if (FMath::IsNearlyZero(WorldDirection.Y))
	{
		return false;
	}

	const float Distance = (DragPlaneY - WorldLocation.Y) / WorldDirection.Y;
	if (Distance < 0.0f)
	{
		return false;
	}

	OutWorldPoint = WorldLocation + WorldDirection * Distance;
	return true;
}
