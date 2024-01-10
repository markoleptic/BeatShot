// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */

struct FLoadingScreenStyle;
DECLARE_DELEGATE(FOnLoadingScreenExitAnimComplete);

class USERINTERFACE_API SLoadingScreenWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreenWidget)
		: _LoadingScreenStyle(&FAppStyle::Get().GetWidgetStyle<FLoadingScreenStyle>("LoadingScreen")),
		_OnLoadingScreenExitAnimComplete()
		{}
		SLATE_STYLE_ARGUMENT(FLoadingScreenStyle, LoadingScreenStyle)
		SLATE_EVENT(FOnLoadingScreenExitAnimComplete, OnLoadingScreenExitAnimComplete)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void FadeToBlack();
	
	FLinearColor GetOverlayColorAndOpacity() const;

	bool bShouldFadeToBlack;
	FOnLoadingScreenExitAnimComplete OnLoadingScreenExitAnimComplete;
	const FLoadingScreenStyle* LoadingScreenStyle;
	TSharedPtr<SOverlay> MainOverlay;
	mutable double FadeToBlackStartTime = 0.0f;
	mutable double FadeToBlackEndTime = 0.0f;
	mutable double FadeToBlackCurrentTime = 0.0f;
	mutable float RenderOpacity = 1.0f;
	float FadeOutDuration = 0.75f;
};


