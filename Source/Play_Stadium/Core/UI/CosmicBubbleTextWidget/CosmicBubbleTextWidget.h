#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CosmicBubbleTextWidget.generated.h"

class UCanvasPanel;
class UTextBlock;

/**
 * Simple widget that renders bubble text with manual line wrapping.
 */
UCLASS()
class PLAY_STADIUM_API UCosmicBubbleTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetBubbleText(const FText& InText);
	void SetFontSize(int32 InFontSize);
	void SetMaxLineLength(int32 InMaxLineLength);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativePreConstruct() override;

private:
	void BuildLayout();
	void ResetLayoutReferences();
	void RefreshText();
	FText BuildWrappedText(const FText& InText) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> RootCanvas = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> BubbleTextBlock = nullptr;

	FText CurrentText = FText::FromString(TEXT("TEXT"));
	int32 CurrentFontSize = 24;
	int32 CurrentMaxLineLength = 24;
};
