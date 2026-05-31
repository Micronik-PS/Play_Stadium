#include "ResultMenuWidget.h"

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
#include "Kismet/KismetSystemLibrary.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateTypes.h"

#include "Play_Stadium/Core/PlayStadiumGameInstance/PlayStadiumGameInstance.h"
#include "Play_Stadium/Core/Questions/MatchingQuestion/MatchingQuestion.h"
#include "Play_Stadium/Core/Questions/MultipleChoiceQuestion/MultipleChoiceQuestion.h"
#include "Play_Stadium/Core/Questions/QuestionBase.h"
#include "Play_Stadium/Core/Questions/SingleChoiceQuestion/SingleChoiceQuestion.h"
#include "Play_Stadium/Core/Questions/TextInputQuestion/TextInputQuestion.h"


int32 GetRewardForQuestion(const UQuestionBase* Question)
{
	if (!Question)
	{
		return 0;
	}

	switch (Question->GetType())
	{
	case EQuestionType::SingleChoiceQuestion:
		if (const USingleChoiceQuestion* SingleChoice = Cast<USingleChoiceQuestion>(Question))
		{
			return SingleChoice->GetQuestionData().GetReward();
		}
		break;

	case EQuestionType::MultipleChoiceQuestion:
		if (const UMultipleChoiceQuestion* MultipleChoice = Cast<UMultipleChoiceQuestion>(Question))
		{
			return MultipleChoice->GetQuestionData().GetReward();
		}
		break;

	case EQuestionType::TextInputQuestion:
		if (const UTextInputQuestion* TextInput = Cast<UTextInputQuestion>(Question))
		{
			return TextInput->GetQuestionData().GetReward();
		}
		break;

	case EQuestionType::MatchingQuestion:
		if (const UMatchingQuestion* Matching = Cast<UMatchingQuestion>(Question))
		{
			return Matching->GetQuestionData().GetReward();
		}
		break;

	default:
		break;
	}

	return 0;
}

int32 CalculateMaximumScore(const UPlayStadiumGameInstance* GameInstance)
{
	if (!GameInstance)
	{
		return 0;
	}

	int32 TotalScore = 0;
	for (const TObjectPtr<UQuestionBase>& Question : GameInstance->GetQuestions())
	{
		TotalScore += GetRewardForQuestion(Question.Get());
	}

	return TotalScore;
}

constexpr float ContentWidth = 560.0f;
constexpr float ContentPadding = 28.0f;
constexpr float ButtonHeight = 60.0f;
constexpr float TitleSpacing = 22.0f;

TSharedRef<SWidget> UResultMenuWidget::RebuildWidget()
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

void UResultMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindButtonCallbacks();
	RefreshScoreText();
}

TSharedPtr<SWidget> UResultMenuWidget::GetInitialFocusWidget()
{
	if (ExitButton)
	{
		return ExitButton->TakeWidget();
	}

	return nullptr;
}

void UResultMenuWidget::BuildLayout()
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
	ContentBorder->SetPadding(FMargin(ContentPadding));
	ContentBorder->SetDesiredSizeScale(FVector2D(1.0f, 1.0f));

	if (UCanvasPanelSlot* BorderSlot = RootCanvas->AddChildToCanvas(ContentBorder))
	{
		BorderSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		BorderSlot->SetAlignment(FVector2D(0.5f));
		BorderSlot->SetAutoSize(true);
	}

	ContentSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("ContentSizeBox"));
	ContentSizeBox->SetWidthOverride(ContentWidth);
	ContentSizeBox->SetMinDesiredHeight(280.0f);
	ContentBorder->SetContent(ContentSizeBox);

	UVerticalBox* VerticalBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MainContainer"));
	ContentSizeBox->SetContent(VerticalBox);

	TitleLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleLabel"));
	TitleLabel->SetText(FText::FromString(TEXT("Результат")));
	TitleLabel->SetJustification(ETextJustify::Center);
	TitleLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.95f, 1.0f)));
	TitleLabel->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), 44));

	if (UVerticalBoxSlot* TitleSlot = VerticalBox->AddChildToVerticalBox(TitleLabel))
	{
		TitleSlot->SetHorizontalAlignment(HAlign_Center);
	}

	USizeBox* ScoreContainer = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("ScoreContainer"));
	ScoreContainer->SetWidthOverride(ContentWidth - (ContentPadding * 2.0f));

	ScoreLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ScoreLabel"));
	ScoreLabel->SetText(FText::FromString(TEXT("Итоговое количество очков : 0 / 0")));
	ScoreLabel->SetJustification(ETextJustify::Center);
	ScoreLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.88f, 0.96f)));
	ScoreLabel->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Medium"), 24));
	ScoreLabel->SetAutoWrapText(true);

	ScoreContainer->SetContent(ScoreLabel);

	if (UVerticalBoxSlot* ScoreSlot = VerticalBox->AddChildToVerticalBox(ScoreContainer))
	{
		ScoreSlot->SetHorizontalAlignment(HAlign_Center);
		ScoreSlot->SetPadding(FMargin(0.0f, TitleSpacing, 0.0f, TitleSpacing));
	}

	ExitButton = CreateMenuButton(FText::FromString(TEXT("Выйти")), ExitLabel);
	if (ExitButton)
	{
		USizeBox* ExitButtonContainer = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("ExitButtonContainer"));
		ExitButtonContainer->SetHeightOverride(ButtonHeight);
		ExitButtonContainer->SetContent(ExitButton);

		if (UVerticalBoxSlot* ExitButtonSlot = VerticalBox->AddChildToVerticalBox(ExitButtonContainer))
		{
			ExitButtonSlot->SetHorizontalAlignment(HAlign_Fill);
		}
	}
}

void UResultMenuWidget::BindButtonCallbacks()
{
	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveAll(this);
		ExitButton->OnClicked.AddDynamic(this, &UResultMenuWidget::HandleExitClicked);
	}
}

void UResultMenuWidget::ResetLayoutReferences()
{
	RootCanvas = nullptr;
	ContentBorder = nullptr;
	ContentSizeBox = nullptr;
	TitleLabel = nullptr;
	ScoreLabel = nullptr;
	ExitButton = nullptr;
	ExitLabel = nullptr;
}

void UResultMenuWidget::RefreshScoreText()
{
	if (!ScoreLabel)
	{
		return;
	}

	int32 CurrentScore = 0;
	int32 MaxScore = 0;

	if (const UWorld* World = GetWorld())
	{
		if (const UPlayStadiumGameInstance* PlayStadiumGameInstance = World->GetGameInstance<UPlayStadiumGameInstance>())
		{
			CurrentScore = PlayStadiumGameInstance->GetCurrentScore();
			MaxScore = CalculateMaximumScore(PlayStadiumGameInstance);
		}
	}

	const FText ScoreText = FText::FromString(FString::Printf(TEXT("Итоговое количество очков : %d / %d"), CurrentScore, MaxScore));
	ScoreLabel->SetText(ScoreText);
}

UButton* UResultMenuWidget::CreateMenuButton(const FText& Label, TObjectPtr<UTextBlock>& OutLabel) const
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

	constexpr float ButtonPadding = 12.0f;
	ButtonStyle.NormalPadding = FMargin(ButtonPadding);
	ButtonStyle.PressedPadding = FMargin(ButtonPadding);
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

void UResultMenuWidget::HandleExitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
