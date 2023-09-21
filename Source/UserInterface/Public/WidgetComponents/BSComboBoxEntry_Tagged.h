// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSComboBoxEntry.h"
#include "BSComboBoxEntry_Tagged.generated.h"


class UHorizontalBox;

UCLASS()
class USERINTERFACE_API UBSComboBoxEntry_Tagged : public UBSComboBoxEntry
{
	GENERATED_BODY()

public:
	/** Adds the widget to Box_TagWidgets */
	void AddGameModeCategoryTagWidget(UUserWidget* InGameModeCategoryTagWidget);

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UHorizontalBox* Box_TagWidgets;

	UPROPERTY(EditDefaultsOnly, Category="BSComboBoxEntry_Tagged")
	FMargin Padding_TagWidget;
	
	UPROPERTY(EditDefaultsOnly, Category="BSComboBoxEntry_Tagged")
	TEnumAsByte<EVerticalAlignment>  VerticalAlignment_TagWidget;

	UPROPERTY(EditDefaultsOnly, Category="BSComboBoxEntry_Tagged")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment_TagWidget;
};
