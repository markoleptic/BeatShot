// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetComponent.h"
#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidgetBase.h"
#include "CustomGameModesWidget_Start.generated.h"

class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_Start : public UCustomGameModesWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext) override;

	FRequestGameModeTemplateUpdate RequestGameModeTemplateUpdate;
	
	FString GetNewCustomGameModeName() const;
	void SetNewCustomGameModeName(const FString& InCustomGameModeName) const;

protected:
	virtual void NativeConstruct() override;
	virtual bool UpdateCanTransitionForward() override;
	virtual void UpdateOptions() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_UseTemplate;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_GameModeTemplates;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_GameModeDifficulty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBoxOptionWidget* EditableTextBoxOption_CustomGameModeName;
	
	UFUNCTION()
	void OnCheckStateChanged_UseTemplate(const bool bChecked);
	UFUNCTION()
	void OnTextChanged_CustomGameModeName(const FText& Text);
	UFUNCTION()
	void OnSelectionChanged_GameModeTemplates(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_GameModeDifficulty(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
};
