#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmicBubble.generated.h"

class UPaperFlipbookComponent;
class USceneComponent;
class UWidgetComponent;
class UCosmicBubbleTextWidget;
class UUserWidget;

UCLASS()
class PLAY_STADIUM_API ACosmicBubble : public AActor
{
	GENERATED_BODY()

public:
	ACosmicBubble();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable, Category = "Bubble")
	void SetBubbleText(const FText& InText);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperFlipbookComponent* BubbleFlipbookComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* BubbleWidgetComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Content", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BubbleWidgetClass;

private:
	void UpdateText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Content", meta = (AllowPrivateAccess = "true"))
	FText BubbleText = FText::FromString(TEXT("TEXT"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Layout", meta = (AllowPrivateAccess = "true"))
	float TextDepthOffset = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Layout", meta = (AllowPrivateAccess = "true"))
	float TextWorldSize = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bubble|Layout", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 MaxCharactersPerLine = 24;

};
