// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "WidgetComponents/VideoSettingButton.h"
#include "VideoAndSoundSettingsWidget.generated.h"

class USettingsMenu_Sound;
class USettingsMenu_Video;
class UPopupMessageWidget;
class USavedTextWidget;
class UVerticalBox;
class UButton;

UCLASS()
class USERINTERFACE_API UVideoAndSoundSettingsWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	/** Returns OnPlayerSettingsChangedDelegate_VideoAndSound, the delegate that is broadcast when this class saves Video and Sound settings */
	FOnPlayerSettingsChanged_VideoAndSound& GetPublicVideoAndSoundSettingsChangedDelegate() {return OnPlayerSettingsChangedDelegate_VideoAndSound;}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | PopUp")
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Video Settings")
	TSubclassOf<USettingsMenu_Video> VideoSettingsClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Sound Settings")
	TSubclassOf<USettingsMenu_Sound> SoundSettingsClass;

	TSoftObjectPtr<USettingsMenu_Video> VideoSettings;
	TSoftObjectPtr<USettingsMenu_Sound> SoundSettings;

	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* SaveButton_VideoAndSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* ResetButton_VideoAndSound;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* VideoSettingsBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SoundSettingsBox;

	/** Adds the ConfirmVideoSettingsMessage to viewport, and starts the RevertVideoSettingsTimer */
	UFUNCTION()
	void ShowConfirmVideoSettingsMessage();

	/** Stops the RevertVideoSettingsTimer and applies the video settings and closes the ConfirmVideoSettingsMessage */
	UFUNCTION()
	void OnConfirmVideoSettingsButtonClicked();

	/** Reverts the video settings and closes the ConfirmVideoSettingsMessage */
	UFUNCTION()
	void OnCancelVideoSettingsButtonClicked();

	/** Saves the Video and Sound Settings */
	UFUNCTION()
	void OnSaveButtonClicked_VideoAndSound();

	/** Reset Video and Sound Settings to defaults and repopulate in Settings Menu. Does not automatically save */
	UFUNCTION()
	void OnResetButtonClicked_VideoAndSound();
	
	/** Timer that starts when window mode or resolution is changed. If it expires, it reverts those changes */
	UPROPERTY()
	FTimerHandle RevertVideoSettingsTimer;
};
