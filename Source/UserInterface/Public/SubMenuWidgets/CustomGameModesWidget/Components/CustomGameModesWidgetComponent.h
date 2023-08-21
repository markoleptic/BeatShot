// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "CustomGameModesWidgetComponent.generated.h"

class UCustomGameModesWidgetComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCanTransitionForwardStateChanged, const TObjectPtr<UCustomGameModesWidgetComponent>, const bool);
DECLARE_MULTICAST_DELEGATE(FRequestUpdateAfterConfigChange);

UCLASS(Abstract)
class USERINTERFACE_API UCustomGameModesWidgetComponent : public UBSSettingCategoryWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	/** Initializes options, sets the pointer to next widget in chain */
	virtual void InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext);

	/** Child classes should override to update the options using the ConfigPtr */
	virtual void UpdateOptions();

	/** Returns the next CustomGameModesWidgetComponent to transition to */
	TObjectPtr<UCustomGameModesWidgetComponent> GetNext() const;

	/** Broadcast when the user should or shouldn't be able to proceed to the next step */
	FOnCanTransitionForwardStateChanged OnCanTransitionForwardStateChanged;

	/** Broadcast when the ConfigPtr needed to be changed */
	FRequestUpdateAfterConfigChange RequestUpdateAfterConfigChange;

	/** Whether or not all options are valid and the user can proceed to the next step */
	bool CanTransitionForward() const { return bCanTransitionForward; }
	
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
	
	/** Child classes should override and call this anytime an option is changed inside a call to SetCanTransitionForward */
	virtual bool UpdateCanTransitionForward() { return CanTransitionForward(); }

	/** Broadcasts the OnCanTransitionForwardStateChanged delegate if the value of bCanTransitionForward has changed.
	 *  Child classes should override and call this anytime an option is changed */
	void SetCanTransitionForward(const bool bInCanTransitionForward);

	/** Sets value of Next */
	void SetNext(const TObjectPtr<UCustomGameModesWidgetComponent> InNext);

	UFUNCTION()
	void SetCollapsed();

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* TransitionInRight;
	
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* TransitionInLeft;

	FWidgetAnimationDynamicEvent OnTransitionInRightFinish;
	FWidgetAnimationDynamicEvent OnTransitionInLeftFinish;
	
	FBSConfig* ConfigPtr;

	UPROPERTY()
	TObjectPtr<UCustomGameModesWidgetComponent> Next;

	bool bCanTransitionForward = false;
};
