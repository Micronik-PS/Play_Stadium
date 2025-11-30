#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmicBlasterQuestionBanner.generated.h"

class UPaperSprite;
class UPaperSpriteComponent;
class UTextRenderComponent;
class USceneComponent;

UCLASS()
class PLAY_STADIUM_API ACosmicBlasterQuestionBanner : public AActor
{
	GENERATED_BODY()

public:
	ACosmicBlasterQuestionBanner();

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = "Question")
	void SetQuestionText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "Question")
	void SetCounter(int32 InCurrent, int32 InTotal);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* QuestionBackgroundSpriteComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* CounterBackgroundSpriteComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* QuestionTextComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* CounterTextComponent = nullptr;

private:
	void UpdateVisuals();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Banner|Content", meta = (AllowPrivateAccess = "true"))
	FText QuestionText = FText::FromString(TEXT("TEXT"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Banner|Content", meta = (AllowPrivateAccess = "true"))
	int32 CurrentValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Banner|Content", meta = (AllowPrivateAccess = "true"))
	int32 TotalValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Banner|Layout", meta = (AllowPrivateAccess = "true"))
	float CounterOffset = 140.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Banner|Layout", meta = (AllowPrivateAccess = "true"))
	float TextDepthOffset = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Banner|Layout", meta = (AllowPrivateAccess = "true"))
	float DefaultTextWorldSize = 24.0f;

};
