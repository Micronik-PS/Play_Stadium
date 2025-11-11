#pragma once

#include "Blueprint/UserWidget.h"
#include "StartMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartMenuSimpleEvent);

/**
 * Widget representing the educational game's start screen.
 * Provides buttons to start the test or exit the application.
 */
UCLASS()
class PLAY_STADIUM_API UStartMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UStartMenuWidget(const FObjectInitializer& ObjectInitializer);

    /** Fired when the player requests to start the test. */
    UPROPERTY(BlueprintAssignable, Category = "Start Menu")
    FStartMenuSimpleEvent OnStartTestRequested;

    /** Fired when the player requests to exit the application. */
    UPROPERTY(BlueprintAssignable, Category = "Start Menu")
    FStartMenuSimpleEvent OnExitRequested;

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeDestruct() override;

private:
    void BuildLayout();
    void BindButtonCallbacks();
    void UnbindButtonCallbacks();

    UFUNCTION()
    void HandleStartTestClicked();

    UFUNCTION()
    void HandleExitClicked();

    UPROPERTY()
    class UButton* StartTestButton;

    UPROPERTY()
    class UButton* ExitButton;
};
