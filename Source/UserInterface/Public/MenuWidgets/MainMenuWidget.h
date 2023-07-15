// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "HttpRequestInterface.h"
#include "SubMenuWidgets/GameModesWidget.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UFeedbackWidget;
class UScoreBrowserWidget;
class UVerticalBox;
class UWidgetSwitcher;
class UGameModesWidget;
class USettingsMenuWidget;
class UFAQWidget;
class ULoginWidget;
class UHorizontalBox;
class UOverlay;
class UEditableTextBox;
class UTextBlock;
class UButton;
class UMenuButton;

/** Widget that is the entry point into the game, holding most other widgets that aren't MenuWidgets */
UCLASS()
class USERINTERFACE_API UMainMenuWidget : public UUserWidget, public ISaveLoadInterface, public IHttpRequestInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UGameModesWidget* GameModesWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScoreBrowserWidget* ScoresWidget;
	UFUNCTION()
	void UpdateLoginState(const bool bSuccessfulLogin, const FString OptionalMessage = "");

	/** Returns OnPlayerSettingsChangedDelegate_User, the delegate that is broadcast when this class saves User settings */
	FOnPlayerSettingsChanged_User& GetUserDelegate() { return OnPlayerSettingsChangedDelegate_User; }

	/** Called when another class saves User settings */
	virtual void OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings) override;

protected:
	virtual void NativeConstruct() override;
	
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
	UBSButton* Button_Feedback;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* Button_Login_Register;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScoreBrowserWidget* WebBrowserOverlayPatchNotes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	ULoginWidget* LoginWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UFeedbackWidget* FeedbackWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_PatchNotes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_GameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Scores;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_FAQ;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Quit;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_SignInState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Username;

private:
	UFUNCTION()
	void OnMenuButtonClicked_BSButton(const UBSButton* Button);
	UFUNCTION()
	void OnButtonClicked_BSButton(const UBSButton* Button);
	UFUNCTION()
	void OnButtonClicked_Login(const FLoginPayload LoginPayload);
	UFUNCTION()
	void OnURLChangedResult_ScoresBrowser(const bool bSuccess);
	UFUNCTION()
	void OnLoginWidgetExitAnimationCompleted();

	FOnLoginResponse OnLoginResponse;

	bool bFadeInScoreBrowserOnButtonPress = false;
};
