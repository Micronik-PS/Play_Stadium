#include "PowerSocket.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "PaperSpriteComponent.h"
#include "Play_Stadium/Targets/PowerCable/PowerCable.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectIterator.h"

APowerSocket::APowerSocket()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetBoxExtent(CollisionExtent);
	CollisionComponent->SetMobility(EComponentMobility::Static);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	SocketSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SocketSprite"));
	SocketSpriteComponent->SetupAttachment(CollisionComponent);
	SocketSpriteComponent->SetMobility(EComponentMobility::Static);
	SocketSpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SocketTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SocketText"));
	SocketTextComponent->SetupAttachment(CollisionComponent);
	SocketTextComponent->SetMobility(EComponentMobility::Movable);
	SocketTextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Right);
	SocketTextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	SocketTextComponent->SetTextRenderColor(FColor::White);
	SocketTextComponent->SetWorldSize(TextWorldSize);
	SocketTextComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SocketTextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	SocketTextComponent->SetRelativeLocation(TextOffset);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> UnlitTextMat(TEXT("/Engine/EngineMaterials/DefaultTextMaterialOpaque"));
	if (UnlitTextMat.Succeeded())
	{
		SocketTextComponent->SetTextMaterial(UnlitTextMat.Object);
	}

	static ConstructorHelpers::FObjectFinder<UFont> DefaultTextFont(TEXT("/Game/Play_Stadium/Fonts/Font_Arial.Font_Arial"));
	if (DefaultTextFont.Succeeded())
	{
		TextFont = DefaultTextFont.Object;
		SocketTextComponent->SetFont(TextFont);
	}

	SocketTextComponent->SetText(Text);
}

void APowerSocket::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyLayout();
	ApplyCollisionSettings();
	UpdateVisuals();
}

void APowerSocket::BeginPlay()
{
	Super::BeginPlay();

	ApplyLayout();
	ApplyCollisionSettings();
	UpdateVisuals();
	SetActorTickEnabled(false);
}

#if WITH_EDITOR
void APowerSocket::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ApplyLayout();
	ApplyCollisionSettings();
	UpdateVisuals();
	UpdateEditorPreviewInstances();
}

bool APowerSocket::ShouldTickIfViewportsOnly() const
{
	return true;
}
#endif

void APowerSocket::Tick(float DeltaSeconds)
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

void APowerSocket::SetSocketText(const FText& InText)
{
	Text = InText;
	UpdateVisuals();
}

void APowerSocket::SetSocketValue(const FString& InSocketValue)
{
	SocketValue = InSocketValue;
	SetSocketText(FText::FromString(SocketValue));
	ClearConnection();
}

bool APowerSocket::AttachCable(APowerCable* Cable)
{
	if (!IsValid(Cable) || ConnectedCable.IsValid())
	{
		return false;
	}

	if (!Cable->LockToSocket(this))
	{
		return false;
	}

	ConnectedCable = Cable;
	return true;
}

void APowerSocket::ClearConnection()
{
	ConnectedCable.Reset();
}

FVector APowerSocket::GetCableAttachLocation() const
{
	return GetActorTransform().TransformPosition(CableAttachOffset);
}

void APowerSocket::UpdateVisuals()
{
	if (SocketTextComponent)
	{
		ApplyTextFont();
		SocketTextComponent->SetText(Text);
		SocketTextComponent->MarkRenderStateDirty();
	}
}

void APowerSocket::ApplyLayout()
{
	if (SocketTextComponent)
	{
		if (SocketTextComponent->Mobility != EComponentMobility::Movable)
		{
			SocketTextComponent->SetMobility(EComponentMobility::Movable);
		}

		SocketTextComponent->SetRelativeLocation(TextOffset);
		SocketTextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		SocketTextComponent->SetWorldSize(TextWorldSize);
		ApplyTextFont();
	}
}

void APowerSocket::ApplyTextFont()
{
	if (SocketTextComponent && TextFont)
	{
		SocketTextComponent->SetFont(TextFont);
	}
}

void APowerSocket::ApplyCollisionSettings() const
{
	if (CollisionComponent)
	{
		CollisionComponent->SetBoxExtent(CollisionExtent);
	}
}

#if WITH_EDITOR
void APowerSocket::UpdateEditorPreviewInstances()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	for (TObjectIterator<APowerSocket> It; It; ++It)
	{
		APowerSocket* Socket = *It;
		if (!Socket || Socket == this || Socket->GetClass() != GetClass() || Socket->HasAnyFlags(RF_ClassDefaultObject))
		{
			continue;
		}

		Socket->Text = Text;
		Socket->ApplyLayout();
		Socket->ApplyCollisionSettings();
		Socket->UpdateVisuals();
	}
}
#endif

