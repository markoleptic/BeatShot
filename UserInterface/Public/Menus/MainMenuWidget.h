// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerScoreInterface.h"
#include "BSPlayerSettingsInterface.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UScoreViewerWidget;
class UBSButton;
class UMenuStyle;
class UFeedbackWidget;
class UVerticalBox;
class UWidgetSwitcher;
class UGameModeMenuWidget;
class USettingsMenuWidget;
class UFAQWidget;
class UTextBlock;
class UMenuButton;

/** Widget that is the entry point into the game, holding most other widgets that aren't MenuWidgets. */
UCLASS()
class USERINTERFACE_API UMainMenuWidget : public UUserWidget,
                                          public IBSPlayerSettingsInterface,
                                          public IBSPlayerScoreInterface
{
	GENERATED_BODY()

public:
	void UpdateLoginState(const FString& SteamPersonaName);

	/** Returns OnPlayerSettingsChangedDelegate_User, the delegate that is broadcast when this class saves User
	 *  settings. */
	FOnPlayerSettingsChanged_User& GetUserDelegate() { return OnPlayerSettingsChangedDelegate_User; }

	UGameModeMenuWidget* GetGameModesWidget() const;

	USettingsMenuWidget* GetSettingsMenuWidget() const;

	/** Called when another class saves User settings. */
	virtual void OnPlayerSettingsChanged(const FPlayerSettings_User& UserSettings) override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	void SetStyles();

	UPROPERTY()
	const UMenuStyle* MenuStyle;

	UPROPERTY(EditDefaultsOnly, Category = "MainMenuWidget")
	TSubclassOf<UMenuStyle> MenuStyleClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UGameModeMenuWidget* GameModesWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScoreViewerWidget* ScoreViewerWidget;

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
	UMenuButton* Button_Feedback;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFeedbackWidget> FeedbackWidgetClass;

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
	UTextBlock* TextBlock_Username;

private:
	void OnMenuButtonClicked_BSButton(const UBSButton* Button);

	TWeakObjectPtr<UMenuButton> LastMenuButton;
};
