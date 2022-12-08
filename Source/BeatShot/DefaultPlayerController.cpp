// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerController.h"
#include "MainMenuWidget.h"
#include "Crosshair.h"
#include "PlayerHUD.h"
#include "PauseMenu.h"
#include "Countdown.h"
#include "LoginWidget.h"
#include "PostGameMenuWidget.h"
#include "DefaultGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	GI->RegisterPlayerController(this);
	if (GI->LoadPlayerSettings().bShowFPSCounter)
	{
		ShowFPSCounter();
	}
	GI->OnPlayerSettingsChange.AddDynamic(this, &ADefaultPlayerController::OnPlayerSettingsChange);
	PlayerHUDActive = false;
	PostGameMenuActive = false;
}

void ADefaultPlayerController::setPlayerEnabledState(bool bPlayerEnabled)
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
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	MainMenu = CreateWidget<UMainMenuWidget>(this, MainMenuClass);
	MainMenu->AddToViewport();
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
	PauseMenu = CreateWidget<UPauseMenu>(this, PauseMenuClass);
	PauseMenu->AddToViewport();
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ADefaultPlayerController::HidePauseMenu()
{
	if (PauseMenu)
	{
		UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitGame);
		UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
		PauseMenu->RemoveFromViewport();
		PauseMenu = nullptr;
	}
}

void ADefaultPlayerController::ShowCrosshair()
{
	Crosshair = CreateWidget<UCrosshair>(this, CrosshairClass);
	Crosshair->AddToViewport();
}

void ADefaultPlayerController::HideCrosshair()
{
	if (Crosshair)
	{
		Crosshair->RemoveFromViewport();
		Crosshair = nullptr;
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
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitGame);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	FadeScreenFromBlack();
	Countdown = CreateWidget<UCountdown>(this, CountdownClass);
	Countdown->AddToViewport();
	CountdownActive = true;
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

void ADefaultPlayerController::ShowPostGameMenu(bool bSavedScores)
{
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	PostGameMenuWidget = CreateWidget<UPostGameMenuWidget>(this, PostGameMenuWidgetClass);
	PostGameMenuWidget->bSavedScores = bSavedScores;
	PostGameMenuWidget->AddToViewport();
	PostGameMenuActive = true;
	HandlePostGameMenuPause(true);
}

void ADefaultPlayerController::HidePostGameMenu()
{
	if (PostGameMenuWidget)
	{
		PostGameMenuWidget->RemoveFromViewport();
		PostGameMenuWidget = nullptr;
		PostGameMenuActive = false;
		HandlePostGameMenuPause(false);
	}
}

void ADefaultPlayerController::ShowPopupMessage()
{
	if (PopupMessageWidget)
	{
		if (PopupMessageWidget->GetParent())
		{
			PopupMessageWidget->GetParent()->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		setPlayerEnabledState(false);
		PopupMessageWidget->AddToViewport();
		PopupMessageWidget->SetVisibility(ESlateVisibility::Visible);
		if (GetWorld()->GetMapName().Contains("Range"))
		{
			SetInputMode(FInputModeUIOnly());
			SetShowMouseCursor(true);
		}
	}
}

void ADefaultPlayerController::HidePopupMessage()
{
	if (PopupMessageWidget)
	{
		if (PopupMessageWidget->GetParent())
		{
			PopupMessageWidget->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		PopupMessageWidget->RemoveFromViewport();
		PostGameMenuWidget = nullptr;
	}
	if (GetWorld()->GetMapName().Contains("Range"))
	{
		SetInputMode(FInputModeGameAndUI());
		SetShowMouseCursor(false);
		setPlayerEnabledState(true);
		if (!Crosshair)
		{
			ShowCrosshair();
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

/* Not being used since Main Menu should be the only place to encounter login screen and
 * it's already part of that widget */
void ADefaultPlayerController::ShowLogin(bool bHasSignedIn)
{
	LoginWidget = CreateWidget<ULoginWidget>(this, LoginClass);
	if (LoginWidget->GetParent())
	{
		LoginWidget->GetParent()->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	if (bHasSignedIn)
	{
		LoginWidget->DefaultSignInText->SetVisibility(ESlateVisibility::Collapsed);
		LoginWidget->HasSignedInBeforeText->SetVisibility(ESlateVisibility::Visible);
	}
	LoginWidget->RegisterOverlay->SetVisibility(ESlateVisibility::Collapsed);
	LoginWidget->LoggedInOverlay->SetVisibility(ESlateVisibility::Collapsed);
	LoginWidget->ContinueWithoutOverlay->SetVisibility(ESlateVisibility::Collapsed);
	LoginWidget->AddToViewport();
	LoginWidget->PlayAnimationForward(LoginWidget->FadeInAllInitial, 1, false);
	LoginWidget->ExitLogin.AddDynamic(this, &ADefaultPlayerController::HideLogin);
}

void ADefaultPlayerController::HideLogin()
{
	if (LoginWidget)
	{
		if (LoginWidget->GetParent())
		{
			LoginWidget->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		LoginWidget->RemoveFromViewport();
		LoginWidget = nullptr;
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
	OnScreenFadeToBlackFinish.Broadcast();
	OnScreenFadeToBlackFinish.Clear();
}

void ADefaultPlayerController::OnFadeScreenFromBlackFinish()
{
	ScreenFadeWidget->OnFadeFromBlackFinish.RemoveAll(this);
	ScreenFadeWidget->RemoveFromViewport();
	ScreenFadeWidget = nullptr;
}

bool ADefaultPlayerController::IsPlayerHUDActive()
{
	return PlayerHUDActive;
}

bool ADefaultPlayerController::IsPostGameMenuActive()
{
	return PostGameMenuActive;
}

UPopupMessageWidget* ADefaultPlayerController::CreatePopupMessageWidget(bool bDestroyOnClick, int32 ButtonIndex)
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	if (bDestroyOnClick)
	{
		if (ButtonIndex == 1)
		{
			PopupMessageWidget->Button1->OnClicked.AddDynamic(this, &ADefaultPlayerController::HidePopupMessage);
		}
		else
		{
			PopupMessageWidget->Button2->OnClicked.AddDynamic(this, &ADefaultPlayerController::HidePopupMessage);
		}
	}
	return PopupMessageWidget;
}

void ADefaultPlayerController::OnPlayerSettingsChange(FPlayerSettings PlayerSettings)
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
