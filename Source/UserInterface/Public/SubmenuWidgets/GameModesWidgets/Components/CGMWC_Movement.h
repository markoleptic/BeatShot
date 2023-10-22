// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CGMWC_Base.h"
#include "CGMWC_Movement.generated.h"

UCLASS()
class USERINTERFACE_API UCGMWC_Movement : public UCGMWC_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	void SetupWarningTooltipCallbacks();

	/** Updates options that depend on TargetSpawnResponses (and ConstantSpawnedTargetVelocity) */
	void UpdateDependentOptions_SpawnResponses(const TArray<ETargetSpawnResponse>& Responses, const bool bConstant);

	/** Updates options that depend on TargetActivationResponses (and ConstantActivationTargetVelocity) */
	void UpdateDependentOptions_ActivationResponses(const TArray<ETargetActivationResponse>& Responses,
		const bool bConstant);

	/** Updates options that depend on TargetDeactivationResponses (and ConstantDeactivatedTargetVelocity) */
	void UpdateDependentOptions_DeactivationResponses(const TArray<ETargetDeactivationResponse>& Responses,
		const bool bConstant);

	UFUNCTION()
	void OnSelectionChanged_MovingTargetDirectionMode(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);

	void OnMinMaxMenuOptionChanged(UConstantMinMaxMenuOptionWidget* Widget,
		const bool bChecked, const float MinOrConstant, const float Max);
	
	FString GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode(const FString& EnumString);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UConstantMinMaxMenuOptionWidget* MenuOption_ActivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UConstantMinMaxMenuOptionWidget* MenuOption_DeactivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UConstantMinMaxMenuOptionWidget* MenuOption_SpawnedTargetVelocity;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_MovingTargetDirectionMode;
};
