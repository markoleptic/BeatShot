// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "SubMenuWidgets/GameModesWidget.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UScoreBrowserWidget;
class UVerticalBox;
class UWidgetSwitcher;
class USlideRightButton;
class UGameModesWidget;
class USettingsMenuWidget;
class UFAQWidget;
class ULoginWidget;
class UHorizontalBox;
class UOverlay;
class UEditableTextBox;
class UTextBlock;
class UButton;

/** Widget that is the entry point into the game, holding most other widgets that aren't MenuWidgets */
UCLASS()
class USERINTERFACE_API UMainMenuWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UGameModesWidget* GameModesWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;

protected:
	virtual void NativeConstruct() override;

	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MainMenuWidgets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MainMenuSwitcher;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_PatchNotes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_GameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Scores;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_FAQ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScoreBrowserWidget* WebBrowserOverlayPatchNotes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScoreBrowserWidget* ScoresWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	ULoginWidget* LoginWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_PatchNotes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_GameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_Settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_Scores;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_FAQ;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_Quit;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_SignInState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Username;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_GitHubIssue;

private:
	/** Calls SlideButton() from SlideRightButton and sets the associated widget to active in the MainMenuSwitcher */
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	UFUNCTION()
	void OnButtonClicked_PatchNotes();
	UFUNCTION()
	void OnButtonClicked_GameModes();
	UFUNCTION()
	void OnButtonClicked_Scoring();
	UFUNCTION()
	void OnButtonClicked_Settings();
	UFUNCTION()
	void OnButtonClicked_FAQButton();
	UFUNCTION()
	void OnButtonClicked_Quit();
	UFUNCTION()
	void OnButtonClicked_Login(const FLoginPayload LoginPayload, const bool bIsPopup);
	UFUNCTION()
	void OnButtonClicked_GitHub();
	UFUNCTION()
	void OnLoginStateChange(const ELoginState& LoginState, const bool bIsPopup);
	/** Override with desired */
	UFUNCTION()
	virtual void UpdateLoginState(const bool bSuccessfulLogin);

	/** Whether or not to fade out the browser overlay to reveal the web browser for the ScoresWidget */
	bool bShowWebBrowserScoring;

	const FString GitHubURL = "https://github.com/markoleptic/BeatShot/issues/new/choose";
};
