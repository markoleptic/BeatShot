// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/CGMW_CreatorView.h"
#include "SubMenuWidgets/GameModesWidgets/Components/CGMWC_Base.h"
#include "SubMenuWidgets/GameModesWidgets/Components/CGMWC_Start.h"
#include "WidgetComponents/BSCarouselNavBar.h"
#include "CommonWidgetCarousel.h"

void UCGMW_CreatorView::NativeConstruct()
{
	Super::NativeConstruct();

	Carousel->OnCurrentPageIndexChanged.AddUniqueDynamic(this, &ThisClass::OnCarouselWidgetIndexChanged);
	Carousel->SetActiveWidgetIndex(0);
	CarouselNavBar->SetNavButtonText(NavBarButtonText);
	CarouselNavBar->SetLinkedCarousel(Carousel);
}

void UCGMW_CreatorView::OnCarouselWidgetIndexChanged(UCommonWidgetCarousel* InCarousel,
	const int32 NewIndex)
{
	UpdateOptionsFromConfig();
}

void UCGMW_CreatorView::UpdateAllChildWidgetOptionsValid()
{
	Super::UpdateAllChildWidgetOptionsValid();
	for (const TPair<TObjectPtr<UCGMWC_Base>, FCustomGameModeCategoryInfo*>& ChildWidgetValidity :
	     ChildWidgetValidityMap)
	{
		// Widget_Start has a separate validity check
		if (ChildWidgetValidity.Key == Widget_Start) continue;
		CarouselNavBar->UpdateNotifications(ChildWidgetValidity.Key->GetIndex(), ChildWidgetValidity.Value->NumCautions,
			ChildWidgetValidity.Value->NumWarnings);
	}
}
