// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "SliderTextBoxWidget.generated.h"

class UCheckBox;
class UTextBlock;
class UEditableTextBox;
class USpacer;
class UHorizontalBox;
class USlider;
class UTooltipImage;
class UBSHorizontalBox;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSliderTextBoxValueChanged, const float);

UCLASS()
class USERINTERFACE_API USliderTextBoxWidget : public UMenuOptionWidget
{
	GENERATED_BODY()

public:
	void SetValues(const float Min, const float Max, const float SnapSize);
	void SetValue(const float Value);

	FOnLockStateChanged OnLockStateChanged;
	FOnSliderTextBoxValueChanged OnSliderTextBoxValueChanged;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* EditableTextBox;

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnSliderChanged_Slider(const float Value);
	UFUNCTION()
	void OnTextCommitted_EditableTextBox(const FText& Text, ETextCommit::Type CommitType);
};
