#include "Core/UI/StartMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Internationalization/Text.h"
#include "Layout/Margin.h"
#include "Math/Vector2D.h"

UStartMenuWidget::UStartMenuWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , StartTestButton(nullptr)
    , ExitButton(nullptr)
{
    bCanEverTick = false;
}

TSharedRef<SWidget> UStartMenuWidget::RebuildWidget()
{
    UnbindButtonCallbacks();

    if (!WidgetTree)
    {
        WidgetTree = NewObject<UWidgetTree>(this, UWidgetTree::StaticClass());
    }

    WidgetTree->RootWidget = nullptr;

    BuildLayout();
    BindButtonCallbacks();

    return Super::RebuildWidget();
}

void UStartMenuWidget::NativeDestruct()
{
    UnbindButtonCallbacks();
    Super::NativeDestruct();
}

void UStartMenuWidget::BuildLayout()
{
    if (!WidgetTree)
    {
        return;
    }

    UOverlay* RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RootOverlay"));
    WidgetTree->RootWidget = RootOverlay;

    UVerticalBox* ButtonBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ButtonBox"));
    if (ButtonBox && RootOverlay)
    {
        if (UOverlaySlot* OverlaySlot = RootOverlay->AddChildToOverlay(ButtonBox))
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Center);
            OverlaySlot->SetVerticalAlignment(VAlign_Center);
            OverlaySlot->SetPadding(FMargin(40.f));
        }
    }

    const FMargin ButtonPadding(10.f);

    // Start Test button setup
    StartTestButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("StartTestButton"));
    if (StartTestButton)
    {
        UTextBlock* StartLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StartTestLabel"));
        if (StartLabel)
        {
            StartLabel->SetText(FText::FromString(TEXT("Начать тест")));
            StartLabel->SetJustification(ETextJustify::Center);
            StartLabel->SetShadowOffset(FVector2D(1.f, 1.f));
        }

        if (StartLabel)
        {
            StartTestButton->AddChild(StartLabel);
        }

        if (ButtonBox)
        {
            if (UVerticalBoxSlot* StartSlot = ButtonBox->AddChildToVerticalBox(StartTestButton))
            {
                StartSlot->SetPadding(ButtonPadding);
                StartSlot->SetHorizontalAlignment(HAlign_Center);
            }
        }
    }

    // Exit button setup
    ExitButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ExitButton"));
    if (ExitButton)
    {
        UTextBlock* ExitLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ExitLabel"));
        if (ExitLabel)
        {
            ExitLabel->SetText(FText::FromString(TEXT("Выход")));
            ExitLabel->SetJustification(ETextJustify::Center);
            ExitLabel->SetShadowOffset(FVector2D(1.f, 1.f));
        }

        if (ExitLabel)
        {
            ExitButton->AddChild(ExitLabel);
        }

        if (ButtonBox)
        {
            if (UVerticalBoxSlot* ExitSlot = ButtonBox->AddChildToVerticalBox(ExitButton))
            {
                ExitSlot->SetPadding(ButtonPadding);
                ExitSlot->SetHorizontalAlignment(HAlign_Center);
            }
        }
    }
}

void UStartMenuWidget::BindButtonCallbacks()
{
    if (StartTestButton)
    {
        StartTestButton->OnClicked.AddDynamic(this, &UStartMenuWidget::HandleStartTestClicked);
    }

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &UStartMenuWidget::HandleExitClicked);
    }
}

void UStartMenuWidget::UnbindButtonCallbacks()
{
    if (StartTestButton)
    {
        StartTestButton->OnClicked.RemoveDynamic(this, &UStartMenuWidget::HandleStartTestClicked);
    }

    if (ExitButton)
    {
        ExitButton->OnClicked.RemoveDynamic(this, &UStartMenuWidget::HandleExitClicked);
    }
}

void UStartMenuWidget::HandleStartTestClicked()
{
    OnStartTestRequested.Broadcast();
}

void UStartMenuWidget::HandleExitClicked()
{
    OnExitRequested.Broadcast();
}
