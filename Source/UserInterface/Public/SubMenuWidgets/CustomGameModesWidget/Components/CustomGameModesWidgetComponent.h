// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "CustomGameModesWidgetComponent.generated.h"

class UMenuOptionWidget;
class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;
class USliderTextBoxWidget;
class UCustomGameModesWidgetComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnValidOptionsStateChanged, const TObjectPtr<UCustomGameModesWidgetComponent>, const bool);
DECLARE_MULTICAST_DELEGATE(FRequestComponentUpdate);

/** Base class for child widgets of UCustomGameModesWidgetBase */
UCLASS(Abstract)
class USERINTERFACE_API UCustomGameModesWidgetComponent : public UBSSettingCategoryWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeDestruct() override;

public:
	/** Sets BSConfig, sets the pointer to next widget in linked list, and calls UpdateOptionsFromConfig */
	virtual void InitComponent(FBSConfig* InConfigPtr, const TObjectPtr<UCustomGameModesWidgetComponent> InNext);

	/** Sets all custom game mode option values using the BSConfig pointer. Only changes the values if different. Only called during transitions */
	virtual void UpdateOptionsFromConfig();

	/** Returns the next CustomGameModesWidgetComponent to transition to */
	TObjectPtr<UCustomGameModesWidgetComponent> GetNext() const;

	/** Broadcasts whether or not all custom game mode options are valid for this widget */
	FOnValidOptionsStateChanged OnValidOptionsStateChanged;

	/** Broadcast when the BSConfig was modified by this widget */
	FRequestComponentUpdate RequestComponentUpdate;

	/** Returns the value of bAllOptionsValid, whether or not all custom game mode options are valid for this widget */
	bool GetAllOptionsValid() const;

	/** Returns whether or not Init has been called */
	bool IsInitialized() const { return bIsInitialized; }

	/** Checks all custom game mode options for validity, returning true if valid and false if any are invalid. Should be called anytime an option is changed */
	virtual bool UpdateAllOptionsValid();

	/** Sets the value bAllOptionsValid. Broadcasts the OnValidOptionsStateChanged delegate if it was changed */
	void SetAllOptionsValid(const bool bUpdateAllOptionsValid);

protected:
	virtual void NativeConstruct() override;
	
	/** Sets value of Next */
	void SetNext(const TObjectPtr<UCustomGameModesWidgetComponent> InNext);

	static bool UpdateValueIfDifferent(const USliderTextBoxWidget* Widget, const float Value);
	static bool UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget, const FString& NewOption);
	static bool UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget, const TArray<FString>& NewOptions);
	static bool UpdateValueIfDifferent(const UCheckBoxOptionWidget* Widget, const bool bIsChecked);
	static bool UpdateValueIfDifferent(const UEditableTextBoxOptionWidget* Widget, const FText& NewText);

	/** Updates TooltipWarningImages for a MenuOptionWidget. Returns true if a component update is needed */
	bool UpdateWarningTooltips(UMenuOptionWidget* Widget, const TArray<FTooltipData>& NewValues);

	/** Pointer to the game mode config inside GameModesWidget */
	FBSConfig* BSConfig;

	/** Pointer to next widget in linked list. Used for CreatorView */
	UPROPERTY()
	TObjectPtr<UCustomGameModesWidgetComponent> Next;

	/** Whether or not all custom game mode options are valid for this widget */
	bool bAllOptionsValid = false;

	/** Whether or not Init has been called */
	bool bIsInitialized = false;
};

