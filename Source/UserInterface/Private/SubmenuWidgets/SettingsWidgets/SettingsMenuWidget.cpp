// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsWidgets/SettingsMenuWidget.h"
#include "CommonWidgetCarousel.h"
#include "SubMenuWidgets/SettingsWidgets/SettingsMenuWidget_AudioAnalyzer.h"
#include "WidgetComponents/BSCarouselNavBar.h"

void USettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AudioAnalyzer_Widget->OnRestartButtonClicked.BindUFunction(this, "OnRestartButtonClicked_AudioAnalyzer");

	Carousel->OnCurrentPageIndexChanged.AddUniqueDynamic(this, &ThisClass::OnCarouselWidgetIndexChanged);
	Carousel->SetActiveWidgetIndex(0);
	CarouselNavBar->SetNavButtonText(NavBarButtonText);
	CarouselNavBar->SetLinkedCarousel(Carousel);

	if (bIsPauseMenuChild)
	{
		AudioAnalyzer_Widget->InitPauseMenuChild();
	}
}

void USettingsMenuWidget::OnRestartButtonClicked_AudioAnalyzer() const
{
	if (!OnRestartButtonClicked.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("OnRestartButtonClicked not bound."));
	}
}

void USettingsMenuWidget::OnCarouselWidgetIndexChanged(UCommonWidgetCarousel* InCarousel, const int32 NewIndex)
{
}
