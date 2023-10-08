// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "CheckBoxOptionWidget.generated.h"

UCLASS()
class USERINTERFACE_API UCheckBoxOptionWidget : public UMenuOptionWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnCheckStateChanged_CheckBox(const bool bChecked);
};
