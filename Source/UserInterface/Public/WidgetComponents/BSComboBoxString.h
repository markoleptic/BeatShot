// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

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
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(UWidget*, FGenerateWidgetForStringWithSelf, const UBSComboBoxString*, BSComboBoxString, FString, Item);

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content)
	int32 MaxNumberOfSelections = -1;
	
	/** The max height of the combobox list that opens */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content, AdvancedDisplay)
	float MaxListHeight;

	/** When false, the down arrow is not generated and it is up to the API consumer
	 *  to make their own visual hint that this is a drop down. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content, AdvancedDisplay)
	bool HasDownArrow;

	/** When false, directional keys will change the selection. When true, ComboBox 
	 *  must be activated and will only capture arrow input while activated. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content, AdvancedDisplay)
	bool EnableGamepadNavigationMode;

	/** The default font to use in the combobox, only applies if you're not implementing OnGenerateWidgetEvent
	 *  to factory each new entry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Style)
	FSlateFontInfo Font;

	/** The foreground color to pass through the hierarchy. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Style, meta=(DesignerRebuild))
	FSlateColor ForegroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Interaction)
	bool bIsFocusable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Content)
	bool bCloseComboBoxOnSelectionChanged = false;
	
	/** Called when the widget is needed for the item. */
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FGenerateWidgetForString OnGenerateWidgetEvent;

	/** Called when a new item is selected in the combobox. */
	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnSelectionChangedEvent OnSelectionChanged;

	/** Called when the combobox is opening */
	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnOpeningEvent OnOpening;

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void AddOption(const FString& Option);

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	bool RemoveOption(const FString& Option);

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	int32 FindOptionIndex(const FString& Option) const;

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	FString GetOptionAtIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	FString GetFirstSelectedOption() const;

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	int32 GetFirstSelectedIndex() const;

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	TArray<int32> GetSelectedIndices() const;

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	TArray<FString> GetSelectedOptions() const;

	/** Returns the number of options */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	int32 GetOptionCount() const;

	/** Returns the number of options */
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	int32 GetSelectionCount() const;

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void ToggleSelectedIndex(const int32 InIndex);

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void ToggleSelectedOption(const FString InOption);
	
	UFUNCTION(BlueprintCallable, Category = "ComboBox")
	void SetSelectedIndices(const TArray<int32> InIndices);
	
	UFUNCTION(BlueprintCallable, Category = "ComboBox")
	void SetSelectedOptions(TArray<FString> InOptions);

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void ClearOptions();

	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void ClearSelection();

	void SetSelectionMode(const ESelectionModeType InSelectionMode) { SelectionMode = InSelectionMode; }
	void SetMaxNumberOfSelections(const int32 InMaxNumberOfSelections) { MaxNumberOfSelections = InMaxNumberOfSelections; }

	/**
	 * Refreshes the list of options.  If you added new ones, and want to update the list even if it's
	 * currently being displayed use this.
	 */
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

	/** Called by slate when the underlying combobox selection changes */
	virtual void HandleSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectionType);
	virtual void HandleMultiSelectionChanged(const TArray<TSharedPtr<FString>>& Items);

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
	
	virtual TSharedRef<SWidget> HandleSelectionChangedGenerateWidget(TSharedPtr<FString> Item) const;

	virtual TSharedRef<SWidget> HandleMultiSelectionChangedGenerateWidget(TConstArrayView<SBSComboBox<TSharedPtr<FString>>::NullableOptionType> Items) const;

public:
	
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FGenerateWidgetForStringWithSelf OnGenerateWidgetEventDelegate;
	
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FGenerateWidgetForStringWithSelf OnSelectionChangedGenerateWidgetEventDelegate;
	
	mutable TArray<TObjectPtr<UBSComboBoxEntry>> OptionWidgets;

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
