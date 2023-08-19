// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSButton.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBSButtonButtonPressed, const UBSButton*, BSButton);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBSButtonButtonPressed_NoParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPressedAnimFinished);

/** The base button used for BeatShot user interface. \n \n \b- Supports mutually exclusive buttons being "active" at once using pointers to other buttons and the SetActive and SetInActive functions, but can also
 *  be used for buttons not mutually exclusive if RestoreClickedButtonState is set to true. \n \b- ImageMaterial is expected to be a material instance with parameters that change the material when pressed or
 *  hovered. \n \b- Create functions in child classes like the SetDefaults function with additional parameters that can be retrieved using the OnBSButtonPressed delegate. */
UCLASS()
class USERINTERFACE_API UBSButton : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	/** Returns a pointer to the next BSButton in a linked list */
	virtual UBSButton* GetNext() const { return Next; }

	/** Called when another linked BSButton is clicked, reverses the OnPressed animation to restore it to pre-animated state */
	virtual void SetInActive();

	/** Manually "click" this button */
	virtual void SetActive();

	/** Create functions like this with additional parameters to store info about the button being pressed. Remember to call SetHasSetDefaults in any child implementations */
	void SetDefaults(const uint8 InEnum, UBSButton* NextButton = nullptr);

	/** Returns the uint8 enum value associated with this button */
	uint8 GetEnumValue() const { return EnumValue; }

	/** Returns whether or not this button has had its defaults set (next pointer and other data) */
	bool HasSetDefaults() const { return bHasSetDefaults; }

	/** Change the text of the button at runtime */
	void ChangeButtonText(const FText& InText);

	/** Broadcast when the button is pressed */
	FOnBSButtonButtonPressed OnBSButtonPressed;

	/** Broadcast when the button is pressed */
	FOnBSButtonButtonPressed_NoParams OnBSButtonButtonClicked_NoParams;
	
	/** Broadcast when the pressed animation is finished */
	FOnPressedAnimFinished OnPressedAnimFinished;

protected:
	/** Sets the value of bHasSetDefaults */
	void SetHasSetDefaults(const bool bHasSet) { bHasSetDefaults = bHasSet; }
	
	/** TextBlock to show on top of the ImageMaterial */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BSButton|Widgets",  meta = (BindWidget))
	UTextBlock* TextBlock;

	/** Invisible button used to access hovered/pressed events */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BSButton|Widgets", meta = (BindWidget))
	UButton* Button;

	/** Image shown as the button; Should be a material instance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BSButton|Widgets|Image", meta = (BindWidget))
	UImage* ImageMaterial;

	/** Name of the Hovered scalar material parameter for the ImageMaterial */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BSButton|Widgets|Image")
	FName HoveredScalarParameterName = "Hovered";

	/** Name of the Pressed scalar material parameter for the ImageMaterial */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BSButton|Widgets|Image")
	FName PressedScalarParameterName = "Pressed";

	/** Dynamic material instance of the ImageMaterial */
	UPROPERTY(BlueprintReadOnly, Category = "BSButton|Widgets|Image")
	UMaterialInstanceDynamic* DynamicImageMaterial;

	/** Font information for the Button text */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "BSButton|Defaults", meta = (ExposeOnSpawn="true"))
	FSlateFontInfo DefaultFontInfo;

	/** Text to display on top of ImageMaterial; Sets the TextBlock text */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "BSButton|Defaults", meta = (ExposeOnSpawn="true"))
	FText ButtonText;

	/** Playback speed used for all animations */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "BSButton|Defaults", meta = (ExposeOnSpawn="true"))
	float PlaybackSpeed = 3.f;

	/** Whether or not to restore the pre-pressed state of the button after pressing. This just sets the PressedScalarParameter value to 0 after the animation finishes */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "BSButton|Defaults", meta = (ExposeOnSpawn="true"))
	bool bRestoreClickedButtonState = false;
	
	/** Animation to play when the button is pressed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BSButton|Animation", Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_OnPressed;

	/** Animation to play when the button is hovered */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BSButton|Animation", Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_OnHovered;

	/** Pointer to the next BSButton in a collection of them */
	TObjectPtr<UBSButton> Next;

	/** Event delegate binding Anim_OnPressed to OnAnimFinished_OnPressed */
	FWidgetAnimationDynamicEvent OnAnimFinished_OnPressedEvent;

	/** The uint8 version of the Enum Value initialized during SetDefaults */
	uint8 EnumValue = 255;
	
	UFUNCTION()
	virtual void OnPressed_Button();
	UFUNCTION()
	virtual void OnHovered_Button();
	UFUNCTION()
	virtual void OnUnhovered_Button();

	UFUNCTION()
	virtual void PlayAnim_OnPressed();
	UFUNCTION()
	virtual void PlayAnim_OnHovered();
	
	UFUNCTION()
	virtual void PlayAnimReverse_OnPressed();
	UFUNCTION()
	virtual void PlayAnimReverse_OnHovered();
	
	UFUNCTION()
	virtual void OnAnimFinished_OnPressed();
	
	bool bIsClicked = false;
	bool bHasSetDefaults = false;
};
