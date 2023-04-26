// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "SettingsMenuWidget_AudioAnalyzer.h"
#include "SettingsMenuWidget_CrossHair.h"
#include "SettingsMenuWidget_Game.h"
#include "SettingsMenuWidget_VideoAndSound.h"
#include "SettingsMenuWidget_Sensitivity.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget.generated.h"

class USlideRightButton;
class UVerticalBox;
class UWidgetSwitcher;

UCLASS()
class USERINTERFACE_API USettingsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Whether or not this instance of SettingsMenuWidget belongs to MainMenuWidget or not */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsMainMenuChild;

	/** Broadcast to owning widget that user has clicked restart button */
	FOnRestartButtonClicked OnRestartButtonClicked;

	/** Returns the UGameSettings widget's OnPlayerSettingsChangedDelegate_Game, which is broadcast when the widget changes Game Settings */
	FOnPlayerSettingsChanged_Game& GetGameDelegate() const { return Game_Widget->GetPublicGameSettingsChangedDelegate(); }

	/** Returns the UCrossHairSettings widget's OnPlayerSettingsChangedDelegate_Game, which is broadcast when the widget changes Game Settings */
	FOnPlayerSettingsChanged_CrossHair& GetCrossHairDelegate() const { return CrossHair_Widget->GetPublicCrossHairSettingsChangedDelegate(); }

	/** Returns the UVideoAndSoundSettings widget's OnPlayerSettingsChangedDelegate_Game, which is broadcast when the widget changes Game Settings */
	FOnPlayerSettingsChanged_VideoAndSound& GetVideoAndSoundDelegate() const { return VideoAndSound_Widget->GetPublicVideoAndSoundSettingsChangedDelegate(); }

	/** Returns the UAASettings widget's OnPlayerSettingsChangedDelegate_AudioAnalyzer, which is broadcast when the widget changes Audio Analyzer Settings */
	FOnPlayerSettingsChanged_AudioAnalyzer& GetAudioAnalyzerDelegate() const { return AudioAnalyzer_Widget->GetPublicAudioAnalyzerSettingsChangedDelegate(); }

	/** Returns the USensitivity widget's OnPlayerSettingsChangedDelegate_User, which is broadcast when the widget changes User Settings */
	FOnPlayerSettingsChanged_User& GetUserDelegate() const { return Sensitivity_Widget->GetPublicUserSettingsChangedDelegate();}
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnRestartButtonClicked_AudioAnalyzer() const;

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
	USettingsMenuWidget_Game* Game_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget_VideoAndSound* VideoAndSound_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget_Sensitivity* Sensitivity_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget_CrossHair* CrossHair_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget_AudioAnalyzer* AudioAnalyzer_Widget;

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
