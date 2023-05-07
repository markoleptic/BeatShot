// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FAQWidget.generated.h"

class UBorder;
class UWidgetSwitcher;
class USlideRightButton;

/** FAQ widget */
UCLASS()
class USERINTERFACE_API UFAQWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

protected:
	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UBorder*> MenuWidgets;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* Button_GameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* Button_Scoring;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* Button_AudioAnalyzer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* FAQSwitcher;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Border_GameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Border_Scoring;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Border_AudioAnalyzer;

private:
	/** Function to Play the Slide Animation for Navigation Buttons */
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);

	UFUNCTION()
	void OnButtonClicked_GameModes();
	UFUNCTION()
	void OnButtonClicked_Scoring();
	UFUNCTION()
	void OnButtonClicked_AudioAnalyzer();
};
