﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuOptionWidget.generated.h"

class UCheckBox;
class UTextBlock;
class UEditableTextBox;
class USpacer;
class UHorizontalBox;
class USlider;
class UTooltipImage;
class UBSHorizontalBox;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLockStateChanged, const bool);

UCLASS()
class USERINTERFACE_API UMenuOptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetIndentLevel(const int32 Value);
	
	void SetShowTooltipImage(const bool bShow);
	void SetShowTooltipWarningImage(const bool bShow);
	void SetShowCheckBoxLock(const bool bShow);
	
	void SetDescriptionText(const FText& InText);
	
	void SetTooltipText(const FText& InText);
	void SetTooltipWarningText(const FText& InText);

	UTooltipImage* GetTooltipImage() const { return TooltipImage; }
	UTooltipImage* GetTooltipWarningImage() const { return TooltipWarningImage; }
	bool GetTooltipWarningVisible() const;
	FText GetTooltipRegularText() const { return TooltipRegularText; }
	FText GetTooltipWarningText() const { return TooltipWarningText; }
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
	UTooltipImage* TooltipWarningImage;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* TooltipBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USpacer* Indent_Left;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Description;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox_Lock;

	/* Text that describes the values this widget controls */
	UPROPERTY(EditInstanceOnly, Category="SliderTextBox")
	FText DescriptionText = FText();

	UPROPERTY(EditInstanceOnly, Category="SliderTextBox")
	int32 IndentLevel = 0;
	
	UPROPERTY(EditInstanceOnly, Category="SliderTextBox")
	bool bShowCheckBoxLock = false;

	UPROPERTY(EditInstanceOnly, Category="SliderTextBox|Tooltip")
	bool bShowTooltipImage = true;
	
	/* Text to show on the tooltip */
	UPROPERTY(EditInstanceOnly, Category="SliderTextBox|Tooltip")
	FText TooltipRegularText = FText();

	UPROPERTY(EditInstanceOnly, Category="SliderTextBox|Tooltip")
	bool bShowTooltipWarningImage = false;

	/* Text to show on the warning tooltip */
	UPROPERTY(EditInstanceOnly, Category="SliderTextBox|Tooltip")
	FText TooltipWarningText = FText();
	
	float GridSnapSize = 1.f;
};
