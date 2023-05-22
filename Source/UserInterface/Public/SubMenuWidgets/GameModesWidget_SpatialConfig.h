// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEnums.h"
#include "GlobalStructs.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "GameModesWidget_SpatialConfig.generated.h"

class UBSComboBoxString;
class UBSComboBoxEntry;
class UBSHorizontalBox;
class UTooltipImage;
class UEditableTextBox;
class UComboBoxString;
class USlider;
class UCheckBox;
class UTooltipImage;

/** SettingCategoryWidget for the GameModesWidget that holds Spatial configuration settings */
UCLASS()
class USERINTERFACE_API UGameModesWidget_SpatialConfig : public UBSSettingCategoryWidget
{
	GENERATED_BODY()
	
	friend class UGameModesWidget;
	virtual void NativeConstruct() override;
	virtual void InitSettingCategoryWidget() override;

public:
	void InitializeTargetSpread(const FBS_SpatialConfig& SpatialConfig, const EBaseGameMode& BaseGameMode);
	FBS_SpatialConfig GetSpatialConfig() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UCheckBox* CheckBox_ForwardSpread;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSComboBoxString* ComboBox_BoundsScalingMethod;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSComboBoxString* ComboBox_TargetDistributionMethod;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_MinTargetDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_MoveTargetsForward;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_ForwardSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_BoundsScalingMethod;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UBSHorizontalBox* BSBox_TargetDistributionMethod;
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

	UPROPERTY(EditDefaultsOnly, Category = "Tooltip")
	TSubclassOf<UBSComboBoxEntry> ComboboxEntryWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_FloorDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_ForwardSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_MinDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_BoundsScalingMethod;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_TargetDistributionMethod;
	
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
	UFUNCTION()
	void OnSelectionChanged_BoundsScalingMethod(const FString SelectedMethod, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDistributionMethod(const FString SelectedMethod, const ESelectInfo::Type SelectionType);

	UFUNCTION()
	UWidget* OnGenerateWidgetEvent_BoundsScalingMethod(FString Method);
	UFUNCTION()
	UWidget* OnSelectionChangedGenerateWidgetEvent_BoundsScalingMethod(FString Method);

	UFUNCTION()
	UWidget* OnGenerateWidgetEvent_TargetDistributionMethod(FString Method);
	UFUNCTION()
	UWidget* OnSelectionChangedGenerateWidgetEvent_TargetDistributionMethod(FString Method);

	/** A very gross way to not show the tooltip image for the selected item widget */
	bool bHideTooltipImage_BoundsScalingMethod = false;
	bool bHideTooltipImage_TargetDistributionMethod  = false;
};
