#include "CosmicBubble.h"

#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "PaperFlipbookComponent.h"
#include "Play_Stadium/Core/UI/CosmicBubbleTextWidget/CosmicBubbleTextWidget.h"
#include "UObject/UnrealType.h"

ACosmicBubble::ACosmicBubble()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	BubbleFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("BubbleFlipbook"));
	BubbleFlipbookComponent->SetupAttachment(RootComponent);
	BubbleFlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BubbleWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("BubbleWidget"));
	BubbleWidgetComponent->SetupAttachment(RootComponent);
	BubbleWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BubbleWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	BubbleWidgetComponent->SetDrawAtDesiredSize(true);
	BubbleWidgetComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	BubbleWidgetComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));
	BubbleWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	BubbleWidgetComponent->SetTranslucentSortPriority(1);

	BubbleWidgetClass = UCosmicBubbleTextWidget::StaticClass();
	BubbleWidgetComponent->SetWidgetClass(BubbleWidgetClass);
}

void ACosmicBubble::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateText();
}

void ACosmicBubble::BeginPlay()
{
	Super::BeginPlay();

	UpdateText();
}

#if WITH_EDITOR
void ACosmicBubble::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateText();
}
#endif

void ACosmicBubble::SetBubbleText(const FText& InText)
{
	BubbleText = InText;
	UpdateText();
}

void ACosmicBubble::UpdateText()
{
	if (!BubbleWidgetComponent)
	{
		return;
	}

	if (!BubbleWidgetClass)
	{
		BubbleWidgetClass = UCosmicBubbleTextWidget::StaticClass();
	}

	BubbleWidgetComponent->SetWidgetClass(BubbleWidgetClass);
	BubbleWidgetComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));
	BubbleWidgetComponent->SetTranslucentSortPriority(1);

	if (!BubbleWidgetComponent->GetWidget())
	{
		BubbleWidgetComponent->InitWidget();
	}

	if (UCosmicBubbleTextWidget* BubbleTextWidget = Cast<UCosmicBubbleTextWidget>(BubbleWidgetComponent->GetWidget()))
	{
		const int32 FontSize = FMath::Max(1, FMath::RoundToInt(TextWorldSize));
		BubbleTextWidget->SetFontSize(FontSize);
		BubbleTextWidget->SetMaxLineLength(MaxCharactersPerLine);
		BubbleTextWidget->SetBubbleText(BubbleText);
	}
}
