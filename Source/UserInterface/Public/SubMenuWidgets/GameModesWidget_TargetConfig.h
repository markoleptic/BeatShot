// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEnums.h"
#include "GlobalStructs.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "GameModesWidget_TargetConfig.generated.h"

class UBSComboBoxEntry;
class UBSComboBoxString;
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
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<UDoubleSyncedSliderAndTextBox> TargetScaleConstrainedClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<UDoubleSyncedSliderAndTextBox> TargetSpeedConstrainedClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<UBSComboBoxEntry> ComboboxEntryWidget;
	
	TSoftObjectPtr<UDoubleSyncedSliderAndTextBox> TargetScaleConstrained;
	TSoftObjectPtr<UDoubleSyncedSliderAndTextBox> TargetSpeedConstrained;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_SpawnBeatDelay;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ConsecutiveTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_LifetimeTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetSpawningPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetDestructionPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetActivationPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetDeactivationPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetDamageType;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_NumCharges;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ConsecutiveChargeScaleMultiplier;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ApplyImmunityOnSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ResetPositionOnDeactivation;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_NumCharges;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_ConsecutiveChargeScaleMultiplier;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_NumCharges;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_ConsecutiveChargeScaleMultiplier;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_LifetimeTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_ConsecutiveTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetSpawningPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetDestructionPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetActivationPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetDeactivationPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetDamageType;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox_ApplyImmunityOnSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox_ResetPositionOnDeactivation;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_LifetimeTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_ConsecutiveTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetSpawningPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetDestructionPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetActivationPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetDeactivationPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetDamageType;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_NumCharges;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_ConsecutiveChargeScaleMultiplier;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_ApplyImmunityOnSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_ResetPositionOnDeactivation;
	
	UFUNCTION()
	void OnSliderChanged_Lifespan(const float NewLifespan);
	UFUNCTION()
	void OnSliderChanged_TargetSpawnCD(const float NewTargetSpawnCD);
	UFUNCTION()
	void OnSliderChanged_SpawnBeatDelay(const float NewPlayerDelay);
	UFUNCTION()
	void OnSliderChanged_NumCharges(const float NewCharges);
	UFUNCTION()
	void OnSliderChanged_ChargeScaleMultiplier(const float NewChargeScaleMultiplier);
	
	UFUNCTION()
	void OnTextCommitted_Lifespan(const FText& NewLifespan, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_TargetSpawnCD(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_SpawnBeatDelay(const FText& NewPlayerDelay, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_NumCharges(const FText& NewCharges, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_ChargeScaleMultiplier(const FText& NewChargeScaleMultiplier, ETextCommit::Type CommitType);
	
	UFUNCTION()
	void OnSelectionChanged_LifetimeTargetScaleMethod(const FString SelectedPolicy, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_ConsecutiveTargetScale(const FString SelectedPolicy, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetSpawningPolicy(const FString SelectedPolicy, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDestructionPolicy(const FString SelectedPolicy, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetActivationPolicy(const FString SelectedPolicy, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDeactivationPolicy(const FString SelectedPolicy, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDamageType(const FString SelectedPolicy, const ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnCheckStateChanged_ApplyImmunityOnSpawn(const bool bApplyImmunityOnSpawn);
	UFUNCTION()
	void OnCheckStateChanged_ResetPositionOnDeactivation(const bool bApplyImmunityOnSpawn);
	
	const FText NewLineDelimit = FText::FromString("\n");

	UFUNCTION()
	UWidget* OnGenerateWidgetEvent_ConsecutiveTargetScale(FString Method);
	UFUNCTION()
	UWidget* OnGenerateWidgetEvent_LifetimeTargetScale(FString Method);
	UFUNCTION()
	UWidget* OnGenerateWidgetEvent_TargetSpawningPolicy(FString Method);
	UFUNCTION()
	UWidget* OnGenerateWidgetEvent_TargetDestructionPolicy(FString Method);
	UFUNCTION()
	UWidget* OnGenerateWidgetEvent_TargetActivationPolicy(FString Method);
	UFUNCTION()
	UWidget* OnGenerateWidgetEvent_TargetDeactivationPolicy(FString Method);
	UFUNCTION()
	UWidget* OnGenerateWidgetEvent_TargetDamageType(FString Method);
	
	UFUNCTION()
	UWidget* OnSelectionChangedGenerateWidgetEvent_ConsecutiveTargetScale(FString Method);
	UFUNCTION()
	UWidget* OnSelectionChangedGenerateWidgetEvent_LifetimeTargetScale(FString Method);
	UFUNCTION()
	UWidget* OnSelectionChangedGenerateWidgetEvent_TargetSpawningPolicy(FString Method);
	UFUNCTION()
	UWidget* OnSelectionChangedGenerateWidgetEvent_TargetDestructionPolicy(FString Method);
	UFUNCTION()
	UWidget* OnSelectionChangedGenerateWidgetEvent_TargetActivationPolicy(FString Method);
	UFUNCTION()
	UWidget* OnSelectionChangedGenerateWidgetEvent_TargetDeactivationPolicy(FString Method);
	UFUNCTION()
	UWidget* OnSelectionChangedGenerateWidgetEvent_TargetDamageType(FString Method);
	
	/** A very gross way to not show the tooltip image for the selected item widget */
	bool bHideTooltipImage_ConsecutiveTargetScale = false;
	bool bHideTooltipImage_LifetimeTargetScale = false;
	bool bHideTooltipImage_TargetSpawningPolicy = false;
	bool bHideTooltipImage_TargetDestructionPolicy = false;
	bool bHideTooltipImage_TargetActivationPolicy = false;
	bool bHideTooltipImage_TargetDeactivationPolicy = false;
	bool bHideTooltipImage_TargetDamageType = false;
};
