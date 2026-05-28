// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatTextWidget.generated.h"

class UTextBlock;

/** Base widget used to show any combat text that appears when destroying targets. */
UCLASS()
class USERINTERFACE_API UCombatTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:
	/** Sets the combat text. */
	void SetText(const FText& InText) const;

	/** Begins the FadeOutAnimation. */
	void ShowCombatText();

	void ShowCombatTextBottom();

	/** Returns the delegate bound to the completion of the FadeOut animation. */
	FWidgetAnimationDynamicEvent& GetFadeOutDelegate() { return FadeOutDelegate; }

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOut;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutBottom;

	FWidgetAnimationDynamicEvent FadeOutDelegate;
};
