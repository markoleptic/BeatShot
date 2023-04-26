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
class UButton;

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
	UEditableTextBox* InnerOffsetValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* LineLengthValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* LineWidthValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* OutlineOpacityValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* OutlineWidthValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* InnerOffsetSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* LineLengthSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* LineWidthSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* OutlineOpacitySlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* OutlineWidthSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ResetToDefaultButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* RevertCrossHairButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* SaveCrossHairButton;

private:
	UFUNCTION()
	void OnColorChanged(const FLinearColor& NewColor);
	UFUNCTION()
	void OnInnerOffsetValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnLineLengthValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnLineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnOutlineOpacityValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnOutlineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType);

	UFUNCTION()
	void OnInnerOffsetSliderChange(const float NewValue);
	UFUNCTION()
	void OnLineLengthSliderChange(const float NewValue);
	UFUNCTION()
	void OnLineWidthSliderChange(const float NewValue);
	UFUNCTION()
	void OnOutlineOpacitySliderChange(const float NewValue);
	UFUNCTION()
	void OnOutlineWidthSliderChange(const float NewValue);

	UFUNCTION()
	void OnResetToDefaultButtonClicked();
	UFUNCTION()
	void OnRevertCrossHairButtonClicked();
	UFUNCTION()
	void OnSaveCrossHairButtonClicked();

	/** Fills out all CrossHair Settings given PlayerSettings */
	void SetCrossHairOptions(const FPlayerSettings_CrossHair& CrossHairSettings);
	/** The PlayerSettings that were initially loaded */
	FPlayerSettings_CrossHair InitialCrossHairSettings;
	/** The PlayerSettings that are changed when interacting with the CrossHairSettingsWidget */
	FPlayerSettings_CrossHair NewCrossHairSettings;
};
