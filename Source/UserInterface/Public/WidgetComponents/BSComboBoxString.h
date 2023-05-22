// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBoxString.h"
#include "BSComboBoxString.generated.h"

class UBSComboBoxEntry;

UCLASS()
class USERINTERFACE_API UBSComboBoxString : public UComboBoxString
{
	GENERATED_BODY()
	
	virtual void HandleSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectionType) override;
	virtual TSharedRef<SWidget> HandleGenerateWidget(TSharedPtr<FString> Item) const override;
	virtual TSharedRef<SWidget> HandleSelectionChangedGenerateWidget(TSharedPtr<FString> Item) const;

public:
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FGenerateWidgetForString OnSelectionChangedGenerateWidgetEvent;
};
