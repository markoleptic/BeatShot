// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "MenuOptionWidget.generated.h"

class UTooltipImage;

/** Data structure that contains info about a Tooltip Warning Image*/
USTRUCT()
struct FTooltipWarningValue
{
	GENERATED_BODY()
	
	UPROPERTY()
	UTooltipImage* TooltipWarningImage;
	
	FString TooltipStringTableKey;

	FText AdditionalTooltipText;

	FTooltipWarningValue()
	{
		TooltipWarningImage = nullptr;
		TooltipStringTableKey = FString();
		AdditionalTooltipText = FText();
	}

	FTooltipWarningValue(UTooltipImage* InTooltipImage, const FString& InKey, const FText& InAdditionalText = FText())
	{
		TooltipWarningImage = InTooltipImage;
		TooltipStringTableKey = InKey;
		AdditionalTooltipText = InAdditionalText;
	}

	explicit FTooltipWarningValue(const FString& InKey)
	{
		TooltipWarningImage = nullptr;
		TooltipStringTableKey = InKey;
		AdditionalTooltipText = FText();
	}

	FORCEINLINE bool operator==(const FTooltipWarningValue& Other) const
	{
		if (!TooltipStringTableKey.Equals(Other.TooltipStringTableKey))
		{
			return false;
		}
		if (!AdditionalTooltipText.EqualTo(Other.AdditionalTooltipText))
		{
			return false;
		}
		return true;
	}
	
	friend FORCEINLINE uint32 GetTypeHash(const FTooltipWarningValue& Value)
	{
		return HashCombine(GetTypeHash(Value.TooltipStringTableKey), GetTypeHash(Value.AdditionalTooltipText.ToString()));
	}
};

class UCheckBox;
class UTextBlock;
class UEditableTextBox;
class USpacer;
class UHorizontalBox;
class USlider;
class UBSHorizontalBox;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLockStateChanged, uint8, const bool);

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
	FText GetTooltipImageText() const { return TooltipImageText; }

	/** Returns the FTooltipWarningValue corresponding to The StringTableKey, or creates one if not found */
	FTooltipWarningValue FindOrAddTooltipWarningValue(const FString& InTooltipStringTableKey, const FText& OptionalAdditionalText = FText());

	/** Returns the StringTableKeys that correspond to each TooltipWarningImage */
	TArray<FString> GetTooltipWarningImageKeys() const;

	/** Returns the values of the WarningTooltips map */
	TArray<FTooltipWarningValue> GetTooltipWarningImageValues() const;

	/** Removes the TooltipWarningImage corresponding to the StringTableKey */
	void RemoveTooltipWarningImage(const FString& InTooltipStringTableKey);

	/** Removes all TooltipWarningImages */
	void RemoveAllTooltipWarningImages();

	/** Returns true if locked */
	bool GetIsLocked() const { return CheckBox_Lock->IsChecked(); }

	/** Sets the locked state */
	void SetIsLocked(const bool bLocked) const { CheckBox_Lock->SetIsChecked(bLocked); }

	void SetIndex(const uint8 InIndex) { Index = InIndex; }

	uint8 GetIndex() const { return Index; }
	
	/** Broadcasts the new state of the lock and the index */
	FOnLockStateChanged OnLockStateChanged;

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnCheckBox_LockStateChanged(const bool bChecked);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* TooltipImage;
	
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

	/** Text that describes the values this widget controls */
	UPROPERTY(EditInstanceOnly, Category="SliderTextBox")
	FText DescriptionText = FText();

	UPROPERTY(EditInstanceOnly, Category="SliderTextBox")
	int32 IndentLevel = 0;
	
	UPROPERTY(EditInstanceOnly, Category="SliderTextBox")
	bool bShowCheckBoxLock = false;

	UPROPERTY(EditInstanceOnly, Category="SliderTextBox|Tooltip")
	bool bShowTooltipImage = true;
	
	/** Text to show on the tooltip */
	UPROPERTY(EditInstanceOnly, Category="SliderTextBox|Tooltip")
	FText TooltipImageText = FText();
	
	/** A map of StringTableKeys to ToolTipWarningImages */
	UPROPERTY()
	TMap<FString, FTooltipWarningValue> WarningTooltips;

	uint8 Index = 0;
};
