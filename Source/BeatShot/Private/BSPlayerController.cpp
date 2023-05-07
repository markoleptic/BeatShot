// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSPlayerController.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility/BSAbilitySystemComponent.h"
#include "BSCharacter.h"
#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "BSPlayerState.h"
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
#include "OverlayWidgets/RLAgentWidget.h"
#include "OverlayWidgets/ScreenFadeWidget.h"
#include "SubMenuWidgets/ScoreBrowserWidget.h"
#include "SubMenuWidgets/SettingsMenuWidget.h"

void ABSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		if (IsLocalController())
		{
			UE_LOG(LogTemp, Display, TEXT("Local Controller %s: Has Authority: true, %s %s"), *GetNameSafe(this), *UEnum::GetValueAsString(GetLocalRole()), *UEnum::GetValueAsString(GetRemoteRole()));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("NotLocal Controller %s: Has Authority: true, %s %s"), *GetNameSafe(this), *UEnum::GetValueAsString(GetLocalRole()), *UEnum::GetValueAsString(GetRemoteRole()));
		}
	}
	else
	{
		if (IsLocalController())
		{
			UE_LOG(LogTemp, Display, TEXT("Local Controller %s: Has Authority: false, %s %s"), *GetNameSafe(this), *UEnum::GetValueAsString(GetLocalRole()), *UEnum::GetValueAsString(GetRemoteRole()));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("NotLocal Controller %s: Has Authority: false, %s %s"), *GetNameSafe(this), *UEnum::GetValueAsString(GetLocalRole()), *UEnum::GetValueAsString(GetRemoteRole()));
		}
	}

	if (LoadPlayerSettings().VideoAndSound.bShowFPSCounter)
	{
		ShowFPSCounter();
	}

	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_VideoAndSound);
	GI->GetPublicVideoAndSoundSettingsChangedDelegate().AddUniqueDynamic(this, &ABSPlayerController::ABSPlayerController::OnPlayerSettingsChanged);
	
	PlayerHUDActive = false;
	PostGameMenuActive = false;
	if (ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->OnGameModeStarted.AddLambda([&]
		{
			ShowCrossHair();
			ShowPlayerHUD();
			HideCountdown();
		});
	}
}

ABSPlayerState* ABSPlayerController::GetBSPlayerState() const
{
	return CastChecked<ABSPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UBSAbilitySystemComponent* ABSPlayerController::GetBSAbilitySystemComponent() const
{
	const ABSPlayerState* PS = GetBSPlayerState();
	return (PS ? PS->GetBSAbilitySystemComponent() : nullptr);
}

void ABSPlayerController::SetPlayerEnabledState(const bool bPlayerEnabled)
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

void ABSPlayerController::ShowMainMenu()
{
	if (!IsLocalController())
	{
		return;
	}
	FadeScreenFromBlack();
	MainMenu = CreateWidget<UMainMenuWidget>(this, MainMenuClass);
	MainMenu->AddToViewport();
	MainMenu->GameModesWidget->OnGameModeStateChanged.AddUObject(Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())), &UBSGameInstance::HandleGameModeTransition);
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(LoadPlayerSettings().VideoAndSound.FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ABSPlayerController::HideMainMenu()
{
	if (MainMenu)
	{
		MainMenu->RemoveFromParent();
		MainMenu = nullptr;
	}
}

void ABSPlayerController::ShowPauseMenu()
{
	if (!IsLocalController())
	{
		return;
	}
	PauseMenu = CreateWidget<UPauseMenuWidget>(this, PauseMenuClass);
	PauseMenu->ResumeGame.BindLambda([&]
	{
		HandlePause();
		HidePauseMenu();
	});
	
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	GI->AddDelegateToOnPlayerSettingsChanged(PauseMenu->SettingsMenuWidget->GetGameDelegate());
	GI->AddDelegateToOnPlayerSettingsChanged(PauseMenu->SettingsMenuWidget->GetVideoAndSoundDelegate());
	GI->AddDelegateToOnPlayerSettingsChanged(PauseMenu->SettingsMenuWidget->GetCrossHairDelegate());
	GI->AddDelegateToOnPlayerSettingsChanged(PauseMenu->SettingsMenuWidget->GetAudioAnalyzerDelegate());
	GI->AddDelegateToOnPlayerSettingsChanged(PauseMenu->SettingsMenuWidget->GetUserDelegate());
	
	PauseMenu->QuitMenuWidget->OnGameModeStateChanged.AddUObject(Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())), &UBSGameInstance::HandleGameModeTransition);
	PauseMenu->AddToViewport();
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(LoadPlayerSettings().VideoAndSound.FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ABSPlayerController::HidePauseMenu()
{
	if (!IsLocalController())
	{
		return;
	}
	if (PauseMenu)
	{
		PauseMenu->RemoveFromParent();
		PauseMenu = nullptr;
		UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(LoadPlayerSettings().VideoAndSound.FrameRateLimitGame);
		UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	}
}

void ABSPlayerController::ShowCrossHair()
{
	if (!IsLocalController())
	{
		return;
	}
	CrossHair = CreateWidget<UCrossHairWidget>(this, CrossHairClass);
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->GetPublicCrossHairSettingsChangedDelegate().AddUniqueDynamic(CrossHair, &UCrossHairWidget::OnPlayerSettingsChanged_CrossHair);
	CrossHair->AddToViewport();
}

void ABSPlayerController::HideCrossHair()
{
	if (CrossHair)
	{
		CrossHair->RemoveFromParent();
		CrossHair = nullptr;
	}
}

void ABSPlayerController::ShowPlayerHUD()
{
	if (!IsLocalController())
	{
		return;
	}
	PlayerHUD = CreateWidget<UPlayerHUD>(this, PlayerHUDClass);
	ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->UpdateScoresToHUD.AddUObject(PlayerHUD, &UPlayerHUD::UpdateAllElements);
	GameMode->OnSecondPassed.AddUObject(PlayerHUD, &UPlayerHUD::UpdateSongProgress);
	if (Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->BSConfig.DefiningConfig.BaseGameMode == EDefaultMode::BeatGrid )
	{
		PlayerHUD->Box_TargetsSpawned->SetVisibility(ESlateVisibility::Collapsed);
		PlayerHUD->Box_Streak->SetVisibility(ESlateVisibility::Collapsed);
		PlayerHUD->Box_TargetsHit->SetVisibility(ESlateVisibility::Collapsed);
		PlayerHUD->Box_ShotsFired->SetVisibility(ESlateVisibility::Collapsed);
	}
	PlayerHUD->AddToViewport();
	PlayerHUDActive = true;
}

void ABSPlayerController::HidePlayerHUD()
{
	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromParent();
		PlayerHUD = nullptr;
		PlayerHUDActive = false;
	}
	HideRLAgentWidget();
}

void ABSPlayerController::ShowCountdown()
{
	if (!IsLocalController())
	{
		return;
	}
	SetControlRotation(FRotator(0, 0, 0));
	if (GetPawn() != nullptr)
	{
		Cast<ABSCharacter>(GetPawn())->SetActorLocationAndRotation(FVector(1580, 0, 102), FRotator(0, 0, 0));
	}
	FadeScreenFromBlack();
	Countdown = CreateWidget<UCountdownWidget>(this, CountdownClass);
	Countdown->PlayerDelay = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->BSConfig.AudioConfig.PlayerDelay;
	ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	Countdown->OnCountdownCompleted.BindUObject(GameMode, &ABSGameMode::StartGameMode);
	Countdown->StartAAManagerPlayback.BindUObject(GameMode, &ABSGameMode::StartAAManagerPlayback);
	Countdown->AddToViewport();
	CountdownActive = true;
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(LoadPlayerSettings().VideoAndSound.FrameRateLimitGame);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ABSPlayerController::HideCountdown()
{
	if (!IsLocalController())
	{
		return;
	}
	if (Countdown)
	{
		Countdown->RemoveFromParent();
		Countdown = nullptr;
		CountdownActive = false;
	}
}

void ABSPlayerController::ShowPostGameMenu()
{
	if (!IsLocalController())
	{
		return;
	}
	
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	
	PostGameMenuWidget = CreateWidget<UPostGameMenuWidget>(this, PostGameMenuWidgetClass);
	PostGameMenuWidget->GameModesWidget->OnGameModeStateChanged.AddUObject(GI, &UBSGameInstance::HandleGameModeTransition);
	PostGameMenuWidget->QuitMenuWidget->OnGameModeStateChanged.AddUObject(GI, &UBSGameInstance::HandleGameModeTransition);

	GI->AddDelegateToOnPlayerSettingsChanged(PostGameMenuWidget->SettingsMenuWidget->GetGameDelegate());
	GI->AddDelegateToOnPlayerSettingsChanged(PostGameMenuWidget->SettingsMenuWidget->GetVideoAndSoundDelegate());
	GI->AddDelegateToOnPlayerSettingsChanged(PostGameMenuWidget->SettingsMenuWidget->GetCrossHairDelegate());
	GI->AddDelegateToOnPlayerSettingsChanged(PostGameMenuWidget->SettingsMenuWidget->GetAudioAnalyzerDelegate());
	GI->AddDelegateToOnPlayerSettingsChanged(PostGameMenuWidget->SettingsMenuWidget->GetUserDelegate());
	
	PostGameMenuWidget->AddToViewport();
	PostGameMenuActive = true;
	
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	SetPlayerEnabledState(false);
	
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(LoadPlayerSettings().VideoAndSound.FrameRateLimitMenu);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
}

void ABSPlayerController::OnPostScoresResponseReceived(const ELoginState& LoginState)
{
	if (!IsLocalController())
	{
		return;
	}
	if (!PostGameMenuWidget)
	{
		return;
	}
	PostGameMenuWidget->ScoresWidget->InitializePostGameScoringOverlay(LoginState);
	if (ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (GameMode->OnPostScoresResponse.IsBoundToObject(this))
		{
			GameMode->OnPostScoresResponse.RemoveAll(this);
		}
	}
}

void ABSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABSPlayerState* PS = GetPlayerState<ABSPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void ABSPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

ABSCharacter* ABSPlayerController::GetBSCharacter() const
{
	return Cast<ABSCharacter>(GetPawn());
}

void ABSPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ABSPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
	if (!IsLocalController() || !HasAuthority())
	{
		return;
	}
	GetBSAbilitySystemComponent()->ProcessAbilityInput(DeltaTime, IsPaused());
}

void ABSPlayerController::HidePostGameMenu()
{
	if (PostGameMenuWidget)
	{
		PostGameMenuWidget->RemoveFromParent();
		PostGameMenuWidget = nullptr;
		PostGameMenuActive = false;
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
		SetPlayerEnabledState(true);
	}
}

void ABSPlayerController::ShowFPSCounter()
{
	if (FPSCounter == nullptr)
	{
		FPSCounter = CreateWidget<UFPSCounterWidget>(this, FPSCounterClass);
		FPSCounter->AddToViewport(ZOrderFPSCounter);
	}
}

void ABSPlayerController::HideFPSCounter()
{
	if (FPSCounter)
	{
		FPSCounter->RemoveFromParent();
		FPSCounter = nullptr;
	}
}

void ABSPlayerController::FadeScreenToBlack()
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

void ABSPlayerController::FadeScreenFromBlack()
{
	if (!IsLocalController())
	{
		return;
	}
	if (!ScreenFadeWidget)
	{
		ScreenFadeWidget = CreateWidget<UScreenFadeWidget>(this, ScreenFadeClass);
		ScreenFadeWidget->AddToViewport(ZOrderFadeScreen);
	}
	ScreenFadeWidget->OnFadeFromBlackFinish.AddUObject(this, &ABSPlayerController::OnFadeScreenFromBlackFinish);
	ScreenFadeWidget->FadeFromBlack();
}

void ABSPlayerController::ShowInteractInfo()
{
	if (!IsLocalController())
	{
		return;
	}
	if (!InteractInfoWidget)
	{
		InteractInfoWidget = CreateWidget<UUserWidget>(this, InteractInfoWidgetClass);
		InteractInfoWidget->AddToViewport(ZOrderFPSCounter);
	}
}

void ABSPlayerController::HideInteractInfo()
{
	if (InteractInfoWidget)
	{
		InteractInfoWidget->RemoveFromParent();
		InteractInfoWidget = nullptr;
	}
}

void ABSPlayerController::ShowRLAgentWidget(FOnQTableUpdate& OnQTableUpdate, const int32 Rows, const int32 Columns, const TArray<float>& QTable)
{
	if (!IsLocalController())
	{
		return;
	}
	if (!RLAgentWidget)
	{
		RLAgentWidget = CreateWidget<URLAgentWidget>(this, RLAgentWidgetClass);
		OnQTableUpdate.AddUObject(RLAgentWidget, &URLAgentWidget::UpdatePanel);
		RLAgentWidget->InitQTable(Rows, Columns, QTable);
		RLAgentWidget->AddToViewport();
	}
}

void ABSPlayerController::HideRLAgentWidget()
{
	if (RLAgentWidget)
	{
		RLAgentWidget->RemoveFromParent();
		RLAgentWidget = nullptr;
	}
}

void ABSPlayerController::OnFadeScreenFromBlackFinish()
{
	if (ScreenFadeWidget)
	{
		ScreenFadeWidget->OnFadeFromBlackFinish.RemoveAll(this);
		ScreenFadeWidget->RemoveFromParent();
		ScreenFadeWidget = nullptr;
	}
}

void ABSPlayerController::OnPlayerSettingsChanged(const FPlayerSettings_VideoAndSound& PlayerSettings)
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
