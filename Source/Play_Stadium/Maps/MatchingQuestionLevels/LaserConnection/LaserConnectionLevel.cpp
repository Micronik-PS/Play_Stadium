#include "LaserConnectionLevel.h"

#include "Algo/RandomShuffle.h"
#include "Camera/CameraActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Play_Stadium/Targets/PowerCable/PowerCable.h"
#include "Play_Stadium/Vicinity/MatchingQuestionLevels/LaserConnection/PowerSocket/PowerSocket.h"
#include "Play_Stadium/Vicinity/SingleChoiceVicinity/CosmicBlaster/CosmicBlasterQuestionBanner/CosmicBlasterQuestionBanner.h"

DEFINE_LOG_CATEGORY_STATIC(LogLaserConnectionLevel, Log, All);

ALaserConnectionLevel::ALaserConnectionLevel()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALaserConnectionLevel::BeginPlay()
{
	Super::BeginPlay();

	ResolveActorReferences();
	ApplyStartupCamera();

	for (APowerCable* Cable : PowerCables)
	{
		if (Cable)
		{
			Cable->CacheInitialTransform();
		}
	}
}

void ALaserConnectionLevel::ApplyQuestionData(const FMatchingQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions)
{
	ResolveActorReferences();
	ResetCableConnections();

	bHasActiveQuestion = true;
	bQuestionAnswered = false;

	AssignPairsToActors(QuestionData);
	UpdateQuestionBanner(QuestionData, CurrentQuestionIndex, TotalQuestions);
}

TArray<APowerCable*> ALaserConnectionLevel::GetPowerCables() const
{
	TArray<APowerCable*> Result;
	Result.Reserve(PowerCables.Num());

	for (APowerCable* Cable : PowerCables)
	{
		if (IsValid(Cable))
		{
			Result.Add(Cable);
		}
	}

	return Result;
}

TArray<APowerSocket*> ALaserConnectionLevel::GetPowerSockets() const
{
	TArray<APowerSocket*> Result;
	Result.Reserve(PowerSockets.Num());

	for (APowerSocket* Socket : PowerSockets)
	{
		if (IsValid(Socket))
		{
			Result.Add(Socket);
		}
	}

	return Result;
}

APowerSocket* ALaserConnectionLevel::FindClosestAvailableSocket(const FVector& WorldLocation, float MaxDistance) const
{
	const float SearchRadius = MaxDistance > 0.0f ? MaxDistance : DefaultConnectionRadius;
	const float SearchRadiusSquared = FMath::Square(SearchRadius);

	APowerSocket* ClosestSocket = nullptr;
	float ClosestDistanceSquared = MAX_FLT;

	for (APowerSocket* Socket : PowerSockets)
	{
		if (!IsValid(Socket) || !Socket->CanAcceptCable())
		{
			continue;
		}

		const FVector AttachLocation = Socket->GetCableAttachLocation();
		const float EffectiveRadius = FMath::Max(SearchRadius, Socket->GetConnectionRadius());
		const float EffectiveRadiusSquared = FMath::Square(EffectiveRadius);
		const float DistanceSquared = FVector::DistSquared2D(FVector(WorldLocation.X, WorldLocation.Y, 0.0f), FVector(AttachLocation.X, AttachLocation.Y, 0.0f)) +
			FMath::Square(WorldLocation.Z - AttachLocation.Z);

		if (DistanceSquared <= FMath::Max(SearchRadiusSquared, EffectiveRadiusSquared) && DistanceSquared < ClosestDistanceSquared)
		{
			ClosestSocket = Socket;
			ClosestDistanceSquared = DistanceSquared;
		}
	}

	return ClosestSocket;
}

bool ALaserConnectionLevel::TryConnectCableToSocket(APowerCable* Cable, APowerSocket* Socket)
{
	if (!bHasActiveQuestion || bQuestionAnswered || !IsValid(Cable) || !IsValid(Socket) || !Cable->CanBeDragged() || !Socket->CanAcceptCable())
	{
		return false;
	}

	if (!Socket->AttachCable(Cable))
	{
		return false;
	}

	CheckForCompletedConnections();
	return true;
}

bool ALaserConnectionLevel::AreAllCablesConnected() const
{
	for (APowerCable* Cable : PowerCables)
	{
		if (!IsValid(Cable) || !Cable->IsConnected())
		{
			return false;
		}
	}

	return !PowerCables.IsEmpty();
}

bool ALaserConnectionLevel::AreAllConnectionsCorrect() const
{
	if (!AreAllCablesConnected())
	{
		return false;
	}

	for (APowerCable* Cable : PowerCables)
	{
		if (!IsValid(Cable) || !Cable->IsCorrectlyConnected())
		{
			return false;
		}
	}

	return true;
}

void ALaserConnectionLevel::ResolveActorReferences()
{
	if (!bAutoFindActorsOnLevel)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (!QuestionBanner)
	{
		for (TActorIterator<ACosmicBlasterQuestionBanner> It(World); It; ++It)
		{
			ACosmicBlasterQuestionBanner* CandidateBanner = *It;
			if (IsValid(CandidateBanner) && !CandidateBanner->IsActorBeingDestroyed())
			{
				QuestionBanner = CandidateBanner;
				break;
			}
		}
	}

	for (int32 CableIndex = PowerCables.Num() - 1; CableIndex >= 0; --CableIndex)
	{
		if (!IsValid(PowerCables[CableIndex]))
		{
			PowerCables.RemoveAtSwap(CableIndex);
		}
	}

	for (int32 SocketIndex = PowerSockets.Num() - 1; SocketIndex >= 0; --SocketIndex)
	{
		if (!IsValid(PowerSockets[SocketIndex]))
		{
			PowerSockets.RemoveAtSwap(SocketIndex);
		}
	}

	for (TActorIterator<APowerCable> It(World); It; ++It)
	{
		APowerCable* CandidateCable = *It;
		if (IsValid(CandidateCable) && !CandidateCable->IsActorBeingDestroyed())
		{
			PowerCables.AddUnique(CandidateCable);
		}
	}

	for (TActorIterator<APowerSocket> It(World); It; ++It)
	{
		APowerSocket* CandidateSocket = *It;
		if (IsValid(CandidateSocket) && !CandidateSocket->IsActorBeingDestroyed())
		{
			PowerSockets.AddUnique(CandidateSocket);
		}
	}

	PowerCables.Sort([](const TObjectPtr<APowerCable>& Left, const TObjectPtr<APowerCable>& Right)
	{
		const float LeftZ = Left ? Left->GetActorLocation().Z : -MAX_FLT;
		const float RightZ = Right ? Right->GetActorLocation().Z : -MAX_FLT;
		return LeftZ > RightZ;
	});

	PowerSockets.Sort([](const TObjectPtr<APowerSocket>& Left, const TObjectPtr<APowerSocket>& Right)
	{
		const float LeftZ = Left ? Left->GetActorLocation().Z : -MAX_FLT;
		const float RightZ = Right ? Right->GetActorLocation().Z : -MAX_FLT;
		return LeftZ > RightZ;
	});
}

void ALaserConnectionLevel::ApplyStartupCamera() const
{
	if (!LaserConnectionCameraActor)
	{
		return;
	}

	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PlayerController)
	{
		return;
	}

	FViewTargetTransitionParams TransitionParams;
	PlayerController->SetViewTarget(LaserConnectionCameraActor, TransitionParams);
}

void ALaserConnectionLevel::AssignPairsToActors(const FMatchingQuestionData& QuestionData)
{
	TArray<TPair<FString, FString>> Pairs;
	Pairs.Reserve(QuestionData.Pairs.Num());
	for (const TPair<FString, FString>& Pair : QuestionData.Pairs)
	{
		Pairs.Add(Pair);
	}

	Algo::RandomShuffle(Pairs);

	TArray<FString> SocketValues;
	SocketValues.Reserve(Pairs.Num());
	for (const TPair<FString, FString>& Pair : Pairs)
	{
		SocketValues.Add(Pair.Value);
	}

	Algo::RandomShuffle(SocketValues);

	for (int32 CableIndex = 0; CableIndex < PowerCables.Num(); ++CableIndex)
	{
		APowerCable* Cable = PowerCables[CableIndex];
		if (!Cable)
		{
			continue;
		}

		if (Pairs.IsValidIndex(CableIndex))
		{
			Cable->SetPairData(Pairs[CableIndex].Key, Pairs[CableIndex].Value);
		}
		else
		{
			Cable->SetPairData(FString(), FString());
		}
	}

	for (int32 SocketIndex = 0; SocketIndex < PowerSockets.Num(); ++SocketIndex)
	{
		APowerSocket* Socket = PowerSockets[SocketIndex];
		if (!Socket)
		{
			continue;
		}

		if (SocketValues.IsValidIndex(SocketIndex))
		{
			Socket->SetSocketValue(SocketValues[SocketIndex]);
		}
		else
		{
			Socket->SetSocketValue(FString());
		}
	}

	if (Pairs.Num() > PowerCables.Num() || SocketValues.Num() > PowerSockets.Num())
	{
		UE_LOG(LogLaserConnectionLevel, Warning, TEXT("Matching question has %d pairs, but level has %d cables and %d sockets."), Pairs.Num(), PowerCables.Num(), PowerSockets.Num());
	}
}

void ALaserConnectionLevel::ResetCableConnections()
{
	for (APowerSocket* Socket : PowerSockets)
	{
		if (Socket)
		{
			Socket->ClearConnection();
		}
	}

	for (APowerCable* Cable : PowerCables)
	{
		if (Cable)
		{
			Cable->ResetConnection();
			Cable->RestoreInitialTransform();
		}
	}
}

void ALaserConnectionLevel::UpdateQuestionBanner(const FMatchingQuestionData& QuestionData, int32 CurrentQuestionIndex, int32 TotalQuestions)
{
	if (!QuestionBanner)
	{
		UE_LOG(LogLaserConnectionLevel, Warning, TEXT("LaserConnectionLevel has no QuestionBanner. Place BP_CosmicBlasterQuestionBanner on the map or assign it in Details."));
		return;
	}

	QuestionBanner->SetQuestionText(FText::FromString(QuestionData.QuestionText));
	QuestionBanner->SetCounter(FMath::Max(1, CurrentQuestionIndex), FMath::Max(1, TotalQuestions));
}

void ALaserConnectionLevel::CheckForCompletedConnections()
{
	if (!bHasActiveQuestion || bQuestionAnswered || !AreAllCablesConnected())
	{
		return;
	}

	bQuestionAnswered = true;
	bHasActiveQuestion = false;
	OnQuestionAnswered.Broadcast(AreAllConnectionsCorrect());
}
