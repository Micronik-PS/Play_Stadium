#include "LaserConnectionPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
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
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void ALaserConnectionPlayerController::HandleLeftMousePressed()
{
	ApplyMouseInputSettings();

	APowerCable* Cable = GetCableUnderCursor();
	if (!Cable || !Cable->CanBeDragged())
	{
		return;
	}

	DraggedCable = Cable;
	DragPlaneY = Cable->GetActorLocation().Y;

	FVector MouseWorldPoint;
	if (GetMouseWorldPointOnDragPlane(MouseWorldPoint))
	{
		DragGrabOffset = Cable->GetActorLocation() - MouseWorldPoint;
		DragGrabOffset.Y = 0.0f;
	}
	else
	{
		DragGrabOffset = FVector::ZeroVector;
	}
}

void ALaserConnectionPlayerController::HandleLeftMouseReleased()
{
	if (DraggedCable.IsValid())
	{
		TryAttachDraggedCable();
	}

	DraggedCable.Reset();
	DragGrabOffset = FVector::ZeroVector;
	ApplyMouseInputSettings();
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
	if (!GetMouseWorldPointOnDragPlane(MouseWorldPoint))
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

APowerCable* ALaserConnectionPlayerController::GetCableUnderCursor() const
{
	FHitResult HitResult;
	if (!GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
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

bool ALaserConnectionPlayerController::GetMouseWorldPointOnDragPlane(FVector& OutWorldPoint) const
{
	FVector WorldLocation;
	FVector WorldDirection;
	if (!DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
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
