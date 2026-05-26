#include "PowerCable.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "PaperSpriteComponent.h"
#include "Play_Stadium/Vicinity/MatchingQuestionLevels/LaserConnection/PowerSocket/PowerSocket.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectIterator.h"

APowerCable::APowerCable()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Movable);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetBoxExtent(CollisionExtent);
	CollisionComponent->SetMobility(EComponentMobility::Movable);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	CableSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CableSprite"));
	CableSpriteComponent->SetupAttachment(CollisionComponent);
	CableSpriteComponent->SetMobility(EComponentMobility::Movable);
	CableSpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CableTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CableText"));
	CableTextComponent->SetupAttachment(CollisionComponent);
	CableTextComponent->SetMobility(EComponentMobility::Movable);
	CableTextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	CableTextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	CableTextComponent->SetTextRenderColor(FColor::White);
	CableTextComponent->SetWorldSize(TextWorldSize);
	CableTextComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CableTextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	CableTextComponent->SetRelativeLocation(TextOffset);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> UnlitTextMat(TEXT("/Engine/EngineMaterials/DefaultTextMaterialOpaque"));
	if (UnlitTextMat.Succeeded())
	{
		CableTextComponent->SetTextMaterial(UnlitTextMat.Object);
	}

	CableTextComponent->SetText(Text);
}

void APowerCable::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyLayout();
	ApplyCollisionSettings();
	UpdateVisuals();
}

void APowerCable::BeginPlay()
{
	Super::BeginPlay();

	CacheInitialTransform();
	ApplyLayout();
	ApplyCollisionSettings();
	UpdateVisuals();
	SetActorTickEnabled(false);
}

#if WITH_EDITOR
void APowerCable::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ApplyLayout();
	ApplyCollisionSettings();
	UpdateVisuals();
	UpdateEditorPreviewInstances();
}

bool APowerCable::ShouldTickIfViewportsOnly() const
{
	return true;
}
#endif

void APowerCable::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if WITH_EDITOR
	const UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		ApplyLayout();
		ApplyCollisionSettings();
		UpdateVisuals();
	}
#endif
}

void APowerCable::SetCableText(const FText& InText)
{
	Text = InText;
	UpdateVisuals();
}

void APowerCable::SetPairData(const FString& InCableValue, const FString& InExpectedSocketValue)
{
	CableValue = InCableValue;
	ExpectedSocketValue = InExpectedSocketValue;
	SetCableText(FText::FromString(CableValue));
	ResetConnection();
}

FVector APowerCable::GetConnectionPointLocation() const
{
	return GetActorTransform().TransformPosition(GetEffectiveConnectionPointOffset());
}

bool APowerCable::LockToSocket(APowerSocket* Socket)
{
	if (!IsValid(Socket) || bIsConnected)
	{
		return false;
	}

	ConnectedSocket = Socket;
	bIsConnected = true;

	const FVector ConnectionOffsetWorld = GetActorTransform().TransformVector(GetEffectiveConnectionPointOffset());
	SetActorLocation(Socket->GetCableAttachLocation() - ConnectionOffsetWorld);

	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return true;
}

void APowerCable::ResetConnection()
{
	ConnectedSocket.Reset();
	bIsConnected = false;

	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
}

bool APowerCable::IsCorrectlyConnected() const
{
	const APowerSocket* Socket = ConnectedSocket.Get();
	return bIsConnected && Socket && Socket->GetSocketValue().Equals(ExpectedSocketValue, ESearchCase::CaseSensitive);
}

void APowerCable::CacheInitialTransform()
{
	if (bHasCachedInitialTransform)
	{
		return;
	}

	InitialTransform = GetActorTransform();
	bHasCachedInitialTransform = true;
}

void APowerCable::RestoreInitialTransform()
{
	if (bHasCachedInitialTransform)
	{
		SetActorTransform(InitialTransform);
	}
}

void APowerCable::UpdateVisuals()
{
	if (CableTextComponent)
	{
		CableTextComponent->SetText(Text);
		CableTextComponent->MarkRenderStateDirty();
	}
}

void APowerCable::ApplyLayout()
{
	if (CableTextComponent)
	{
		CableTextComponent->SetRelativeLocation(TextOffset);
		CableTextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		CableTextComponent->SetWorldSize(TextWorldSize);
	}
}

void APowerCable::ApplyCollisionSettings() const
{
	if (CollisionComponent)
	{
		CollisionComponent->SetBoxExtent(CollisionExtent);
	}
}

FVector APowerCable::GetEffectiveConnectionPointOffset() const
{
	if (!bUseCollisionLeftEdgeAsConnectionPoint)
	{
		return ConnectionPointOffset;
	}

	const FVector EffectiveExtent = CollisionComponent ? CollisionComponent->GetUnscaledBoxExtent() : CollisionExtent;
	return FVector(-FMath::Abs(EffectiveExtent.X), 0.0f, 0.0f);
}

#if WITH_EDITOR
void APowerCable::UpdateEditorPreviewInstances()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	for (TObjectIterator<APowerCable> It; It; ++It)
	{
		APowerCable* Cable = *It;
		if (!Cable || Cable == this || Cable->GetClass() != GetClass() || Cable->HasAnyFlags(RF_ClassDefaultObject))
		{
			continue;
		}

		Cable->Text = Text;
		Cable->ApplyLayout();
		Cable->ApplyCollisionSettings();
		Cable->UpdateVisuals();
	}
}
#endif
