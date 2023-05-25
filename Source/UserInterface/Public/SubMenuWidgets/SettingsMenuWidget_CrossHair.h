// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "SettingsMenuWidget_CrossHair.generated.h"

class USavedTextWidget;
class UColorSelectWidget;
class UCrossHairWidget;
class USlider;
class UImage;
class UEditableTextBox;
class UBSButton;

/** Settings category widget holding CrossHair settings */
UCLASS()
class USERINTERFACE_API USettingsMenuWidget_CrossHair : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	/** Returns OnPlayerSettingsChangedDelegate_CrossHair, the delegate that is broadcast when this class saves CrossHair settings */
	FOnPlayerSettingsChanged_CrossHair& GetPublicCrossHairSettingsChangedDelegate() { return OnPlayerSettingsChangedDelegate_CrossHair; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCrossHairWidget* CrossHairWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UColorSelectWidget* ColorSelectWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_InnerOffset;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_LineLength;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_LineWidth;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_OutlineOpacity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_OutlineWidth;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_InnerOffset;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_LineLength;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_LineWidth;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_OutlineOpacity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_OutlineWidth;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Reset;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Revert;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Save;

private:
	UFUNCTION()
	void OnColorChanged(const FLinearColor& NewColor);
	UFUNCTION()
	void OnValueChanged_InnerOffset(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_LineLength(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_LineWidth(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_OutlineOpacity(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_OutlineWidth(const FText& NewValue, ETextCommit::Type CommitType);

	UFUNCTION()
	void OnSliderChanged_InnerOffset(const float NewValue);
	UFUNCTION()
	void OnSliderChanged_LineLength(const float NewValue);
	UFUNCTION()
	void OnSliderChanged_LineWidth(const float NewValue);
	UFUNCTION()
	void OnSliderChanged_OutlineOpacity(const float NewValue);
	UFUNCTION()
	void OnSliderChanged_OutlineWidth(const float NewValue);

	UFUNCTION()
	void OnButtonClicked_Reset();
	UFUNCTION()
	void OnButtonClicked_Revert();
	UFUNCTION()
	void OnButtonClicked_Save();
	UFUNCTION()
	void OnButtonClicked_BSButton(const UBSButton* Button);

	/** Fills out all CrossHair Settings given PlayerSettings */
	void SetCrossHairOptions(const FPlayerSettings_CrossHair& CrossHairSettings);
	/** The PlayerSettings that were initially loaded */
	FPlayerSettings_CrossHair InitialCrossHairSettings;
	/** The PlayerSettings that are changed when interacting with the CrossHairSettingsWidget */
	FPlayerSettings_CrossHair NewCrossHairSettings;
};
