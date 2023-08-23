// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
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

public:
	/** Sets BSConfig, sets the pointer to next widget in linked list, and calls UpdateOptionsFromConfig */
	virtual void InitComponent(FBSConfig* InConfigPtr, const TObjectPtr<UCustomGameModesWidgetComponent> InNext);

	/** Sets all custom game mode option values using the BSConfig pointer. Only changes the values if different */
	virtual void UpdateOptionsFromConfig();

	/** Returns the next CustomGameModesWidgetComponent to transition to */
	TObjectPtr<UCustomGameModesWidgetComponent> GetNext() const;

	/** Broadcasts whether or not all custom game mode options are valid for this widget */
	FOnValidOptionsStateChanged OnValidOptionsStateChanged;

	/** Broadcast when the BSConfig was modified by this widget */
	FRequestComponentUpdate RequestComponentUpdate;

	/** Returns the value of bAllOptionsValid, whether or not all custom game mode options are valid for this widget */
	bool GetAllOptionsValid() const;

	bool IsInitialized() const { return bIsInitialized; }
	
	/** Starts with the widget not visible and shifts to the right from the left */
	void PlayAnim_TransitionInLeft_Forward(const bool bCollapseOnFinish);
	/** Starts with the widget visible and shifts to the left from the right */
	void PlayAnim_TransitionInLeft_Reverse(const bool bCollapseOnFinish);
	/** Starts with the widget not visible and shifts to the left from the right */
	void PlayAnim_TransitionInRight_Forward(const bool bCollapseOnFinish);
	/** Starts with the widget visible and shifts to the right from the left */
	void PlayAnim_TransitionInRight_Reverse(const bool bCollapseOnFinish);

protected:
	virtual void NativeConstruct() override;
	
	/** Checks all custom game mode options for validity, returning true if valid and false if any are invalid. Should be called anytime an option is changed */
	virtual bool UpdateAllOptionsValid();

	/** Sets the value bAllOptionsValid. Broadcasts the OnValidOptionsStateChanged delegate if it was changed */
	void SetAllOptionsValid(const bool bUpdateAllOptionsValid);

	/** Sets value of Next */
	void SetNext(const TObjectPtr<UCustomGameModesWidgetComponent> InNext);

	/** Sets the visibility of the widget to collapsed and unbinds from any OnAnimationFinished delegates */
	UFUNCTION()
	void SetCollapsed();

	static bool UpdateValueIfDifferent(const USliderTextBoxWidget* Widget, const float Value);
	static bool UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget, const FString& NewOption);
	static bool UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget, const TArray<FString>& NewOptions);
	static bool UpdateValueIfDifferent(const UCheckBoxOptionWidget* Widget, const bool bIsChecked);
	static bool UpdateValueIfDifferent(const UEditableTextBoxOptionWidget* Widget, const FText& NewText);

	/** Updates TooltipWarningImages for a MenuOptionWidget. Returns true if a component update is needed */
	bool UpdateTooltipWarningImages(UMenuOptionWidget* Widget, const TArray<FString>& NewKeys);

	/** Returns an array of keys for use with UpdateTooltipWarningImages based on invalid settings */
	virtual TArray<FString> GetWarningTooltipKeys();

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* TransitionInRight;
	
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* TransitionInLeft;

	FWidgetAnimationDynamicEvent OnTransitionInRightFinish;
	FWidgetAnimationDynamicEvent OnTransitionInLeftFinish;

	/** Pointer to the game mode config inside GameModesWidget */
	FBSConfig* BSConfig;

	/** Pointer to next widget in linked list. Used for CreatorView */
	UPROPERTY()
	TObjectPtr<UCustomGameModesWidgetComponent> Next;

	/** Whether or not all custom game mode options are valid for this widget */
	bool bAllOptionsValid = false;

	bool bIsInitialized = false;
};
