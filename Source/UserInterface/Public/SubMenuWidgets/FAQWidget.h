// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FAQWidget.generated.h"

class UBorder;
class UWidgetSwitcher;
class USlideRightButton;
/**
 * 
 */
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
	USlideRightButton* GameModesButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* ScoringButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* AudioAnalyzerButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* FAQSwitcher;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* GameModesBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ScoringBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* AudioAnalyzerBox;

private:

	/** Function to Play the Slide Animation for Navigation Buttons */
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	
	UFUNCTION()
	void OnGameModesButtonClicked();
	UFUNCTION()
	void OnScoringButtonClicked();
	UFUNCTION()
	void OnAudioAnalyzerButtonClicked();
};
