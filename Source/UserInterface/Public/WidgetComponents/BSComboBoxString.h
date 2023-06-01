// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBoxString.h"
#include "BSComboBoxString.generated.h"

class UBSComboBoxEntry;

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(UWidget*, FGenerateWidgetForStringWithSelf, const UBSComboBoxString*, BSComboBoxString, FString, Item);

UCLASS()
class USERINTERFACE_API UBSComboBoxString : public UComboBoxString
{
	GENERATED_BODY()
	
	virtual void HandleSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectionType) override;
	virtual TSharedRef<SWidget> HandleGenerateWidget(TSharedPtr<FString> Item) const override;
	virtual TSharedRef<SWidget> HandleSelectionChangedGenerateWidget(TSharedPtr<FString> Item) const;

public:
	TArray<int32> GetSelectedIndices() const;
	TArray<FString> GetSelectedOptions() const;
	void SetSelectedIndices(const TArray<int32> InIndices);
	void SetSelectedOptions(TArray<FString> InOptions);
	void AddSelectedIndex(const int32 InIndex);
	void AddSelectedOption(const FString InOption);
	
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FGenerateWidgetForStringWithSelf OnGenerateWidgetEventDelegate;
	
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FGenerateWidgetForStringWithSelf OnSelectionChangedGenerateWidgetEventDelegate;

	/** Used to not show the tooltip image for the selected item widget */
	mutable bool bHideTooltipImage;

	TArray<FString> SelectedOptions;
	TArray<TSharedPtr<FString>> CurrentOptionPointers;
	mutable UBSComboBoxString* NonConstSelfRef;
};
