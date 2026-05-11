#include "Play_Stadium/Vicinity/TextInputVicinity/CosmicStars/CosmicWordFrame/CosmicWordFrame.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "Play_Stadium/Core/UI/CosmicBubbleTextWidget/CosmicBubbleTextWidget.h"

ACosmicWordFrame::ACosmicWordFrame()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetBoxExtent(CollisionBoxExtent);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACosmicWordFrame::HandleFrameHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ACosmicWordFrame::HandleFrameOverlap);

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetupAttachment(CollisionComponent);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpriteComponent->SetRelativeScale3D(FrameSpriteScale);

	TextWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TextWidget"));
	TextWidgetComponent->SetupAttachment(CollisionComponent);
	TextWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TextWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	TextWidgetComponent->SetDrawAtDesiredSize(false);
	TextWidgetComponent->SetDrawSize(TextWidgetDrawSize);
	TextWidgetComponent->SetRelativeRotation(TextWidgetRotation);
	TextWidgetComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));
	TextWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	TextWidgetComponent->SetTwoSided(true);
	TextWidgetComponent->SetTranslucentSortPriority(TextTranslucentSortPriority + 1);
	TextWidgetComponent->SetHiddenInGame(false);
	TextWidgetComponent->SetVisibility(true);

	TextWidgetClass = UCosmicBubbleTextWidget::StaticClass();
	TextWidgetComponent->SetWidgetClass(TextWidgetClass);
}

void ACosmicWordFrame::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyCollisionSettings();
	ApplyRandomSprite();
	ApplyTextLayout();
	UpdateText();
}

void ACosmicWordFrame::BeginPlay()
{
	Super::BeginPlay();

	bHasReachedDestination = false;
	SetActorTickEnabled(bMoveOnBeginPlay);
	ApplyCollisionSettings();
	ApplyRandomSprite();
	ApplyTextLayout();
	UpdateText();
}

void ACosmicWordFrame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasReachedDestination || MovementSpeed <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	FHitResult HitResult;
	const FVector Delta = FVector(0.0f, 0.0f, -MovementSpeed * DeltaTime);
	AddActorWorldOffset(Delta, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		HandleDestinationImpact(HitResult.GetActor());
	}
}

void ACosmicWordFrame::SetFrameText(const FText& InText)
{
	FrameText = FText::FromString(InText.ToString().ToUpper());
	UpdateText();
}

void ACosmicWordFrame::InitializeFrameLetter(const FString& InExpectedLetter, bool bRevealImmediately)
{
	ExpectedLetter = InExpectedLetter.ToUpper();
	bIsRevealed = bRevealImmediately;
	FrameText = bRevealImmediately ? FText::FromString(ExpectedLetter) : FText::GetEmpty();
	UpdateText();
}

bool ACosmicWordFrame::RevealIfMatches(const FString& InLetter)
{
	if (bIsRevealed || ExpectedLetter.IsEmpty())
	{
		return false;
	}

	if (!ExpectedLetter.Equals(InLetter, ESearchCase::IgnoreCase))
	{
		return false;
	}

	bIsRevealed = true;
	FrameText = FText::FromString(ExpectedLetter);
	UpdateText();
	return true;
}

void ACosmicWordFrame::SetMovementSpeed(float InSpeed)
{
	MovementSpeed = FMath::Max(0.0f, InSpeed);
	SetActorTickEnabled(MovementSpeed > KINDA_SMALL_NUMBER);
}

float ACosmicWordFrame::GetCollisionHalfHeight() const
{
	if (CollisionComponent)
	{
		return CollisionComponent->GetScaledBoxExtent().Z;
	}

	return CollisionBoxExtent.Z * GetActorScale3D().Z;
}

void ACosmicWordFrame::ApplyCollisionSettings() const
{
	if (CollisionComponent)
	{
		CollisionComponent->SetBoxExtent(CollisionBoxExtent);
	}
}

void ACosmicWordFrame::ApplyRandomSprite()
{
	if (!SpriteComponent || FrameSprites.IsEmpty())
	{
		return;
	}

	const int32 SpriteIndex = FMath::RandRange(0, FrameSprites.Num() - 1);
	if (UPaperSprite* SelectedSprite = FrameSprites[SpriteIndex])
	{
		SpriteComponent->SetSprite(SelectedSprite);
		SpriteComponent->SetRelativeScale3D(FrameSpriteScale);
	}
}

void ACosmicWordFrame::ApplyTextLayout()
{
	if (TextWidgetComponent)
	{
		if (!TextWidgetClass)
		{
			TextWidgetClass = UCosmicBubbleTextWidget::StaticClass();
		}

		TextWidgetComponent->SetWidgetClass(TextWidgetClass);
		TextWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
		TextWidgetComponent->SetDrawAtDesiredSize(false);
		TextWidgetComponent->SetDrawSize(TextWidgetDrawSize);
		TextWidgetComponent->SetRelativeRotation(TextWidgetRotation);
		TextWidgetComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));
		TextWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
		TextWidgetComponent->SetTwoSided(true);
		TextWidgetComponent->SetTranslucentSortPriority(TextTranslucentSortPriority + 1);
		TextWidgetComponent->SetHiddenInGame(false);
		TextWidgetComponent->SetVisibility(true);
	}
}

void ACosmicWordFrame::UpdateText()
{
	if (!TextWidgetComponent)
	{
		return;
	}

	ApplyTextLayout();

	if (!TextWidgetClass)
	{
		TextWidgetClass = UCosmicBubbleTextWidget::StaticClass();
	}

	TextWidgetComponent->SetWidgetClass(TextWidgetClass);

	if (!TextWidgetComponent->GetWidget())
	{
		TextWidgetComponent->InitWidget();
	}

	if (UCosmicBubbleTextWidget* TextWidget = Cast<UCosmicBubbleTextWidget>(TextWidgetComponent->GetWidget()))
	{
		TextWidget->SetFontSize(FMath::Max(1, FMath::RoundToInt(TextWorldSize)));
		TextWidget->SetMaxLineLength(MaxCharactersPerLine);
		TextWidget->SetBubbleText(FrameText);
	}
}

void ACosmicWordFrame::HandleDestinationImpact(AActor* OtherActor)
{
	if (bHasReachedDestination || OtherActor == this)
	{
		return;
	}

	bHasReachedDestination = true;
	OnReachedDestination.Broadcast(this);
	Destroy();
}

void ACosmicWordFrame::HandleFrameHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HandleDestinationImpact(OtherActor);
}

void ACosmicWordFrame::HandleFrameOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleDestinationImpact(OtherActor);
}

