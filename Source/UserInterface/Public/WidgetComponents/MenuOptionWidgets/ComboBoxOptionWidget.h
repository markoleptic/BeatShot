// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSWidgetInterface.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "ComboBoxOptionWidget.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(FString, FGetComboBoxEntryTooltipStringTableKey, const FString& EnumString);

class UBSComboBoxString;

UCLASS()
class USERINTERFACE_API UComboBoxOptionWidget : public UMenuOptionWidget, public IBSWidgetInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBSComboBoxString* ComboBox;

	/** Executed when a ComboBoxEntry requests a tooltip description. If an empty string is returned, no tooltip image is shown */
	FGetComboBoxEntryTooltipStringTableKey GetComboBoxEntryTooltipStringTableKey;
	
protected:
	virtual void NativeConstruct() override;
	virtual UTooltipWidget* IBSWidgetInterface::ConstructTooltipWidget() override { return nullptr; }
	virtual UBSComboBoxEntry* ConstructComboBoxEntryWidget() override;
	virtual FString GetStringTableKeyFromComboBox(const UBSComboBoxString* ComboBoxString, const FString& EnumString) override;

	UFUNCTION()
	void OnSelectionChanged_ComboBox(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
};
