// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuWidget.h"

#include "DefaultPlayerController.h"
#include "SaveQuitMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "SlideRightButton.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ResumeButton->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeButtonClicked);
	QuitButton->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitButtonClicked);
	PauseMenuWidgets.Add(ResumeButton, PauseScreen);
	PauseMenuWidgets.Add(SettingsButton, SettingsMenu);
	PauseMenuWidgets.Add(FAQButton, FAQ);

	FadeInWidget();
}

void UPauseMenuWidget::OnResumeButtonClicked()
{
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Controller->HandlePause();
	Controller->HidePauseMenu();
}

void UPauseMenuWidget::OnQuitMainMenuButtonClicked()
{
	/** TODO: QuitMenu stuff*/
}

void UPauseMenuWidget::OnQuitDesktopButtonClicked()
{
	/** TODO: QuitMenu stuff*/
}

void UPauseMenuWidget::OnQuitButtonClicked()
{
	SaveQuitMenuWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	/** TODO: QuitMenu stuff*/
}

void UPauseMenuWidget::SlideButtons(const USlideRightButton* ActiveButton)
{
	for (TTuple<USlideRightButton*, UVerticalBox*>& Elem : PauseMenuWidgets)
	{
		if (Elem.Key != ActiveButton)
		{
			Elem.Key->SlideButton(false);
			continue;
		}
		Elem.Key->SlideButton(true);
		PauseMenuSwitcher->SetActiveWidget(Elem.Value);
	}
}
