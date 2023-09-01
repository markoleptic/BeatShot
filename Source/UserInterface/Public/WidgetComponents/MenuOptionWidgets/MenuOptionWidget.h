// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "WidgetComponents/TooltipImage.h"
#include "MenuOptionWidget.generated.h"

class UCheckBox;
class UTextBlock;
class UEditableTextBox;
class USpacer;
class UHorizontalBox;
class USlider;
class UBSHorizontalBox;
class UMenuOptionWidget;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLockStateChanged, UMenuOptionWidget*, const bool);

/** Base class for a Menu Option Widget, which is basically just a description, tooltip, and some value(s) that can be changed */
UCLASS()
class USERINTERFACE_API UMenuOptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Sets the left hand side indent, with each level increasing by 50 */
	void SetIndentLevel(const int32 Value);

	/** Toggles showing the TooltipImage */
	void SetShowTooltipImage(const bool bShow);

	/** Toggles showing the CheckBoxLock */
	void SetShowCheckBoxLock(const bool bShow);

	/** Sets the Description Text */
	void SetDescriptionText(const FText& InText);

	/** Sets the TooltipImage Text */
	void SetTooltipText(const FText& InText);

	/** Returns the TooltipImage */
	UTooltipImage* GetTooltipImage() const;

	/** Returns the TooltipImage Text */
	FText GetTooltipImageText() const { return DescriptionTooltipText; }

	/** Returns the FTooltipData corresponding to the StringTableKey, or creates one if not found */
	FTooltipData FindOrAddTooltip(const FString& InTooltipStringTableKey, const ETooltipImageType& TooltipType, const FText& OptionalAdditionalText = FText());

	/** Returns the StringTableKeys that correspond to each TooltipWarningImage */
	TArray<FString> GetTooltipWarningImageKeys() const;

	/** Returns the values of the WarningTooltips map */
	TArray<FTooltipData> GetAllTooltipData() const;

	/** Removes the TooltipWarningImage corresponding to the StringTableKey */
	void RemoveTooltipWarningImage(const FString& InTooltipStringTableKey);

	/** Removes all TooltipWarningImages */
	void RemoveAllTooltipWarningImages();

	/** Returns true if locked */
	bool GetIsLocked() const;

	/** Sets the locked state */
	void SetIsLocked(const bool bLocked) const;

	/** Returns value of bShowTooltipImage */
	bool ShouldShowTooltip() const { return bShowTooltipImage; }
	
	/** Broadcasts the new state of the lock and the index */
	FOnLockStateChanged OnLockStateChanged;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnCheckBox_LockStateChanged(const bool bChecked);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* DescriptionTooltip;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* TooltipBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USpacer* Indent_Left;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Description;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UCheckBox* CheckBox_Lock;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTooltipImage> TooltipWarningImageClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTooltipImage> TooltipCautionImageClass;

	/** Text that describes the values this widget controls */
	UPROPERTY(EditInstanceOnly, Category="MenuOptionWidget")
	FText DescriptionText = FText();

	UPROPERTY(EditInstanceOnly, Category="MenuOptionWidget")
	int32 IndentLevel = 0;
	
	UPROPERTY(EditInstanceOnly, Category="MenuOptionWidget")
	bool bShowCheckBoxLock = false;

	UPROPERTY(EditInstanceOnly, Category="MenuOptionWidget|Tooltip")
	bool bShowTooltipImage = true;
	
	/** Text to show on the tooltip */
	UPROPERTY(EditInstanceOnly, Category="MenuOptionWidget|Tooltip")
	FText DescriptionTooltipText = FText();
	
	/** A map of StringTableKeys to ToolTipWarningImages */
	//UPROPERTY()
	//TMap<FString, FTooltipData> WarningTooltips;
};
