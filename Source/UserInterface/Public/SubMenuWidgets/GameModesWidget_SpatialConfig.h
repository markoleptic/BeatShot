// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TooltipInterface.h"
#include "GlobalEnums.h"
#include "GlobalStructs.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "GameModesWidget_SpatialConfig.generated.h"

class UBSHorizontalBox;
class UTooltipImage;
class UEditableTextBox;
class UComboBoxString;
class USlider;
class UCheckBox;
class UTooltipImage;

UCLASS()
class USERINTERFACE_API UGameModesWidget_SpatialConfig : public UBSSettingCategoryWidget, public ITooltipInterface
{
	GENERATED_BODY()
	
	friend class UGameModesWidget;
	
	virtual void NativeConstruct() override;
	virtual UTooltipWidget* ConstructTooltipWidget() override;

public:
	void InitializeTargetSpread(const FBS_SpatialConfig& SpatialConfig, const EDefaultMode& BaseGameMode);
	FBS_SpatialConfig GetSpatialConfig() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Tooltip")
	TSubclassOf<UTooltipWidget> TooltipWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UCheckBox* CheckBox_HeadShotOnly;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UCheckBox* CheckBox_ForwardSpread;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UComboBoxString* ComboBox_SpreadType;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_MinTargetDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_MoveTargetsForward;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_ForwardSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_SpreadType;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_HeadShotOnly;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_FloorDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_HorizontalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_VerticalSpread;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	USlider* Slider_MinTargetDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	USlider* Slider_HorizontalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	USlider* Slider_VerticalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	USlider* Slider_ForwardSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	USlider* Slider_FloorDistance;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UEditableTextBox* Value_MinTargetDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UEditableTextBox* Value_HorizontalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UEditableTextBox* Value_VerticalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UEditableTextBox* Value_ForwardSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UEditableTextBox* Value_FloorDistance;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_HeadshotHeight;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_FloorDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_ForwardSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_MinDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_SpreadType;

	UFUNCTION()
	void OnCheckStateChanged_HeadShotOnly(const bool bHeadshotOnly);
	UFUNCTION()
	void OnSliderChanged_FloorDistance(const float NewFloorDistance);
	UFUNCTION()
	void OnTextCommitted_FloorDistance(const FText& NewFloorDistance, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_MinTargetDistance(const float NewMinTargetDistance);
	UFUNCTION()
	void OnTextCommitted_MinTargetDistance(const FText& NewMinTargetDistance, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_HorizontalSpread(const float NewHorizontalSpread);
	UFUNCTION()
	void OnTextCommitted_HorizontalSpread(const FText& NewHorizontalSpread, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_VerticalSpread(const float NewVerticalSpread);
	UFUNCTION()
	void OnTextCommitted_VerticalSpread(const FText& NewVerticalSpread, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnCheckStateChanged_MoveTargetsForward(const bool bUseForwardSpread);
	UFUNCTION()
	void OnSliderChanged_ForwardSpread(const float NewForwardSpread);
	UFUNCTION()
	void OnTextCommitted_ForwardSpread(const FText& NewForwardSpread, ETextCommit::Type CommitType);

	/** Returns the ESpreadType corresponding to the SpreadType string */
	ESpreadType GetSpreadType() const;
};
