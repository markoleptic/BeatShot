﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SBSComboBox.h"
#include "UObject/ObjectMacros.h"
#include "Fonts/SlateFontInfo.h"
#include "Layout/Margin.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SWidget.h"
#include "Widgets/Input/SComboBox.h"
#include "Components/Widget.h"
#include "BSComboBoxString.generated.h"

UENUM(BlueprintType)
enum class ESelectionModeType : uint8
{
	/** Nothing can be selected and there is no hover cue for selection.  You can still handle mouse button events though. */
	None,

	/** A single item can be selected at once, or no item may be selected. */
	Single,

	/** A single item can be selected at once, or no item may be selected.  You can click the item to toggle selection on and off. */
	SingleToggle,

	/** Multiple items can be selected at the same time. */
	Multi
};

class UBSComboBoxEntry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectionChangedEvent, FString, SelectedItem, ESelectInfo::Type, SelectionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpeningEvent);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(UWidget*, FGenerateWidgetForSingleItem, const UBSComboBoxString*, BSComboBoxString, FString, Item);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(UWidget*, FGenerateWidgetForMultiSelection, const UBSComboBoxString*, BSComboBoxString, const TArray<FString>&, Items);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMultiSelectionChangedEvent, const TArray<FString>&, ActiveSelections, const ESelectInfo::Type, SelectionType);

UCLASS(meta=( DisplayName="BSComboBox (String)"))
class USERINTERFACE_API UBSComboBoxString : public UWidget
{
	GENERATED_BODY()

	UBSComboBoxString();

	/** The default list of items to be displayed on the combobox. */
	UPROPERTY(EditAnywhere, Category=Content)
	TArray<FString> DefaultOptions;

	/** The item in the combobox to select by default */
	UPROPERTY(EditAnywhere, Category=Content)
	FString SelectedOption;

public:

	/** The style. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Style, meta=( DisplayName="Style" ))
	FComboBoxStyle WidgetStyle;

	/** The item row style. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Style)
	FTableRowStyle ItemStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content)
	FMargin ContentPadding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content)
	ESelectionModeType SelectionMode = ESelectionModeType::Multi;

	/** Should the user be able to select no options */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content)
	bool bCanSelectNone = false;
	
	/** Should the combo box automatically close when a selection is made */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content)
	bool bCloseComboBoxOnSelectionChanged = false;

	/** The max number of options that a user can select */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content)
	int32 MaxNumberOfSelections = -1;

	/** When false, directional keys will change the selection. When true, ComboBox 
	 *  must be activated and will only capture arrow input while activated. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content, AdvancedDisplay)
	bool EnableGamepadNavigationMode;
	
	/** The max height of the combobox list that opens */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content, AdvancedDisplay)
	float MaxListHeight;

	/** When false, the down arrow is not generated and it is up to the API consumer
	 *  to make their own visual hint that this is a drop down. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content, AdvancedDisplay)
	bool HasDownArrow;

	/** The default font to use in the combobox, only applies if you're not implementing OnGenerateWidgetEvent
	 *  to factory each new entry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Style)
	FSlateFontInfo Font;

	/** The foreground color to pass through the hierarchy. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Style, meta=(DesignerRebuild))
	FSlateColor ForegroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Interaction)
	bool bIsFocusable;
	
	/** Called when the widget is needed for the item. */
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FGenerateWidgetForString OnGenerateWidgetEvent;

	/** Called when ayn change in selection occurs in the combobox. */
	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnMultiSelectionChangedEvent OnSelectionChanged;

	/** Called when the combobox is opening */
	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnOpeningEvent OnOpening;

	/** Adds a new option to the combo box */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void AddOption(const FString& Option);

	/** Removes existing option from the combo box */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	bool RemoveOption(const FString& Option);

	/** Returns the index corresponding to the Option, or -1 if not found */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	int32 FindOptionIndex(const FString& Option) const;

	/** Returns string option corresponding to the Index, or empty string if not found */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	FString GetOptionAtIndex(int32 Index) const;

	/** Returns the selected option. Should only use if SelectionMode is Single, or MaxNumSelectedItems = 1 */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	FString GetSelectedOption() const;

	/** Returns the selected index. Should only use if SelectionMode is Single, or MaxNumSelectedItems = 1 */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	int32 GetSelectedIndex() const;

	/** Returns an array of selected indices */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	TArray<int32> GetSelectedIndices() const;

	/** Returns an array of selected string options */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	TArray<FString> GetSelectedOptions() const;

	/** Returns the number of options */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	int32 GetOptionCount() const;

	/** Returns the number of currently selected options */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	int32 GetSelectedOptionCount() const;

	/** Selects the specified index if it exists */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void SetSelectedIndex(const int32 InIndex);

	/** Selects the specified option if it exists */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void SetSelectedOption(const FString InOption);

	/** Selects the specified indices if they exists */
	UFUNCTION(BlueprintCallable, Category = "ComboBox")
	void SetSelectedIndices(const TArray<int32> InIndices);

	/** Selects the specified options if they exists */
	UFUNCTION(BlueprintCallable, Category = "ComboBox")
	void SetSelectedOptions(TArray<FString> InOptions);

	/** Clears all options */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void ClearOptions();

	/** Clears all selected options */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void ClearSelection();

	/** Refreshes the list of options.  If you added new ones, and want to update the list even if it's currently being displayed use this. */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void RefreshOptions();
	
	UFUNCTION(BlueprintCallable, Category="ComboBox", Meta = (ReturnDisplayName = "bOpen"))
	bool IsOpen() const;

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	//~ Begin UObject Interface
	virtual void PostInitProperties() override;
	virtual void Serialize(FArchive& Ar) override;
	virtual void PostLoad() override;
	//~ End UObject Interface

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	/** Refresh ComboBoxContent with the correct widget/data when the selected option changes */
	void UpdateOrGenerateWidget(TSharedPtr<FString> Item);

	/** Called by slate when it needs to generate a new item for the combobox */
	virtual TSharedRef<SWidget> HandleGenerateWidget(TSharedPtr<FString> Item) const;

	/** Called by slate when the underlying combobox selection changes. Handles both single select and multi-select */
	virtual void HandleSelectionChanged(const TArray<TSharedPtr<FString>>& Items, const ESelectInfo::Type SelectionType);

	/** Called by slate when the underlying combobox is opening */
	virtual void HandleOpening();
	
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

	/** The true objects bound to the Slate combobox. */
	TArray< TSharedPtr<FString> > Options;

	/** A shared pointer to the underlying slate combobox */
	TSharedPtr< SBSComboBox< TSharedPtr<FString> > > MyComboBox;

	/** A shared pointer to a container that holds the combobox content that is selected */
	TSharedPtr< SBox > ComboBoxContent;

	/** If OnGenerateWidgetEvent is not bound, this will store the default STextBlock generated */
	TWeakPtr<STextBlock> DefaultComboBoxContent;

	/** An array of shared pointers to the current selected strings */
	TArray<TSharedPtr<FString>> CurrentlySelectedOptionPointers;

	/** Generates a widget for a row inside of the combo box */
	/*virtual TSharedRef<SWidget> HandleSelectionChangedGenerateWidget(TSharedPtr<FString> Item) const;*/

	/** Generates a widget for the selected item (top) of the combo box */
	virtual TSharedRef<SWidget> HandleMultiSelectionChangedGenerateWidget(TConstArrayView<SBSComboBox<TSharedPtr<FString>>::NullableOptionType> Items) const;

public:
	/** Executed to allow other widgets to create a combo box row */
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FGenerateWidgetForSingleItem OnGenerateWidgetEventDelegate;

	/** Executed to allow other widgets to create selected (top) combo box row */
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FGenerateWidgetForSingleItem OnSelectionChanged_GenerateWidgetForSingleSelection;

	/** Executed to allow other widgets to create selected (top) combo box row */
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FGenerateWidgetForMultiSelection OnSelectionChanged_GenerateWidgetForMultiSelection;

	static TAttribute<ESelectionMode::Type> GetSelectionModeType(const ESelectionModeType& SelectionModeType)
	{
		switch (SelectionModeType) {
		case ESelectionModeType::None:
			return ESelectionMode::Type::None;
		case ESelectionModeType::Single:
			return ESelectionMode::Type::Single;
		case ESelectionModeType::SingleToggle:
			return ESelectionMode::Type::SingleToggle;
		case ESelectionModeType::Multi:
			return ESelectionMode::Type::Multi;
		}
		return ESelectionMode::Type::None;
	}
};
