// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Menus/PostGameMenuWidget.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Menus/GameModeMenuWidget.h"
#include "Menus/ScoreViewerWidget.h"
#include "Overlays/AudioSelectWidget.h"
#include "Overlays/QuitMenuWidget.h"
#include "Styles/MenuStyle.h"
#include "Utilities/Buttons/MenuButton.h"

void UPostGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetStyles();

	MenuButton_Scores->SetDefaults(Box_Scores, MenuButton_PlayAgain);
	MenuButton_PlayAgain->SetDefaults(nullptr, MenuButton_GameModes);
	MenuButton_GameModes->SetDefaults(Box_GameModes, MenuButton_Settings);
	MenuButton_Settings->SetDefaults(Box_Settings, MenuButton_FAQ);
	MenuButton_FAQ->SetDefaults(Box_FAQ, MenuButton_Quit);
	MenuButton_Quit->SetDefaults(nullptr, MenuButton_Scores);

	MenuButton_Scores->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_PlayAgain->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_GameModes->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_Settings->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_FAQ->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_Quit->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);

	FadeInWidgetDelegate.BindDynamic(this, &UPostGameMenuWidget::SetScoresWidgetVisibility);

	BindToAnimationFinished(FadeInWidget, FadeInWidgetDelegate);
	PlayFadeInWidget();
}

void UPostGameMenuWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetStyles();
}

void UPostGameMenuWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	SaveGamePlayerScore = InSaveGamePlayerScore;
	GameModesWidget->SetSaveGamePlayerScore(InSaveGamePlayerScore);
	ScoreViewerWidget->LoadScores(SaveGamePlayerScore, true);
}

void UPostGameMenuWidget::SetStyles()
{
	MenuStyle = IBSWidgetInterface::GetStyleCDO(MenuStyleClass);
}

void UPostGameMenuWidget::ShowAudioFormatSelect()
{
	auto* AudioSelectWidget = CreateWidget<UAudioSelectWidget>(this, AudioSelectClass);
	AudioSelectWidget->SetSongs(SaveGamePlayerScore->CreateSongDurationMap());
	AudioSelectWidget->OnStartButtonClickedDelegate.BindLambda(
		[this, AudioSelectWidget](const FBS_AudioConfig& AudioConfig)
		{
			FGameModeTransitionState GameModeTransitionState;
			GameModeTransitionState.TransitionState = ETransitionState::PlayAgain;
			GameModeTransitionState.bSaveCurrentScores = false;
			GameModeTransitionState.BSConfig.AudioConfig.SongTitle = AudioConfig.SongTitle;
			GameModeTransitionState.BSConfig.AudioConfig.SongLength = AudioConfig.SongLength;
			GameModeTransitionState.BSConfig.AudioConfig.InAudioDevice = AudioConfig.InAudioDevice;
			GameModeTransitionState.BSConfig.AudioConfig.SongPath = AudioConfig.SongPath;
			GameModeTransitionState.BSConfig.AudioConfig.bPlaybackAudio = AudioConfig.bPlaybackAudio;
			GameModeTransitionState.BSConfig.AudioConfig.AudioFormat = AudioConfig.AudioFormat;
			OnGameModeStateChanged.ExecuteIfBound(GameModeTransitionState);
			AudioSelectWidget->FadeOut();
		});
	AudioSelectWidget->OnExitAudioSelect.BindLambda([this]
	{
		if (auto* LastMenuButtonLock = LastMenuButton.Get())
		{
			if (const auto AssociatedWidget = LastMenuButtonLock->GetAssociatedWidget())
			{
				LastMenuButtonLock->SetActive();
				MenuSwitcher->SetActiveWidget(AssociatedWidget);
			}
		}
	});
	AudioSelectWidget->AddToViewport();
	AudioSelectWidget->FadeIn();
}

void UPostGameMenuWidget::SetScoresWidgetVisibility()
{
	ScoreViewerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	MenuButton_Scores->SetActive();
	MenuSwitcher->SetActiveWidget(MenuButton_Scores->GetAssociatedWidget());
}

void UPostGameMenuWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	const UMenuButton* MenuButton = Cast<UMenuButton>(Button);
	if (!MenuButton)
	{
		return;
	}

	// Always stop the game mode preview if game modes widget is not visible
	if (MenuButton != MenuButton_GameModes)
	{
		GameModesWidget->StopGameModePreview();
	}

	if (Button == MenuButton_Quit)
	{
		auto* QuitMenuWidget = CreateWidget<UQuitMenuWidget>(this, QuitMenuWidgetClass);
		QuitMenuWidget->SetIsPostGameModeMenuChild(true);
		QuitMenuWidget->OnGameModeStateChanged.BindLambda([this](auto&&... Args)
		{
			OnGameModeStateChanged.ExecuteIfBound(Forward<decltype(Args)>(Args)...);
		});
		QuitMenuWidget->OnExitQuitMenu.BindLambda([this]
		{
			if (auto* LastMenuButtonLock = LastMenuButton.Get())
			{
				if (const auto AssociatedWidget = LastMenuButtonLock->GetAssociatedWidget())
				{
					LastMenuButtonLock->SetActive();
					MenuSwitcher->SetActiveWidget(AssociatedWidget);
				}
			}
		});
		QuitMenuWidget->AddToViewport();
		QuitMenuWidget->PlayInitialFadeInMenu();
	}
	else if (Button == MenuButton_PlayAgain)
	{
		ShowAudioFormatSelect();
	}

	if (const auto AssociatedWidget = MenuButton->GetAssociatedWidget())
	{
		MenuSwitcher->SetActiveWidget(AssociatedWidget);
		if (Button != MenuButton_Quit && Button != MenuButton_PlayAgain)
		{
			LastMenuButton = MakeWeakObjectPtr(const_cast<UMenuButton*>(MenuButton));
		}
	}
}
