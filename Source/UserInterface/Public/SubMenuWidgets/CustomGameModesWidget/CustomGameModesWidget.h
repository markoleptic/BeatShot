// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "WidgetComponents/Buttons/BSButton.h"
#include "CustomGameModesWidget.generated.h"

class UCustomGameModesWidget_SpawnArea;
class UCustomGameModesWidgetComponent;
class UGameModePreviewWidget;
class UBSButton;
class UCustomGameModesWidget_Start;
class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;


// TODO: Reparent to UUserWidget
UCLASS()
class USERINTERFACE_API UCustomGameModesWidget : public UBSSettingCategoryWidget, public ISaveLoadInterface
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual UBSGameModeDataAsset* GetGameModeDataAsset() const override { return GameModeDataAsset.Get(); }
	
	UFUNCTION()
	void OnBSButtonPressed(const UBSButton* BSButton);

	/** Checks the CurrentWidget's Can Transition Forward State and if allowed,
	 *  transitions in the Next widget and assigns it to the value of CurrentWidget */
	void TransitionForward();
	
	void TransitionBackward();

	void OnCanTransitionForwardStateChanged(const TObjectPtr<UCustomGameModesWidgetComponent> Widget, const bool bCanTransition);
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Game Modes")
	TObjectPtr<UBSGameModeDataAsset> GameModeDataAsset;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UGameModePreviewWidget* Widget_Preview;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomGameModesWidget_Start* Widget_Start;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomGameModesWidget_SpawnArea* Widget_SpawnArea;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Next;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Previous;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Create;
	
	UPROPERTY()
	TObjectPtr<UCustomGameModesWidgetComponent> CurrentWidget;

	UPROPERTY(VisibleAnywhere)
	FBSConfig CurrentConfig;

	FBSConfig* CurrentConfigPtr;

	TMap<TObjectPtr<UCustomGameModesWidgetComponent>, bool> TransitionMap;
};
