#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenuWidget.generated.h"

class UBorder;
class UButton;
class UCanvasPanel;
class USizeBox;
class UTextBlock;

UCLASS()
class PLAY_STADIUM_API UStartMenuWidget : public UUserWidget
{
        GENERATED_BODY()

protected:
        virtual void NativeConstruct() override;

private:
        void BuildLayout();
        void BindButtonCallbacks();

        UButton* CreateMenuButton(const FText& Label, UTextBlock*& OutLabel) const;

        UFUNCTION()
        void HandleStartTestClicked();

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
        TObjectPtr<UTextBlock> SubtitleLabel = nullptr;

        UPROPERTY(Transient)
        TObjectPtr<UButton> StartTestButton = nullptr;

        UPROPERTY(Transient)
        TObjectPtr<UButton> ExitButton = nullptr;

        UPROPERTY(Transient)
        TObjectPtr<UTextBlock> StartTestLabel = nullptr;

        UPROPERTY(Transient)
        TObjectPtr<UTextBlock> ExitLabel = nullptr;

        bool bIsLayoutBuilt = false;
};
