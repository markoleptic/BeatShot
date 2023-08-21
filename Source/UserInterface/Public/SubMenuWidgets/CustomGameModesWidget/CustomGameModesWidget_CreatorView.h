// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetBase.h"
#include "Components/CustomGameModesWidget_Start.h"
#include "WidgetComponents/Buttons/BSButton.h"
#include "CustomGameModesWidget_CreatorView.generated.h"

class UCustomGameModesWidget_Target;
class UCustomGameModesWidget_Spawning;
class UCustomGameModesWidget_General;
class UCustomGameModesWidget_Deactivation;
class UCustomGameModesWidget_Activation;
class UCustomGameModesWidget_SpawnArea;
class UCustomGameModesWidgetComponent;
class UCustomGameModesWidget_Preview;
class UBSButton;
class UCustomGameModesWidget_Start;
class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_CreatorView : public UCustomGameModesWidgetBase
{
	GENERATED_BODY()

public:
	virtual void Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset) override;
	
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnBSButtonPressed(const UBSButton* BSButton);
	
	/** Checks the CurrentWidget's Can Transition Forward State and if allowed, transitions in the Next widget and assigns it to the value of CurrentWidget */
	void TransitionForward();

	/** Transitions in the previous widget and assigns it to the value of CurrentWidget */
	void TransitionBackward();

	/** Changes the value of CurrentWidget and plays the appropriate widget animation */
	void ChangeCurrentWidget(const TObjectPtr<UCustomGameModesWidgetComponent> Widget);

	/** Bound to all child widget's OnValidOptionsStateChanged delegates */
	virtual void OnValidOptionsStateChanged(const TObjectPtr<UCustomGameModesWidgetComponent> Widget, const bool bAllOptionsValid) override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomGameModesWidget_Preview* Widget_Preview;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Next;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Previous;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Create;
	
	UPROPERTY()
	TObjectPtr<UCustomGameModesWidgetComponent> CurrentWidget;
	UPROPERTY()
	TObjectPtr<UCustomGameModesWidgetComponent> FirstWidget;
	UPROPERTY()
	TObjectPtr<UCustomGameModesWidgetComponent> LastWidget;

	TMap<TObjectPtr<UCustomGameModesWidgetComponent>, bool> TransitionMap;
};
