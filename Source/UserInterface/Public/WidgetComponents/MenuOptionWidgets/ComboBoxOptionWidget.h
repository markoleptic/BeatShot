// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSWidgetInterface.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "ComboBoxOptionWidget.generated.h"

class UBSComboBoxString;

UCLASS()
class USERINTERFACE_API UComboBoxOptionWidget : public UMenuOptionWidget, public IBSWidgetInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSComboBoxString* ComboBox;
	
protected:
	virtual void NativeConstruct() override;
	virtual UTooltipWidget* IBSWidgetInterface::ConstructTooltipWidget() override { return nullptr; }
	virtual UBSComboBoxEntry* ConstructComboBoxEntryWidget() override;

	UFUNCTION()
	void OnSelectionChanged_ComboBox(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
};
