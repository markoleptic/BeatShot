// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEnums.h"
#include "GlobalStructs.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "GameModesWidget_TargetConfig.generated.h"

class UBSHorizontalBox;
class UDoubleSyncedSliderAndTextBox;
class UCheckBox;
class UComboBoxString;

/** SettingCategoryWidget for the GameModesWidget that holds target configuration settings */
UCLASS()
class USERINTERFACE_API UGameModesWidget_TargetConfig : public UBSSettingCategoryWidget
{
	GENERATED_BODY()
	
	friend class UGameModesWidget;
	virtual void NativeConstruct() override;
	virtual void InitSettingCategoryWidget() override;

public:
	void InitializeTargetConfig(const FBS_TargetConfig& InTargetConfig, const EBaseGameMode& BaseGameMode);
	FBS_TargetConfig GetTargetConfig() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Target Scale")
	TSubclassOf<UDoubleSyncedSliderAndTextBox> TargetScaleConstrainedClass;

	TSoftObjectPtr<UDoubleSyncedSliderAndTextBox> TargetScaleConstrained;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	UBSHorizontalBox* BSBox_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	UBSHorizontalBox* BSBox_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	UBSHorizontalBox* BSBox_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	UBSHorizontalBox* BSBox_DynamicTargetScale;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	UBSHorizontalBox* BSBox_LifetimeTargetScale;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	USlider* Slider_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	USlider* Slider_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	USlider* Slider_SpawnBeatDelay;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	UEditableTextBox* Value_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	UEditableTextBox* Value_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	UEditableTextBox* Value_SpawnBeatDelay;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	UComboBoxString* ComboBox_LifetimeTargetScale;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target")
	UCheckBox* CheckBox_DynamicTargetScale;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_DynamicTargetScale;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_LifetimeTargetScale;

	UFUNCTION()
	void OnSliderChanged_Lifespan(const float NewLifespan);
	UFUNCTION()
	void OnSliderChanged_TargetSpawnCD(const float NewTargetSpawnCD);
	UFUNCTION()
	void OnSliderChanged_SpawnBeatDelay(const float NewPlayerDelay);
	UFUNCTION()
	void OnTextCommitted_Lifespan(const FText& NewLifespan, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_TargetSpawnCD(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_SpawnBeatDelay(const FText& NewPlayerDelay, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSelectionChanged_LifetimeTargetScaleMethod(const FString SelectedMethod, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnCheckStateChanged_DynamicTargetScale(const bool bIsChecked);
	UFUNCTION()
	void OnCheckStateChanged_ConstantTargetScale(const bool bIsChecked);

	const FText NewLineDelimit = FText::FromString("\n");
};
