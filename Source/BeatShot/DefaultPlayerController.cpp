// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerController.h"
#include "MainMenuWidget.h"
#include "LoadingScreenWidget.h"
#include "Crosshair.h"
#include "PlayerHUD.h"
#include "PauseMenu.h"
#include "Countdown.h"
#include "DefaultCharacter.h"
#include "LoginWidget.h"
#include "PostGameMenuWidget.h"
#include "DefaultGameInstance.h"
#include "WebBrowserWidget.h"
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
	GI->OnPostPlayerScoresResponse.AddDynamic(this, &ADefaultPlayerController::OnPostPlayerScoresResponse);
}

void ADefaultPlayerController::SetPlayerEnabledState(bool bPlayerEnabled)
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
	PauseMenu = CreateWidget<UPauseMenu>(this, PauseMenuClass);
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
	PostGameMenuWidget = CreateWidget<UPostGameMenuWidget>(this, PostGameMenuWidgetClass);
	PostGameMenuWidget->bSavedScores = bSavedScores;
	PostGameMenuWidget->AddToViewport();
	PostGameMenuActive = true;
	HandlePostGameMenuPause(true);
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(GI->LoadPlayerSettings().FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ADefaultPlayerController::OnPostPlayerScoresResponse(FString Message, int32 ResponseCode)
{
	UE_LOG(LogTemp, Display, TEXT("HitPostPlayerScoresResponse"));
	if (!PostGameMenuActive)
	{
		return;
	}
	if (ResponseCode != 200)
	{
		PostGameMenuWidget->ScoresOverlayTextSwitcher(2);
		PostGameMenuWidget->ScoringButtonClicked();
		return;
	}
	
	const FPlayerSettings PlayerSettings = GI->LoadPlayerSettings();
	if (!PlayerSettings.HasLoggedInHttp)
	{
		PostGameMenuWidget->ScoresOverlayTextSwitcher(2);
		PostGameMenuWidget->ScoringButtonClicked();
		return;
	}
	PostGameMenuWidget->WebBrowserWidget->OnURLLoaded.AddDynamic(this, &ADefaultPlayerController::OnURLLoaded);
	if (GI->GameModeActorStruct.CustomGameModeName.IsEmpty())
	{
		PostGameMenuWidget->WebBrowserWidget->LoadDefaultGameModesURL(PlayerSettings.Username);
	}
	else
	{
		PostGameMenuWidget->WebBrowserWidget->LoadCustomGameModesURL(PlayerSettings.Username);
	}
}

void ADefaultPlayerController::OnURLLoaded(const bool bLoadedSuccessfully)
{
	if (!PostGameMenuActive)
	{
		return;
	}
	if (bLoadedSuccessfully)
	{
		PostGameMenuWidget->bShowScoresWebBrowser = true;
		PostGameMenuWidget->ScoringButtonClicked();
	}
	else
	{
		PostGameMenuWidget->ScoresOverlayTextSwitcher(4);
		PostGameMenuWidget->ScoringButtonClicked();
	}
	PostGameMenuWidget->WebBrowserWidget->OnURLLoaded.RemoveDynamic(this, &ADefaultPlayerController::OnURLLoaded);
}

void ADefaultPlayerController::OnLoadingScreenFadeOutFinish()
{
	LoadingScreenWidget->RemoveFromViewport();
	LoadingScreenWidget->Destruct();
	LoadingScreenWidget = nullptr;
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
		SetPlayerEnabledState(false);
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
		SetPlayerEnabledState(true);
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
void ADefaultPlayerController::ShowLogin(const bool bHasSignedIn)
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

void ADefaultPlayerController::FadeInLoadingScreen()
{
}

void ADefaultPlayerController::FadeOutLoadingScreen(float LastTime)
{
	LoadingScreenWidget = CreateWidget<ULoadingScreenWidget>(this, LoadingScreenClass);
	LoadingScreenWidget->Time = LastTime;
	LoadingScreenWidget->AddToViewport(21);
	LoadingScreenWidget->BindToFadeOutFinish(this, FName("OnLoadingScreenFadeOutFinish"));
	LoadingScreenWidget->FadeOut();
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

UPopupMessageWidget* ADefaultPlayerController::CreatePopupMessageWidget(const bool bDestroyOnClick, const int32 ButtonIndex)
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

void ADefaultPlayerController::OnPlayerSettingsChange(const FPlayerSettings PlayerSettings)
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
