#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultMenuWidget.generated.h"

class UBorder;
class UButton;
class UCanvasPanel;
class USizeBox;
class SWidget;
class UTextBlock;

UCLASS()
class PLAY_STADIUM_API UResultMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

public:
	TSharedPtr<SWidget> GetInitialFocusWidget();

private:
	void BuildLayout();
	void BindButtonCallbacks();
	void ResetLayoutReferences();
	void RefreshScoreText();

	UButton* CreateMenuButton(const FText& Label, TObjectPtr<UTextBlock>& OutLabel) const;

	UFUNCTION()
	void HandleExitClicked();

private:
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> RootCanvas = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> ContentBorder = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USizeBox> ContentSizeBox = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TitleLabel = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ScoreLabel = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UButton> ExitButton = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ExitLabel = nullptr;
};
