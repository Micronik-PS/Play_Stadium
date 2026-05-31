#include "PixelStreamingCursorUtils.h"

#include "Brushes/SlateColorBrush.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"

namespace
{
	const FSlateColorBrush WhiteCursorBrush(FLinearColor::White);
	const FSlateColorBrush AccentCursorBrush(FLinearColor(0.0f, 0.7f, 1.0f, 1.0f));

	TSharedRef<SWidget> MakeCursorLine(const FSlateBrush* Brush, float Width, float Height)
	{
		return SNew(SBox)
			.WidthOverride(Width)
			.HeightOverride(Height)
			[
				SNew(SImage)
				.Image(Brush)
			];
	}

	TSharedRef<SWidget> MakeDefaultCursorWidget()
	{
		return SNew(SBox)
			.WidthOverride(22.0f)
			.HeightOverride(22.0f)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				[
					MakeCursorLine(&WhiteCursorBrush, 16.0f, 3.0f)
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				[
					MakeCursorLine(&WhiteCursorBrush, 3.0f, 16.0f)
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				.Padding(3.0f, 3.0f, 0.0f, 0.0f)
				[
					MakeCursorLine(&AccentCursorBrush, 10.0f, 2.0f)
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				.Padding(3.0f, 3.0f, 0.0f, 0.0f)
				[
					MakeCursorLine(&AccentCursorBrush, 2.0f, 10.0f)
				]
			];
	}

	TSharedRef<SWidget> MakeTextEditCursorWidget()
	{
		return SNew(SBox)
			.WidthOverride(10.0f)
			.HeightOverride(24.0f)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Fill)
				[
					MakeCursorLine(&WhiteCursorBrush, 3.0f, 24.0f)
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Top)
				[
					MakeCursorLine(&WhiteCursorBrush, 10.0f, 2.0f)
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Bottom)
				[
					MakeCursorLine(&WhiteCursorBrush, 10.0f, 2.0f)
				]
			];
	}
}

namespace PlayStadium::PixelStreamingCursor
{
	void EnsureSoftwareCursors(UWorld* World)
	{
		UGameViewportClient* ViewportClient = World ? World->GetGameViewport() : nullptr;
		if (!ViewportClient)
		{
			return;
		}

		if (!ViewportClient->HasSoftwareCursor(EMouseCursor::Default))
		{
			ViewportClient->AddSoftwareCursorFromSlateWidget(EMouseCursor::Default, MakeDefaultCursorWidget());
		}

		if (!ViewportClient->HasSoftwareCursor(EMouseCursor::TextEditBeam))
		{
			ViewportClient->AddSoftwareCursorFromSlateWidget(EMouseCursor::TextEditBeam, MakeTextEditCursorWidget());
		}
	}
}
