// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "AASettingsWidget.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget.generated.h"

class UCrossHairSettingsWidget;
class UGameSettingsWidget;
class UVideoAndSoundSettingsWidget;
class USensitivitySettingsWidget;
class USlideRightButton;
class UVerticalBox;
class UWidgetSwitcher;


UCLASS()
class USERINTERFACE_API USettingsMenuWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	/** Whether or not this instance of SettingsMenuWidget belongs to MainMenuWidget or not */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsMainMenuChild;

	UPROPERTY()
	FOnPlayerSettingsChange OnPlayerSettingsChanged;

	UPROPERTY()
	FOnAASettingsChange OnAASettingsChanged;

	FOnRestartButtonClicked OnRestartButtonClicked;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPlayerSettingsSaved();

	UFUNCTION()
	void OnAASettingsSaved();

	FGameModeTransitionState RestartState;

	UFUNCTION()
	void OnRestartButtonClicked_AudioAnalyzer()
	{
		if (!OnRestartButtonClicked.ExecuteIfBound())
		{
			UE_LOG(LogTemp, Display, TEXT("OnRestartButtonClicked not bound."));
		}
	}

	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MenuWidgets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	/** Containers for each widget in SettingsMenu */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Game;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* VideoAndSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* AudioAnalyzer;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Sensitivity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* CrossHair;

	/* --------------- */
	/* SubMenu Widgets */
	/* --------------- */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UGameSettingsWidget* Game_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVideoAndSoundSettingsWidget* VideoAndSound_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USensitivitySettingsWidget* Sensitivity_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCrossHairSettingsWidget* CrossHair_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UAASettingsWidget* AudioAnalyzer_Widget;

	/* ------------------ */
	/* Navigation Buttons */
	/* ------------------ */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* Game_Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* VideoAndSound_Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* AudioAnalyzer_Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* Sensitivity_Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* CrossHair_Button;

	/** Function to Play the Slide Animation for Navigation Buttons */
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	UFUNCTION()
	void On_Game_ButtonClicked() { SlideButtons(Game_Button); }

	UFUNCTION()
	void On_VideoAndSound_ButtonClicked() { SlideButtons(VideoAndSound_Button); }

	UFUNCTION()
	void On_AudioAnalyzer_ButtonClicked() { SlideButtons(AudioAnalyzer_Button); }

	UFUNCTION()
	void On_Sensitivity_ButtonClicked() { SlideButtons(Sensitivity_Button); }

	UFUNCTION()
	void On_CrossHair_ButtonClicked() { SlideButtons(CrossHair_Button); }
};
