// Fill out your copyright notice in the Description page of Project Settings.


#include "QuitMenuWidget.h"

#include "DefaultGameMode.h"
#include "DefaultPlayerController.h"
#include "LoginWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UQuitMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
	QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeInSaveMenu);
	QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetGotoMainMenuTrue);
	QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetSaveMenuTitleMainMenu);

	QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
	QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeInSaveMenu);
	QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetGotoMainMenuFalse);
	QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetSaveMenuTitleDesktop);

	QuitBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
	QuitBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::InitializeExit);

	QuitAndSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutSaveMenu);
	QuitAndSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresTrue);
	QuitAndSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::Quit);

	QuitWithoutSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutSaveMenu);
	QuitWithoutSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresFalse);
	QuitWithoutSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::Quit);

	SaveBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutSaveMenu);
	SaveBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeInMenu);

	RestartAndSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresTrue);
	RestartAndSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::Restart);
	RestartWithoutSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresFalse);
	RestartWithoutSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::Restart);

	RestartBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutRestartMenu);
	RestartBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::InitializeExit);
}

void UQuitMenuWidget::Quit()
{
	if (bGotoMainMenu)
	{
		QuitToMainMenu();
		return;
	}
	QuitToDesktop();
}

void UQuitMenuWidget::QuitToMainMenu()
{
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Controller->OnScreenFadeToBlackFinish.AddDynamic(this, &UQuitMenuWidget::HandleQuitToMainMenu);
	Controller->FadeScreenToBlack();
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(bShouldSaveScores, false);
}

void UQuitMenuWidget::HandleQuitToMainMenu()
{
	Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->HidePauseMenu();
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
}

void UQuitMenuWidget::QuitToDesktop()
{
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(bShouldSaveScores, false);
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
								   EQuitPreference::Quit, false);
}

void UQuitMenuWidget::Restart()
{
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Controller->OnScreenFadeToBlackFinish.AddDynamic(this, &UQuitMenuWidget::HandleRestart);
	Controller->FadeScreenToBlack();
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(bShouldSaveScores, false);
}

void UQuitMenuWidget::HandleRestart()
{
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->InitializeGameMode();
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Controller->HandlePause();
	Controller->HidePauseMenu();
}

void UQuitMenuWidget::CollapseWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
	UnbindFromAnimationFinished(FadeOutBackgroundBlur, FadeOutWidgetDelegate);
}

void UQuitMenuWidget::InitializeExit()
{
	PlayAnimationForward(FadeOutBackgroundBlur);
	FadeOutWidgetDelegate.BindDynamic(this, & UQuitMenuWidget::CollapseWidget);
	BindToAnimationFinished(FadeOutBackgroundBlur, FadeOutWidgetDelegate);
	OnExitQuitMenu.Execute();
}
