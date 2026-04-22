#include "CosmicBubbleTextWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateColor.h"

TSharedRef<SWidget> UCosmicBubbleTextWidget::RebuildWidget()
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

void UCosmicBubbleTextWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	RefreshText();
}

void UCosmicBubbleTextWidget::SetBubbleText(const FText& InText)
{
	CurrentText = InText;
	RefreshText();
}

void UCosmicBubbleTextWidget::SetFontSize(int32 InFontSize)
{
	CurrentFontSize = FMath::Max(1, InFontSize);

	if (BubbleTextBlock)
	{
		FSlateFontInfo FontInfo = BubbleTextBlock->GetFont();
		FontInfo.Size = CurrentFontSize;
		BubbleTextBlock->SetFont(FontInfo);
	}
}

void UCosmicBubbleTextWidget::SetMaxLineLength(int32 InMaxLineLength)
{
	CurrentMaxLineLength = FMath::Max(1, InMaxLineLength);
	RefreshText();
}

void UCosmicBubbleTextWidget::BuildLayout()
{
	if (!WidgetTree)
	{
		return;
	}

	RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
	WidgetTree->RootWidget = RootCanvas;

	BubbleTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BubbleText"));
	BubbleTextBlock->SetJustification(ETextJustify::Center);
	BubbleTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	BubbleTextBlock->SetAutoWrapText(true);
	BubbleTextBlock->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), CurrentFontSize));

	if (UCanvasPanelSlot* TextSlot = RootCanvas->AddChildToCanvas(BubbleTextBlock))
	{
		TextSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		TextSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		TextSlot->SetAutoSize(true);
	}

	RefreshText();
}

void UCosmicBubbleTextWidget::ResetLayoutReferences()
{
	RootCanvas = nullptr;
	BubbleTextBlock = nullptr;
}

void UCosmicBubbleTextWidget::RefreshText()
{
	if (!BubbleTextBlock)
	{
		return;
	}

	BubbleTextBlock->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), CurrentFontSize));
	BubbleTextBlock->SetText(BuildWrappedText(CurrentText));
}

FText UCosmicBubbleTextWidget::BuildWrappedText(const FText& InText) const
{
	const FString SourceString = InText.ToString();
	const int32 MaxLength = FMath::Max(1, CurrentMaxLineLength);

	FString WrappedString;
	WrappedString.Reserve(SourceString.Len() + SourceString.Len() / MaxLength);

	int32 CurrentLength = 0;

	for (int32 Index = 0; Index < SourceString.Len(); ++Index)
	{
		const TCHAR Symbol = SourceString[Index];

		if (Symbol == TEXT('\n'))
		{
			CurrentLength = 0;
			WrappedString.AppendChar(Symbol);
			continue;
		}

		if (CurrentLength >= MaxLength)
		{
			WrappedString.AppendChar(TEXT('\n'));
			CurrentLength = 0;
		}

		WrappedString.AppendChar(Symbol);
		++CurrentLength;
	}

	return FText::FromString(WrappedString);
}
