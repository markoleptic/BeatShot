// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "LoginWidget.h"
#include "SettingsMenuWidget.h"
#include "GameModesWidget.h"
#include "SlideRightButton.h"
#include "WebBrowserOverlay.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MainMenuWidgets.Add(PatchNotesButton, PatchNotes);
	MainMenuWidgets.Add(GameModesButton, GameModes);
	MainMenuWidgets.Add(ScoresButton, Scores);
	MainMenuWidgets.Add(SettingsButton, SettingsMenu);
	MainMenuWidgets.Add(FAQButton, FAQ);

	ScoresWidget->OnLoginStateChange.AddDynamic(this, &UMainMenuWidget::OnLoginStateChange);

	LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UMainMenuWidget::OnLoginButtonClicked);
	LoginWidget->OkayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnScoringButtonClicked);

	PatchNotesButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPatchNotesButtonClicked);
	GameModesButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnGameModesButtonClicked);
	ScoresButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnScoringButtonClicked);
	SettingsButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsButtonClicked);
	FAQButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnFAQButtonClicked);
	QuitButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);

	GitHubIssueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnGitHubButtonClicked);
	ScoresWidget->InitializeScoringOverlay();
	WebBrowserOverlayPatchNotes->BrowserWidget->LoadPatchNotesURL();
	WebBrowserOverlayPatchNotes->FadeOut();
}

void UMainMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ScoresWidget->BrowserWidget->ParentTickOverride(InDeltaTime);
}

void UMainMenuWidget::SlideButtons(const USlideRightButton* ActiveButton)
{
	for (TTuple<USlideRightButton*, UVerticalBox*>& Elem : MainMenuWidgets)
	{
		if (Elem.Key != ActiveButton)
		{
			Elem.Key->SlideButton(false);
			continue;
		}
		Elem.Key->SlideButton(true);
		MainMenuSwitcher->SetActiveWidget(Elem.Value);
	}
}

void UMainMenuWidget::OnPatchNotesButtonClicked()
{
	SlideButtons(PatchNotesButton);
}

void UMainMenuWidget::OnGameModesButtonClicked()
{
	SlideButtons(GameModesButton);
}

void UMainMenuWidget::OnScoringButtonClicked()
{
	SlideButtons(ScoresButton);
	if (bShowWebBrowserScoring) ScoresWidget->FadeOut();
}

void UMainMenuWidget::OnSettingsButtonClicked()
{
	SlideButtons(SettingsButton);
}

void UMainMenuWidget::OnFAQButtonClicked()
{
	SlideButtons(FAQButton);
}

void UMainMenuWidget::OnLoginButtonClicked(const FLoginPayload LoginPayload, const bool bIsPopup)
{
	ScoresWidget->LoginUserHttp(LoginPayload, bIsPopup);
	bShowWebBrowserScoring = true;
}

void UMainMenuWidget::OnGitHubButtonClicked()
{
	UKismetSystemLibrary::LaunchURL(GitHubURL);
}

void UMainMenuWidget::OnLoginStateChange(const bool bLoggedInHttp, const bool bLoggedInBrowser, const bool bIsPopup)
{
	if (!bLoggedInHttp)
	{
		LoginWidget->ShowRegisterScreen();
		UpdateLoginState(false);
		return;
	}
	if (!bLoggedInBrowser)
	{
		LoginWidget->ShowLoginScreen();
		UpdateLoginState(false);
		return;
	}

	bShowWebBrowserScoring = true;
	if (!bIsPopup)
	{
		UpdateLoginState(true);
		return;
	}
	LoginWidget->OnLoginSuccess();
	UpdateLoginState(true);
}

void UMainMenuWidget::UpdateLoginState(const bool bSuccessfulLogin)
{
	if (!bSuccessfulLogin)
	{
		SignInStateText->SetText(FText::FromString("Not Signed In"));
		UsernameText->SetText(FText());
		return;
	}
	SignInStateText->SetText(FText::FromString("Signed in as"));
	UsernameText->SetText(FText::FromString(
		Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->LoadPlayerSettings().Username));
}

void UMainMenuWidget::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
	                               EQuitPreference::Quit, false);
}
