// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
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
/**
 * 
 */
UCLASS()
class USERINTERFACE_API UMainMenuWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

#pragma region MenuWidgets

	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MainMenuWidgets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MainMenuSwitcher;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* PatchNotes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* GameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Scores;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SettingsMenu;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* FAQ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScoreBrowserWidget* WebBrowserOverlayPatchNotes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScoreBrowserWidget* ScoresWidget;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UGameModesWidget* GameModesWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* PatchNotesButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* GameModesButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SettingsButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* ScoresButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* FAQButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* QuitButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	ULoginWidget* LoginWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SignInStateText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* UsernameText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* GitHubIssueButton;

#pragma	endregion

private:
	/** Calls SlideButton() from SlideRightButton and sets the associated widget to active in the MainMenuSwitcher */
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	UFUNCTION()
	void OnPatchNotesButtonClicked();
	UFUNCTION()
	void OnGameModesButtonClicked();
	UFUNCTION()
	void OnScoringButtonClicked();
	UFUNCTION()
	void OnSettingsButtonClicked();
	UFUNCTION()
	void OnFAQButtonClicked();
	UFUNCTION()
	void OnQuitButtonClicked();
	UFUNCTION()
	void OnLoginButtonClicked(const FLoginPayload LoginPayload, const bool bIsPopup);
	UFUNCTION()
	void OnGitHubButtonClicked();
	UFUNCTION()
	void OnLoginStateChange(const ELoginState& LoginState, const bool bIsPopup);
	/** Override with desired */
	UFUNCTION()
	virtual void UpdateLoginState(const bool bSuccessfulLogin);

	/** Whether or not to fade out the browser overlay to reveal the web browser for the ScoresWidget */
	bool bShowWebBrowserScoring;

	const FString GitHubURL = "https://github.com/markoleptic/BeatShot/issues/new/choose";
};
