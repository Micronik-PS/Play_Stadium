#include "StartMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateTypes.h"

#include "Play_Stadium/Core/PlayStadiumGameInstance/PlayStadiumGameInstance.h"


constexpr float StartMenuContentWidth = 640.0f;
constexpr float StartMenuContentPadding = 36.0f;
constexpr float StartMenuButtonHeight = 64.0f;
constexpr float StartMenuButtonPadding = 12.0f;
constexpr float SpacingAfterTitle = 28.0f;
constexpr float SpacingBetweenButtons = 20.0f;

TSharedRef<SWidget> UStartMenuWidget::RebuildWidget()
{
        if (!WidgetTree)
        {
                WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
                WidgetTree->SetFlags(RF_Transactional);
        }

        if (WidgetTree)
        {
                WidgetTree->RootWidget = nullptr;
        }

        ResetLayoutReferences();
        BuildLayout();

        if (WidgetTree && WidgetTree->RootWidget)
        {
                return WidgetTree->RootWidget->TakeWidget();
        }

        return Super::RebuildWidget();
}

void UStartMenuWidget::NativePreConstruct()
{
        Super::NativePreConstruct();
}

void UStartMenuWidget::NativeConstruct()
{
        Super::NativeConstruct();

        BindButtonCallbacks();
}

void UStartMenuWidget::BuildLayout()
{
        if (!WidgetTree)
        {
                return;
        }

        RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
        WidgetTree->RootWidget = RootCanvas;

        UImage* BackgroundImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Background"));
        BackgroundImage->SetColorAndOpacity(FLinearColor(0.02f, 0.05f, 0.08f, 0.92f));

        if (UCanvasPanelSlot* BackgroundSlot = RootCanvas->AddChildToCanvas(BackgroundImage))
        {
                BackgroundSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
                BackgroundSlot->SetOffsets(FMargin(0.0f));
        }

        ContentBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ContentBorder"));
        ContentBorder->SetBrushColor(FLinearColor(0.06f, 0.08f, 0.15f, 0.95f));
        ContentBorder->SetPadding(FMargin(StartMenuContentPadding));
        ContentBorder->SetDesiredSizeScale(FVector2D(1.0f, 1.0f));

        if (UCanvasPanelSlot* BorderSlot = RootCanvas->AddChildToCanvas(ContentBorder))
        {
                BorderSlot->SetAnchors(FAnchors(0.5f, 0.5f));
                BorderSlot->SetAlignment(FVector2D(0.5f));
                BorderSlot->SetAutoSize(true);
        }

        ContentSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("ContentSizeBox"));
        ContentSizeBox->SetWidthOverride(StartMenuContentWidth);
        ContentSizeBox->SetMinDesiredHeight(400.0f);
        ContentBorder->SetContent(ContentSizeBox);

        UVerticalBox* VerticalBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MainContainer"));
        ContentSizeBox->SetContent(VerticalBox);

        TitleLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleLabel"));
        TitleLabel->SetText(FText::FromString(TEXT("Play Stadium")));
        TitleLabel->SetJustification(ETextJustify::Center);
        TitleLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.95f, 1.0f)));
        TitleLabel->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), 48));

        if (UVerticalBoxSlot* TitleSlot = VerticalBox->AddChildToVerticalBox(TitleLabel))
        {
                TitleSlot->SetHorizontalAlignment(HAlign_Center);
        }

        SubtitleLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SubtitleLabel"));
        SubtitleLabel->SetText(FText::FromString(TEXT("Образовательная игра-опрос")));
        SubtitleLabel->SetJustification(ETextJustify::Center);
        SubtitleLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.82f, 0.94f)));
        SubtitleLabel->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 24));

        if (UVerticalBoxSlot* SubtitleSlot = VerticalBox->AddChildToVerticalBox(SubtitleLabel))
        {
                SubtitleSlot->SetHorizontalAlignment(HAlign_Center);
                SubtitleSlot->SetPadding(FMargin(0.0f, 12.0f, 0.0f, SpacingAfterTitle));
        }

        StartTestButton = CreateMenuButton(FText::FromString(TEXT("Начать тест")), StartTestLabel);
        if (StartTestButton)
        {
                USizeBox* StartButtonContainer = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("StartButtonContainer"));
                StartButtonContainer->SetHeightOverride(StartMenuButtonHeight);
                StartButtonContainer->SetContent(StartTestButton);

                if (UVerticalBoxSlot* StartButtonSlot = VerticalBox->AddChildToVerticalBox(StartButtonContainer))
                {
                        StartButtonSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, SpacingBetweenButtons));
                        StartButtonSlot->SetHorizontalAlignment(HAlign_Fill);
                }
        }

        ExitButton = CreateMenuButton(FText::FromString(TEXT("Выход")), ExitLabel);
        if (ExitButton)
        {
                USizeBox* ExitButtonContainer = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("ExitButtonContainer"));
                ExitButtonContainer->SetHeightOverride(StartMenuButtonHeight);
                ExitButtonContainer->SetContent(ExitButton);

                if (UVerticalBoxSlot* ExitButtonSlot = VerticalBox->AddChildToVerticalBox(ExitButtonContainer))
                {
                        ExitButtonSlot->SetHorizontalAlignment(HAlign_Fill);
                }
        }
}

void UStartMenuWidget::BindButtonCallbacks()
{
        if (StartTestButton)
        {
                StartTestButton->OnClicked.RemoveAll(this);
                StartTestButton->OnClicked.AddDynamic(this, &UStartMenuWidget::HandleStartTestClicked);
        }

        if (ExitButton)
        {
                ExitButton->OnClicked.RemoveAll(this);
                ExitButton->OnClicked.AddDynamic(this, &UStartMenuWidget::HandleExitClicked);
        }
}

void UStartMenuWidget::ResetLayoutReferences()
{
        RootCanvas = nullptr;
        ContentBorder = nullptr;
        ContentSizeBox = nullptr;
        TitleLabel = nullptr;
        SubtitleLabel = nullptr;
        StartTestButton = nullptr;
        ExitButton = nullptr;
        StartTestLabel = nullptr;
        ExitLabel = nullptr;
}

UButton* UStartMenuWidget::CreateMenuButton(const FText& Label, TObjectPtr<UTextBlock>& OutLabel) const
{
        if (!WidgetTree)
        {
                OutLabel = nullptr;
                return nullptr;
        }

        UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());

        FButtonStyle ButtonStyle = Button->GetStyle();

        FSlateBrush NormalBrush = ButtonStyle.Normal;
        NormalBrush.TintColor = FSlateColor(FLinearColor(0.05f, 0.32f, 0.62f, 1.0f));
        ButtonStyle.Normal = NormalBrush;

        FSlateBrush HoveredBrush = ButtonStyle.Hovered;
        HoveredBrush.TintColor = FSlateColor(FLinearColor(0.08f, 0.45f, 0.85f, 1.0f));
        ButtonStyle.Hovered = HoveredBrush;

        FSlateBrush PressedBrush = ButtonStyle.Pressed;
        PressedBrush.TintColor = FSlateColor(FLinearColor(0.02f, 0.2f, 0.42f, 1.0f));
        ButtonStyle.Pressed = PressedBrush;

        ButtonStyle.NormalPadding = FMargin(StartMenuButtonPadding);
        ButtonStyle.PressedPadding = FMargin(StartMenuButtonPadding);
        Button->SetStyle(ButtonStyle);

        OutLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        OutLabel->SetText(Label);
        OutLabel->SetJustification(ETextJustify::Center);
        OutLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
        OutLabel->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Medium"), 24));

        if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Button->AddChild(OutLabel.Get())))
        {
                ButtonSlot->SetHorizontalAlignment(HAlign_Center);
                ButtonSlot->SetVerticalAlignment(VAlign_Center);
        }

        return Button;
}

void UStartMenuWidget::HandleStartTestClicked()
{
	if (APlayerController* OwningPlayer = GetOwningPlayer())
	{
		FInputModeGameOnly InputMode;
		OwningPlayer->SetInputMode(InputMode);
		OwningPlayer->bShowMouseCursor = false;
		OwningPlayer->bEnableClickEvents = false;
		OwningPlayer->bEnableMouseOverEvents = false;
	}

	if (!GetWorld())
	{
		return;
	}

        if (UPlayStadiumGameInstance* PlayStadiumGameInstance = GetWorld()->GetGameInstance<UPlayStadiumGameInstance>())
        {
                PlayStadiumGameInstance->HandleStartTestRequested();
        }
}

void UStartMenuWidget::HandleExitClicked()
{
        UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
