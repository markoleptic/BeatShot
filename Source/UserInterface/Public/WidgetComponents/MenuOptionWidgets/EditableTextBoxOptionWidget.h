// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "EditableTextBoxOptionWidget.generated.h"

UCLASS()
class USERINTERFACE_API UEditableTextBoxOptionWidget : public UMenuOptionWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* EditableTextBox;
	
protected:
	virtual void SetStyling() override;
};
