// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerSettingsInterface.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class USaveGamePlayerScore;
class UScoreViewerWidget;
class UBSButton;
class UMenuStyle;
class UVerticalBox;
class UWidgetSwitcher;
class UGameModeMenuWidget;
class USettingsMenuWidget;
class UFAQWidget;
class UTextBlock;
class UMenuButton;

/** Widget that is the entry point into the game, holding most other widgets that aren't MenuWidgets. */
UCLASS()
class USERINTERFACE_API UMainMenuWidget : public UUserWidget, public IBSPlayerSettingsInterface
{
	GENERATED_BODY()

public:
	void UpdateLoginState(const FString& SteamPersonaName);

	/** Returns OnPlayerSettingsChangedDelegate_User, the delegate that is broadcast when this class saves User
	 *  settings. */
	FOnPlayerSettingsChanged_User& GetUserDelegate() { return OnPlayerSettingsChangedDelegate_User; }

	UGameModeMenuWidget* GetGameModesWidget() const;

	USettingsMenuWidget* GetSettingsMenuWidget() const;
	
	void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore);

protected:
	/** Called when another class saves User settings. */
	virtual void OnPlayerSettingsChanged(const FPlayerSettings_User& UserSettings) override;
	
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	void SetStyles();

	UPROPERTY()
	const UMenuStyle* MenuStyle;

	UPROPERTY(EditDefaultsOnly, Category = "MainMenuWidget")
	TSubclassOf<UMenuStyle> MenuStyleClass;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UGameModeMenuWidget* GameModesWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScoreViewerWidget* ScoreViewerWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MainMenuSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_PatchNotes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_GameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Scores;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Settings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_FAQ;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_PatchNotes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_GameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Settings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Scores;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_FAQ;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Quit;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Username;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_SignedInThroughSteamAs;

private:
	void OnMenuButtonClicked_BSButton(const UBSButton* Button);

	TWeakObjectPtr<UMenuButton> LastMenuButton;

	UPROPERTY()
	TObjectPtr<USaveGamePlayerScore> SaveGamePlayerScore;
};
