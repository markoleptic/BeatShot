// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEnums.h"
#include "GlobalStructs.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "GameModesWidget_TargetConfig.generated.h"

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
	virtual TSharedRef<SWidget> RebuildWidget() override;
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
	
	TSoftObjectPtr<UDoubleSyncedSliderAndTextBox> TargetScaleConstrained;
	TSoftObjectPtr<UDoubleSyncedSliderAndTextBox> TargetSpeedConstrained;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ApplyImmunityOnSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ContinuouslySpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_MoveTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_MoveTargetsForward;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_SpawnAtOriginWheneverPossible;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_SpawnEveryOtherTargetInCenter;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_BoundsScalingPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ConsecutiveTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_LifetimeTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_RecentTargetMemoryPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetActivationSelectionPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetDamageType;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetDistributionPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetSpawningPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetActivationResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetDeactivationConditions;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetDeactivationResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetDestructionConditions;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ConsecutiveChargeScaleMultiplier;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_MinTargetDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ForwardSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_FloorDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_HorizontalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_VerticalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_MaxNumRecentTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_RecentTargetTimeLength;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_NumUpfrontTargetsToSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ExpirationHealthPenalty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_MaxHealth;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_ConsecutiveChargeScaleMultiplier;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_MinTargetDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_HorizontalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_VerticalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_ForwardSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_FloorDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_MaxNumRecentTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_RecentTargetTimeLength;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_NumUpfrontTargetsToSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_ExpirationHealthPenalty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_MaxHealth;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_ConsecutiveChargeScaleMultiplier;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_MinTargetDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_HorizontalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_VerticalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_ForwardSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_FloorDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_MaxNumRecentTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_RecentTargetTimeLength;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_NumUpfrontTargetsToSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_ExpirationHealthPenalty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_MaxHealth;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_BoundsScalingPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_ConsecutiveTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_LifetimeTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_RecentTargetMemoryPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetActivationSelectionPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetDamageType;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetDistributionPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetSpawningPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetActivationResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetDeactivationConditions;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetDeactivationResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox_TargetDestructionConditions;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox_ApplyImmunityOnSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox_MoveTargetsForward;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox_ContinuouslySpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox_MoveTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox_SpawnAtOriginWheneverPossible;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox_SpawnEveryOtherTargetInCenter;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_Lifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetSpawnCD;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_FloorDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_ForwardSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_MinDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_ConsecutiveChargeScaleMultiplier;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_ApplyImmunityOnSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_MaxNumRecentTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_RecentTargetTimeLength;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_NumUpfrontTargetsToSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_ExpirationHealthPenalty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_MaxHealth;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_BoundsScalingPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_ConsecutiveTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_LifetimeTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_RecentTargetMemoryPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetActivationSelectionPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetDamageType;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetDistributionPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetSpawningPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetActivationResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetDeactivationConditions;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetDeactivationResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_TargetDestructionConditions;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_MoveTargetsForward;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_ContinuouslySpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_MoveTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_SpawnAtOriginWheneverPossible;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_SpawnEveryOtherTargetInCenter;
	
	UFUNCTION()
	void OnSliderChanged_Lifespan(const float NewLifespan);
	UFUNCTION()
	void OnSliderChanged_TargetSpawnCD(const float NewTargetSpawnCD);
	UFUNCTION()
	void OnSliderChanged_SpawnBeatDelay(const float NewPlayerDelay);
	UFUNCTION()
	void OnSliderChanged_ChargeScaleMultiplier(const float NewChargeScaleMultiplier);
	UFUNCTION()
	void OnSliderChanged_FloorDistance(const float NewFloorDistance);
	UFUNCTION()
	void OnSliderChanged_MinTargetDistance(const float NewMinTargetDistance);
	UFUNCTION()
	void OnSliderChanged_HorizontalSpread(const float NewHorizontalSpread);
	UFUNCTION()
	void OnSliderChanged_VerticalSpread(const float NewVerticalSpread);
	UFUNCTION()
	void OnSliderChanged_ForwardSpread(const float NewForwardSpread);
	UFUNCTION()
	void OnSliderChanged_MaxNumRecentTargets(const float NewMaxNum);
	UFUNCTION()
	void OnSliderChanged_RecentTargetTimeLength(const float NewTimeLength);
	UFUNCTION()
	void OnSliderChanged_NumUpfrontTargetsToSpawn(const float NewNum);
	UFUNCTION()
	void OnSliderChanged_ExpirationHealthPenalty(const float NewHealthPenalty);
	UFUNCTION()
	void OnSliderChanged_MaxHealth(const float NewMaxHealth);
	
	UFUNCTION()
	void OnTextCommitted_Lifespan(const FText& NewLifespan, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_TargetSpawnCD(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_SpawnBeatDelay(const FText& NewPlayerDelay, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_ChargeScaleMultiplier(const FText& NewChargeScaleMultiplier, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_FloorDistance(const FText& NewFloorDistance, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_MinTargetDistance(const FText& NewMinTargetDistance, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_HorizontalSpread(const FText& NewHorizontalSpread, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_VerticalSpread(const FText& NewVerticalSpread, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_ForwardSpread(const FText& NewForwardSpread, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_MaxNumRecentTargets(const FText& NewMaxNum, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_RecentTargetTimeLength(const FText& NewTimeLength, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_NumUpfrontTargetsToSpawn(const FText& NewNum, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_ExpirationHealthPenalty(const FText& NewHealthPenalty, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_MaxHealth(const FText& NewMaxHealth, ETextCommit::Type CommitType);
	
	UFUNCTION()
	void OnSelectionChanged_BoundsScalingPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_ConsecutiveTargetScalePolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_LifetimeTargetScalePolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_RecentTargetMemoryPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetActivationSelectionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDamageType(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDistributionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetSpawningPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetActivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDeactivationConditions(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDeactivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDestructionConditions(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	
	UFUNCTION()
	void OnCheckStateChanged_ApplyImmunityOnSpawn(const bool bApplyImmunityOnSpawn);
	UFUNCTION()
	void OnCheckStateChanged_MoveTargetsForward(const bool bMoveTargetsForward);
	UFUNCTION()
	void OnCheckStateChanged_ContinuouslySpawn(const bool bContinuouslySpawn);
	UFUNCTION()
	void OnCheckStateChanged_MoveTargets(const bool bMoveTargets);
	UFUNCTION()
	void OnCheckStateChanged_SpawnAtOriginWheneverPossible(const bool bSpawnAtOrigin);
	UFUNCTION()
	void OnCheckStateChanged_SpawnEveryOtherTargetInCenter(const bool bSpawnEveryOther);
	
	UFUNCTION()
	UWidget* OnGenerateWidgetEvent(const UBSComboBoxString* ComboBoxString, FString Method);
	UFUNCTION()
	UWidget* OnSelectionChanged_GenerateSingleSelectionItem(const UBSComboBoxString* ComboBoxString, FString SelectedOption);
	UFUNCTION()
	UWidget* OnSelectionChanged_GenerateMultiSelectionItem(const UBSComboBoxString* ComboBoxString, const TArray<FString>& SelectedOptions);
	
	FString GetStringTableKeyFromComboBox(const UBSComboBoxString* ComboBoxString, const FString& EnumString);

	const FText NewLineDelimit = FText::FromString("\n");
};
