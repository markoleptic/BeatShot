// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlideRightButton.generated.h"

class UHorizontalBox;
class UButton;
class UTextBlock;

/** Instanced button widget that provides a simple animation after clicking */
UCLASS()
class USERINTERFACE_API USlideRightButton : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	/** The font information for the Button */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	FSlateFontInfo DefaultFontInfo;
	
	/** The text to display on the Button */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	FText ButtonText;
	
	/** Plays the correct Button animation depending on if the button is the active selection or not */
	UFUNCTION(BlueprintCallable)
	void SlideButton(const bool bIsActiveSelection);
	
	/** The Button that provides us with OnClicked event */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Button;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* SlideButtonAnim;
	
	UFUNCTION()
	void SlideButtonRight() { PlayAnimationForward(SlideButtonAnim); }

	UFUNCTION()
	void SlideButtonLeft() { PlayAnimationReverse(SlideButtonAnim); }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* HorizontalBox;

	/** The widget to display the instance specific ButtonText */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ButtonDisplayText;

	/** Delegate used to remove the overlay from WebBrowserOverlay after FadeOut */
	UPROPERTY()
	FWidgetAnimationDynamicEvent ActivatedButtonDelegate;
};
