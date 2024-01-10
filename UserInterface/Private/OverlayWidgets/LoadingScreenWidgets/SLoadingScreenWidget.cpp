// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/LoadingScreenWidgets/SLoadingScreenWidget.h"

#include "GlobalConstants.h"
#include "SlateOptMacros.h"
#include "Styles/LoadingScreenStyle.h"
#include "Widgets/Images/SSpinningImage.h"
#include "Widgets/Images/SThrobber.h"

int32 SLoadingScreenWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (bShouldFadeToBlack && FadeToBlackStartTime == 0)
	{
		FadeToBlackStartTime = Args.GetCurrentTime();
		FadeToBlackCurrentTime = FadeToBlackStartTime;
		FadeToBlackEndTime = FadeToBlackStartTime + FadeOutDuration;
	}
	else if (FadeToBlackStartTime > 0)
	{
		FadeToBlackCurrentTime += Args.GetDeltaTime();
		const float NewRenderOpacity = (FadeToBlackCurrentTime - FadeToBlackStartTime) / FadeOutDuration;
		if (NewRenderOpacity > 0.f && NewRenderOpacity <= 1.f)
		{
			RenderOpacity = 1 - NewRenderOpacity;
		}
		else
		{
			RenderOpacity = 0.f;
			if (OnLoadingScreenExitAnimComplete.IsBound())
			{
				OnLoadingScreenExitAnimComplete.Execute();
			}
		}
		MainOverlay->SetRenderOpacity(RenderOpacity);
	}
	
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
		bParentEnabled);
}

void SLoadingScreenWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SLoadingScreenWidget::FadeToBlack()
{
	bShouldFadeToBlack = true;
}

FLinearColor SLoadingScreenWidget::GetOverlayColorAndOpacity() const
{
	return FLinearColor::LerpUsingHSV(FLinearColor::Blue, FLinearColor::Green, RenderOpacity);
}

void SLoadingScreenWidget::Construct(const FArguments& InArgs)
{
	OnLoadingScreenExitAnimComplete = InArgs._OnLoadingScreenExitAnimComplete;
	//GetColorAndOpacityAttribute().ToAttribute().BindRaw(this, &SLoadingScreenWidget::GetOverlayColorAndOpacity);
	LoadingScreenStyle = InArgs._LoadingScreenStyle;

	ChildSlot
	[
		SNew(SOverlay)
			+SOverlay::Slot()
			[
			SAssignNew(MainOverlay, SOverlay)
			+SOverlay::Slot()
			.ZOrder(10000)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.Image(&LoadingScreenStyle->BackgroundImage)
			]
			+SOverlay::Slot()
			.ZOrder(10001)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(0,0,0,128))
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(64, 0, 0, 16))
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SImage)
						.DesiredSizeOverride(FVector2d(512, 512))
						.Image(&LoadingScreenStyle->LogoImage)
					]
					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Bottom)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(STextBlock)
							.Font(LoadingScreenStyle->LogoFontLeft)
							.Text(FText::FromString("BEAT"))
							.ColorAndOpacity(FLinearColor::White)
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SSpacer)
							.Size(FVector2d(30, 0))
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(STextBlock)
							.Font(LoadingScreenStyle->LogoFontLeft)
							.Text(FText::FromString("SHOT"))
							.ColorAndOpacity(Constants::BeatShotBlue)
						]
					]
				]
			]
		]
	];
}