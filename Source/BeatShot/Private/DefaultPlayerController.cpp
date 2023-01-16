// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerController.h"
#include "DefaultCharacter.h"
#include "DefaultGameInstance.h"
#include "DefaultGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MenuWidgets/MainMenuWidget.h"
#include "MenuWidgets/PauseMenuWidget.h"
#include "MenuWidgets/PostGameMenuWidget.h"
#include "OverlayWidgets/CountdownWidget.h"
#include "OverlayWidgets/CrossHairWidget.h"
#include "OverlayWidgets/FPSCounterWidget.h"
#include "OverlayWidgets/PlayerHUD.h"
#include "OverlayWidgets/ScreenFadeWidget.h"
#include "SubMenuWidgets/ScoreBrowserWidget.h"
#include "SubMenuWidgets/SettingsMenuWidget.h"
#include "SubMenuWidgets/AASettingsWidget.h"

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (LoadPlayerSettings().bShowFPSCounter)
	{
		ShowFPSCounter();
	}
	PlayerHUDActive = false;
	PostGameMenuActive = false;
}

void ADefaultPlayerController::SetPlayerEnabledState(const bool bPlayerEnabled)
{
	if (GetWorld()->GetMapName().Contains("Range"))
	{
		if (bPlayerEnabled)
		{
			GetPawn()->EnableInput(this);
		}
		else
		{
			GetPawn()->DisableInput(this);
		}
	}
}

void ADefaultPlayerController::ShowMainMenu()
{
	FadeScreenFromBlack();
	MainMenu = CreateWidget<UMainMenuWidget>(this, MainMenuClass);
	MainMenu->AddToViewport();
	MainMenu->GameModesWidget->OnGameModeStateChanged.AddUFunction(Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())), "HandleGameModeTransition");
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(LoadPlayerSettings().FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ADefaultPlayerController::HideMainMenu()
{
	if (MainMenu)
	{
		MainMenu->RemoveFromViewport();
		MainMenu = nullptr;
	}
}

void ADefaultPlayerController::ShowPauseMenu()
{
	PauseMenu = CreateWidget<UPauseMenuWidget>(this, PauseMenuClass);
	PauseMenu->ResumeGame.BindLambda([&]
	{
		HandlePause();
		HidePauseMenu();
	});
	PauseMenu->SettingsMenuWidget->OnPlayerSettingsChange.AddDynamic(this, &ADefaultPlayerController::OnPlayerSettingsChanged);
	if (ADefaultCharacter* DefaultCharacter = Cast<ADefaultCharacter>(GetPawn()))
	{
		PauseMenu->SettingsMenuWidget->OnPlayerSettingsChange.AddDynamic(DefaultCharacter, &ADefaultCharacter::OnUserSettingsChange);
	}
	if (ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		PauseMenu->SettingsMenuWidget->OnPlayerSettingsChange.AddDynamic(GameMode, &ADefaultGameMode::RefreshPlayerSettings);
		PauseMenu->SettingsMenuWidget->AASettingsWidget->OnAASettingsChange.AddDynamic(GameMode, &ADefaultGameMode::RefreshAASettings);
	}
	if (CrossHair)
	{
		PauseMenu->SettingsMenuWidget->OnPlayerSettingsChange.AddDynamic(CrossHair, &UCrossHairWidget::OnPlayerSettingsChange);
	}
	PauseMenu->QuitMenuWidget->OnGameModeStateChanged.AddUFunction(
		Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())), "HandleGameModeTransition");
	PauseMenu->AddToViewport();
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(LoadPlayerSettings().FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ADefaultPlayerController::HidePauseMenu()
{
	if (PauseMenu)
	{
		PauseMenu->RemoveFromViewport();
		PauseMenu = nullptr;
		UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(LoadPlayerSettings().FrameRateLimitGame);
		UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	}
}

void ADefaultPlayerController::ShowCrossHair()
{
	CrossHair = CreateWidget<UCrossHairWidget>(this, CrossHairClass);
	CrossHair->AddToViewport();
}

void ADefaultPlayerController::HideCrossHair()
{
	if (CrossHair)
	{
		CrossHair->RemoveFromViewport();
		CrossHair = nullptr;
	}
}

void ADefaultPlayerController::ShowPlayerHUD()
{
	PlayerHUD = CreateWidget<UPlayerHUD>(this, PlayerHUDClass);
	const UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->UpdateScoresToHUD.BindUFunction(PlayerHUD, FName("UpdateAllElements"));
	GameMode->OnAAManagerSecondPassed.BindUFunction(PlayerHUD, FName("UpdateSongProgress"));
	if (GI->GameModeActorStruct.IsBeatTrackMode)
	{
		PlayerHUD->TargetsSpawnedBox->SetVisibility(ESlateVisibility::Collapsed);
		PlayerHUD->StreakBox->SetVisibility(ESlateVisibility::Collapsed);
		PlayerHUD->TargetsHitBox->SetVisibility(ESlateVisibility::Collapsed);
		PlayerHUD->ShotsFiredBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	PlayerHUD->AddToViewport();
	PlayerHUDActive = true;
}

void ADefaultPlayerController::HidePlayerHUD()
{
	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromViewport();
		PlayerHUD = nullptr;
		PlayerHUDActive = false;
	}
}

void ADefaultPlayerController::ShowCountdown()
{
	SetControlRotation(FRotator(0, 0, 0));
	if (GetPawn() != nullptr)
	{
		Cast<ADefaultCharacter>(GetPawn())->SetActorLocationAndRotation(FVector(1580, 0, 102), FRotator(0, 0, 0));
	}
	FadeScreenFromBlack();
	Countdown = CreateWidget<UCountdownWidget>(this, CountdownClass);
	Countdown->PlayerDelay = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->
	                         GameModeActorStruct.PlayerDelay;
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	Countdown->OnCountdownCompleted.BindUFunction(GameMode, "StartGameMode");
	Countdown->StartAAManagerPlayback.BindUFunction(GameMode, "StartAAManagerPlayback");
	Countdown->AddToViewport();
	CountdownActive = true;
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(LoadPlayerSettings().FrameRateLimitGame);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ADefaultPlayerController::HideCountdown()
{
	if (Countdown)
	{
		Countdown->RemoveFromViewport();
		Countdown = nullptr;
		CountdownActive = false;
	}
}

void ADefaultPlayerController::ShowPostGameMenu()
{
	PostGameMenuWidget = CreateWidget<UPostGameMenuWidget>(this, PostGameMenuWidgetClass);
	PostGameMenuWidget->SettingsMenuWidget->OnPlayerSettingsChange.AddDynamic(this, &ADefaultPlayerController::OnPlayerSettingsChanged);
	if (UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		PostGameMenuWidget->GameModesWidget->OnGameModeStateChanged.AddUFunction(GI, "HandleGameModeTransition");
		PostGameMenuWidget->QuitMenuWidget->OnGameModeStateChanged.AddUFunction(GI, "HandleGameModeTransition");
	}
	if (Cast<ADefaultCharacter>(GetPawn()))
	{
		PostGameMenuWidget->SettingsMenuWidget->OnPlayerSettingsChange.AddDynamic(Cast<ADefaultCharacter>(GetPawn()), &ADefaultCharacter::OnUserSettingsChange);
	}
	if (CrossHair)
	{
		PostGameMenuWidget->SettingsMenuWidget->OnPlayerSettingsChange.AddDynamic(CrossHair, &UCrossHairWidget::OnPlayerSettingsChange);
	}
	if (ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		PostGameMenuWidget->SettingsMenuWidget->OnPlayerSettingsChange.AddDynamic(GameMode, &ADefaultGameMode::RefreshPlayerSettings);
		PostGameMenuWidget->SettingsMenuWidget->AASettingsWidget->OnAASettingsChange.AddDynamic(GameMode, &ADefaultGameMode::RefreshAASettings);
	}
	PostGameMenuWidget->AddToViewport();
	PostGameMenuActive = true;
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	SetPlayerEnabledState(false);
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(LoadPlayerSettings().FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ADefaultPlayerController::OnPostPlayerScoresResponse(const bool bDidPostScores, const ELoginState& LoginState)
{
	if (!bDidPostScores)
	{
		UE_LOG(LogTemp, Display, TEXT("Didn't post player scores"));
		PostGameMenuWidget->ScoresWidget->SetOverlayText("DidNotSaveScores");
	}
	if (PostGameMenuWidget)
	{
		PostGameMenuWidget->ScoresWidget->InitializePostGameScoringOverlay(LoginState);
	}
}

void ADefaultPlayerController::HidePostGameMenu()
{
	if (PostGameMenuWidget)
	{
		PostGameMenuWidget->RemoveFromViewport();
		PostGameMenuWidget = nullptr;
		PostGameMenuActive = false;
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
		SetPlayerEnabledState(true);
	}
}

void ADefaultPlayerController::ShowFPSCounter()
{
	if (FPSCounter == nullptr)
	{
		FPSCounter = CreateWidget<UFPSCounterWidget>(this, FPSCounterClass);
		FPSCounter->AddToViewport(ZOrderFPSCounter);
	}
}

void ADefaultPlayerController::HideFPSCounter()
{
	if (FPSCounter)
	{
		FPSCounter->RemoveFromViewport();
		FPSCounter = nullptr;
	}
}

void ADefaultPlayerController::FadeScreenToBlack()
{
	if (!ScreenFadeWidget)
	{
		ScreenFadeWidget = CreateWidget<UScreenFadeWidget>(this, ScreenFadeClass);
	}
	ScreenFadeWidget->AddToViewport(ZOrderFadeScreen);
	ScreenFadeWidget->OnFadeToBlackFinish.AddLambda([&]
	{
		if (!OnScreenFadeToBlackFinish.ExecuteIfBound())
		{
			UE_LOG(LogTemp, Display, TEXT("OnScreenFadeToBlackFinish not bound."));
		}
	});
	ScreenFadeWidget->FadeToBlack();
}

void ADefaultPlayerController::FadeScreenFromBlack()
{
	if (!ScreenFadeWidget)
	{
		ScreenFadeWidget = CreateWidget<UScreenFadeWidget>(this, ScreenFadeClass);
		ScreenFadeWidget->AddToViewport(ZOrderFadeScreen);
	}
	ScreenFadeWidget->OnFadeFromBlackFinish.AddUFunction(this, "OnFadeScreenFromBlackFinish");
	ScreenFadeWidget->FadeFromBlack();
}

void ADefaultPlayerController::OnFadeScreenFromBlackFinish()
{
	if (ScreenFadeWidget)
	{
		ScreenFadeWidget->OnFadeFromBlackFinish.RemoveAll(this);
		ScreenFadeWidget->RemoveFromViewport();
		ScreenFadeWidget = nullptr;
	}
}

bool ADefaultPlayerController::IsPlayerHUDActive() const
{
	return PlayerHUDActive;
}

bool ADefaultPlayerController::IsPostGameMenuActive() const
{
	return PostGameMenuActive;
}

void ADefaultPlayerController::OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings)
{
	UE_LOG(LogTemp, Display, TEXT("OnPlayerSettingsChanged called"));
	if (PlayerSettings.bShowFPSCounter)
	{
		if (!FPSCounter)
		{
			ShowFPSCounter();
		}
	}
	else
	{
		if (FPSCounter)
		{
			HideFPSCounter();
		}
	}
}
