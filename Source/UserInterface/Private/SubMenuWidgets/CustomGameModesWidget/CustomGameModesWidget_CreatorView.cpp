// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_CreatorView.h"
#include "CommonWidgetCarousel.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Activation.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Deactivation.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_General.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Preview.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_SpawnArea.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Spawning.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Start.h"
#include "WidgetComponents/BSCarouselNavBar.h"

void UCustomGameModesWidget_CreatorView::NativeConstruct()
{
	Super::NativeConstruct();
	AddChildWidget(Widget_Preview);
	
	Carousel->OnCurrentPageIndexChanged.AddUniqueDynamic(this, &ThisClass::OnCarouselWidgetIndexChanged);
	Carousel->SetActiveWidgetIndex(0);
	CarouselNavBar->SetNavButtonText(NavBarButtonText);
	CarouselNavBar->SetLinkedCarousel(Carousel);
}

void UCustomGameModesWidget_CreatorView::Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset)
{
	// Doesn't call parent function so it can set correct Next widget values in InitComponent
	
	BSConfig = InConfig;
	GameModeDataAsset = InGameModeDataAsset;
	
	Widget_Preview->InitComponent(BSConfig, nullptr);
	
	Widget_Start->InitComponent(BSConfig, Widget_General);
	Widget_General->InitComponent(BSConfig, Widget_SpawnArea);
	Widget_SpawnArea->InitComponent(BSConfig, Widget_Spawning);
	Widget_Spawning->InitComponent(BSConfig, Widget_Activation);
	Widget_Activation->InitComponent(BSConfig, Widget_Deactivation);
	Widget_Deactivation->InitComponent(BSConfig, Widget_Start);
}

void UCustomGameModesWidget_CreatorView::OnCarouselWidgetIndexChanged(UCommonWidgetCarousel* InCarousel, const int32 NewIndex)
{
	UpdateOptionsFromConfig();
}

void UCustomGameModesWidget_CreatorView::UpdateAllChildWidgetOptionsValid()
{
	Super::UpdateAllChildWidgetOptionsValid();
	int Index = 0;
	for (const TPair<TObjectPtr<UCustomGameModesWidgetComponent>, FCustomGameModeCategoryInfo*>& ChildWidgetValidity : ChildWidgetValidityMap)
	{
		if (ChildWidgetValidity.Key == Widget_Start)
		{
			Index++;
			continue;
		}
		CarouselNavBar->UpdateNotifications(Index, ChildWidgetValidity.Value->NumCautions, ChildWidgetValidity.Value->NumWarnings);
		Index++;
	}
}
