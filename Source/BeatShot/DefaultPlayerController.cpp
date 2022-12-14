// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerController.h"
#include "MainMenuWidget.h"
#include "CrossHairWidget.h"
#include "PlayerHUD.h"
#include "PauseMenuWidget.h"
#include "Countdown.h"
#include "DefaultCharacter.h"
#include "PostGameMenuWidget.h"
#include "DefaultGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI->LoadPlayerSettings().bShowFPSCounter)
	{
		ShowFPSCounter();
	}
	GI->OnPlayerSettingsChange.AddDynamic(this, &ADefaultPlayerController::OnPlayerSettingsChange);
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
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitMenu);
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
	PauseMenu->AddToViewport();
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ADefaultPlayerController::HidePauseMenu()
{
	if (PauseMenu)
	{
		PauseMenu->RemoveFromViewport();
		PauseMenu = nullptr;
		UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitGame);
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
	SetControlRotation(FRotator(0,0,0));
	if (GetPawn() != nullptr)
	{
		Cast<ADefaultCharacter>(GetPawn())->SetActorLocationAndRotation(FVector(1580,0,102), FRotator(0,0,0));
	}
	FadeScreenFromBlack();
	Countdown = CreateWidget<UCountdown>(this, CountdownClass);
	Countdown->AddToViewport();
	CountdownActive = true;
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitGame);
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

void ADefaultPlayerController::ShowPostGameMenu(const bool bSavedScores)
{
	GI->OnPostPlayerScoresResponse.AddDynamic(this, &ADefaultPlayerController::OnPostPlayerScoresResponse);
	PostGameMenuWidget = CreateWidget<UPostGameMenuWidget>(this, PostGameMenuWidgetClass);
	/** If scores weren't saved, update Overlay text to reflect that. This also means OnPostPlayerScores won't get called */
	if (!bSavedScores)
	{
		UE_LOG(LogTemp, Display, TEXT("Didn't save player scores"));
		PostGameMenuWidget->ScoresWidget->SetOverlayText("DidNotSaveScores");
	}
	PostGameMenuWidget->AddToViewport();
	PostGameMenuActive = true;
	//HandlePostGameMenuPause(true);
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	SetPlayerEnabledState(false);
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ADefaultPlayerController::OnPostPlayerScoresResponse(const ELoginState& LoginState) 
{
	if (PostGameMenuWidget)
	{
		PostGameMenuWidget->ScoresWidget->InitializePostGameScoringOverlay(LoginState);
	}
	GI->OnPostPlayerScoresResponse.RemoveDynamic(this, &ADefaultPlayerController::OnPostPlayerScoresResponse);
}

void ADefaultPlayerController::HidePostGameMenu()
{
	if (PostGameMenuWidget)
	{
		PostGameMenuWidget->RemoveFromViewport();
		PostGameMenuWidget = nullptr;
		PostGameMenuActive = false;
		//HandlePostGameMenuPause(false);
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
		SetPlayerEnabledState(true);
	}
}

UPopupMessageWidget* ADefaultPlayerController::CreatePopupMessageWidget(const bool bDestroyOnClick, const int32 ButtonIndex)
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	if (PopupMessageWidget)
	{
		PopupMessageWidget->AddToViewport();
		PopupMessageWidget->FadeIn();
		if (GetWorld()->GetMapName().Contains("Range")  && !UGameplayStatics::IsGamePaused(GetWorld()))
		{
			SetInputMode(FInputModeUIOnly());
			SetShowMouseCursor(true);
			SetPlayerEnabledState(false);
		}
	}
	return PopupMessageWidget;
}

void ADefaultPlayerController::HidePopupMessage()
{
	if (PopupMessageWidget != nullptr)
	{
		PopupMessageWidget->FadeOut();
	}
}

void ADefaultPlayerController::OnFadeOutPopupMessageFinish()
{
	PopupMessageWidget->RemoveFromViewport();
	PostGameMenuWidget = nullptr;
	if (GetWorld()->GetMapName().Contains("Range") && !UGameplayStatics::IsGamePaused(GetWorld()))
	{
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
		SetPlayerEnabledState(true);
		if (!CrossHair)
		{
			ShowCrossHair();
		}
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
	ScreenFadeWidget->OnFadeToBlackFinish.AddDynamic(this, &ADefaultPlayerController::OnFadeScreenToBlackFinish);
	ScreenFadeWidget->FadeToBlack();
}

void ADefaultPlayerController::FadeScreenFromBlack()
{
	if (!ScreenFadeWidget)
	{
		ScreenFadeWidget = CreateWidget<UScreenFadeWidget>(this, ScreenFadeClass);
		ScreenFadeWidget->AddToViewport(ZOrderFadeScreen);
	}
	ScreenFadeWidget->OnFadeFromBlackFinish.AddDynamic(this, &ADefaultPlayerController::OnFadeScreenFromBlackFinish);
	ScreenFadeWidget->FadeFromBlack();
}

void ADefaultPlayerController::OnFadeScreenToBlackFinish()
{
	if (ScreenFadeWidget)
	{
		OnScreenFadeToBlackFinish.Broadcast();
		OnScreenFadeToBlackFinish.Clear();
	}
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

void ADefaultPlayerController::OnPlayerSettingsChange(const FPlayerSettings& PlayerSettings)
{
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
