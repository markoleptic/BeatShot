// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "GameModesWidget_AIConfig.generated.h"

class UBSHorizontalBox;
class UTooltipImage;
class UEditableTextBox;
class USlider;
class UCheckBox;
class UHorizontalBox;

/** SettingCategoryWidget for the GameModesWidget that holds AI configuration settings */
UCLASS()
class USERINTERFACE_API UGameModesWidget_AIConfig : public UBSSettingCategoryWidget
{
	GENERATED_BODY()

protected:
	
	friend class UGameModesWidget;
	virtual void NativeConstruct() override;
	virtual void InitSettingCategoryWidget() override;

public:
	void InitializeAIConfig(const FBS_AIConfig& InAIConfig, const EDefaultMode& BaseGameMode);
	FBS_AIConfig GetAIConfig() const;
	
protected:	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UBSHorizontalBox* BSBox_EnableAI;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UBSHorizontalBox* BSBox_AIAlpha;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UBSHorizontalBox* BSBox_AIEpsilon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UBSHorizontalBox* BSBox_AIGamma;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UCheckBox* CheckBox_EnableAI;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	USlider* Slider_AIAlpha;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	USlider* Slider_AIEpsilon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	USlider* Slider_AIGamma;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UEditableTextBox* Value_AIAlpha;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UEditableTextBox* Value_AIEpsilon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UEditableTextBox* Value_AIGamma;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_EnableAI;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_Alpha;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_Epsilon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_Gamma;

	UFUNCTION()
	void OnCheckStateChanged_EnableAI(const bool bEnableAI);
	UFUNCTION()
	void OnSliderChanged_AIAlpha(const float NewAlpha);
	UFUNCTION()
	void OnTextCommitted_AIAlpha(const FText& NewAlpha, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_AIEpsilon(const float NewEpsilon);
	UFUNCTION()
	void OnTextCommitted_AIEpsilon(const FText& NewEpsilon, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_AIGamma(const float NewGamma);
	UFUNCTION()
	void OnTextCommitted_AIGamma(const FText& NewGamma, ETextCommit::Type CommitType);
};
