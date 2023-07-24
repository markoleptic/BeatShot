// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerMappableInputConfig.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "SettingsMenuWidget_Input.generated.h"

class UInputMappingWidget;
UCLASS()
class USERINTERFACE_API USettingsMenuWidget_Input : public UBSSettingCategoryWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	friend class USettingsMenuWidget;

public:
	/** Returns OnPlayerSettingsChangedDelegate_User, the delegate that is broadcast when this class saves User settings */
	FOnPlayerSettingsChanged_User& GetPublicUserSettingsChangedDelegate() { return OnPlayerSettingsChangedDelegate_User; }
	
	virtual void NativeConstruct() override;

	/** Populates the settings menu */
	void InitializeInputSettings();

protected:

	void OnKeySelected(const FName KeyName, const FInputChord SelectedKey);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInputMappingWidget> InputMappingWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	UPlayerMappableInputConfig* PlayerMappableInputConfig;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* BSBox_Combat;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* BSBox_Movement;
};
