#include "CosmicBlasterQuestionBanner.h"

#include "Components/TextRenderComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInterface.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "UObject/ConstructorHelpers.h"

ACosmicBlasterQuestionBanner::ACosmicBlasterQuestionBanner()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	QuestionBackgroundSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("QuestionBackground"));
	QuestionBackgroundSpriteComponent->SetupAttachment(RootComponent);
	QuestionBackgroundSpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	QuestionBackgroundSpriteComponent->SetRelativeRotation(FRotator::ZeroRotator);
	QuestionBackgroundSpriteComponent->SetMobility(EComponentMobility::Static);

	CounterBackgroundSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CounterBackground"));
	CounterBackgroundSpriteComponent->SetupAttachment(RootComponent);
	CounterBackgroundSpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CounterBackgroundSpriteComponent->SetRelativeRotation(FRotator::ZeroRotator);
	CounterBackgroundSpriteComponent->SetMobility(EComponentMobility::Static);

	QuestionTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("QuestionText"));
	QuestionTextComponent->SetupAttachment(RootComponent);
	QuestionTextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Left);
	QuestionTextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	QuestionTextComponent->SetTextRenderColor(FColor::White);
	QuestionTextComponent->SetWorldSize(DefaultTextWorldSize);
	QuestionTextComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	QuestionTextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	QuestionTextComponent->SetMobility(EComponentMobility::Static);
	QuestionTextComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));

	CounterTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CounterText"));
	CounterTextComponent->SetupAttachment(RootComponent);
	CounterTextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	CounterTextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	CounterTextComponent->SetTextRenderColor(FColor::White);
	CounterTextComponent->SetWorldSize(DefaultTextWorldSize);
	CounterTextComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CounterTextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	CounterTextComponent->SetMobility(EComponentMobility::Static);
	CounterTextComponent->SetRelativeLocation(FVector(CounterOffset, TextDepthOffset, 0.0f));

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> UnlitTextMat(TEXT("/Engine/EngineMaterials/DefaultTextMaterialOpaque"));
	if (UnlitTextMat.Succeeded())
	{
		QuestionTextComponent->SetTextMaterial(UnlitTextMat.Object);
		CounterTextComponent->SetTextMaterial(UnlitTextMat.Object);
	}

	ApplyLayout();
}

void ACosmicBlasterQuestionBanner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyLayout();
	UpdateVisuals();
}

void ACosmicBlasterQuestionBanner::SetQuestionText(const FText& InText)
{
	QuestionText = InText;
	UpdateVisuals();
}

void ACosmicBlasterQuestionBanner::SetCounter(int32 InCurrent, int32 InTotal)
{
	CurrentValue = InCurrent;
	TotalValue = InTotal;
	UpdateVisuals();
}

void ACosmicBlasterQuestionBanner::UpdateVisuals()
{
	if (QuestionTextComponent)
	{
		QuestionTextComponent->SetText(QuestionText);
	}

	if (CounterTextComponent)
	{
		const FString CounterString = FString::Printf(TEXT("%d/%d"), CurrentValue, TotalValue);
		CounterTextComponent->SetText(FText::FromString(CounterString));
		CounterTextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	}
}

void ACosmicBlasterQuestionBanner::ApplyLayout()
{
	if (QuestionTextComponent)
	{
		QuestionTextComponent->SetRelativeLocation(FVector(0.0f, TextDepthOffset, 0.0f));
		QuestionTextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		QuestionTextComponent->SetWorldSize(DefaultTextWorldSize);
	}

	if (CounterTextComponent)
	{
		CounterTextComponent->SetRelativeLocation(FVector(CounterOffset, TextDepthOffset, 0.0f));
		CounterTextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		CounterTextComponent->SetWorldSize(DefaultTextWorldSize);
	}

	if (QuestionBackgroundSpriteComponent)
	{
		QuestionBackgroundSpriteComponent->SetRelativeLocation(FVector::ZeroVector);
		QuestionBackgroundSpriteComponent->SetRelativeRotation(FRotator::ZeroRotator);
	}

	if (CounterBackgroundSpriteComponent)
	{
		CounterBackgroundSpriteComponent->SetRelativeLocation(FVector(CounterOffset, 0.0f, 0.0f));
		CounterBackgroundSpriteComponent->SetRelativeRotation(FRotator::ZeroRotator);
	}
}
