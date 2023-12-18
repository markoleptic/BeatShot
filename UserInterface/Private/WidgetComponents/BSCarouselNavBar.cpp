// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSCarouselNavBar.h"
#include "WidgetComponents/Buttons/BSButton.h"
#include "CommonWidgetCarousel.h"
#include "WidgetComponents/Buttons/NotificationButtonCombo.h"

void UBSCarouselNavBar::SetLinkedCarousel(UCommonWidgetCarousel* CommonCarousel)
{
	if (LinkedCarousel) LinkedCarousel->OnCurrentPageIndexChanged.RemoveAll(this);
	
	LinkedCarousel = CommonCarousel;

	if (LinkedCarousel)
	{
		LinkedCarousel->OnCurrentPageIndexChanged.AddDynamic(this, &UBSCarouselNavBar::HandlePageChanged);
	}

	RebuildButtons();
}

void UBSCarouselNavBar::SetNavButtonText(const TArray<FText>& InButtonText)
{
	ButtonText = InButtonText;
}

void UBSCarouselNavBar::UpdateNotifications(const int32 Index, const int32 NumCautions, const int32 NumWarnings)
{
	if (!Buttons.IsValidIndex(Index)) return;
	
	UNotificationButtonCombo* Button = Cast<UNotificationButtonCombo>(Buttons[Index]);
	if (!Button) return;
	
	Button->SetNumWarnings(NumWarnings);
	Button->SetNumCautions(NumCautions);
}

void UBSCarouselNavBar::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	Buttons.Empty();
	MyContainer.Reset();
}

void UBSCarouselNavBar::HandlePageChanged(UCommonWidgetCarousel* CommonCarousel, int32 PageIndex)
{
	if (Buttons.IsValidIndex(PageIndex))
	{
		Buttons[PageIndex]->SetActive();
	}
}

void UBSCarouselNavBar::HandleButtonClicked(const UBSButton* AssociatedButton)
{
	if (LinkedCarousel && LinkedCarousel->GetActiveWidgetIndex() != AssociatedButton->GetEnumValue())
	{
		LinkedCarousel->EndAutoScrolling();
		const uint8 EnumValue = AssociatedButton->GetEnumValue();
		LinkedCarousel->SetActiveWidgetIndex(EnumValue);
	}
}

TSharedRef<SWidget> UBSCarouselNavBar::RebuildWidget()
{
	Buttons.Empty();
	MyContainer = SNew(SHorizontalBox);
	//RebuildButtons();
	return MyContainer.ToSharedRef();
}

void UBSCarouselNavBar::RebuildButtons()
{
	if (!MyContainer || !LinkedCarousel || !ButtonWidgetType) return;

	MyContainer->ClearChildren();
	
	const int32 NumPages = LinkedCarousel->GetChildrenCount();
	const int32 LastPage = NumPages - 1;

	if (NumPages <= 0) return;
	
	for (int32 CurPage = 0; CurPage < NumPages; CurPage++)
	{
		UBSButton* ButtonUserWidget = Cast<UBSButton>(CreateWidget(GetOwningPlayer(), ButtonWidgetType));
		if (!ButtonUserWidget) continue;

		Buttons.Add(ButtonUserWidget);
		TSharedRef<SWidget> ButtonSWidget = ButtonUserWidget->TakeWidget();
		
		if (ButtonText.IsValidIndex(CurPage))
		{
			ButtonUserWidget->SetButtonText(ButtonText[CurPage]);
		}

		FMargin CurrentPadding = Padding_Button;
		if (CurPage == 0) CurrentPadding.Left = 0.f;
		if (CurPage == LastPage) CurrentPadding.Right = 0.f;
		
		MyContainer->AddSlot()
			.HAlign(HAlign_Button).VAlign(VAlign_Button)
			.FillWidth(1.f).Padding(CurrentPadding)
			[ButtonSWidget];
	}
	
	
	for (int32 CurPage = 0; CurPage < LinkedCarousel->GetChildrenCount(); CurPage++)
	{
		const int32 NextPage = CurPage + 1;
		
		// Bind button clicking to function, link buttons together to deactivate when not clicked
		if (Buttons.IsValidIndex(CurPage))
		{
			Buttons[CurPage]->OnBSButtonPressed.AddDynamic(this, &UBSCarouselNavBar::HandleButtonClicked);
			
			if (Buttons.IsValidIndex(NextPage)) Buttons[CurPage]->SetDefaults(CurPage, Buttons[NextPage]);
			
			UNotificationButtonCombo* Button = Cast<UNotificationButtonCombo>(Buttons[CurPage]);
			if (!Button) continue;
			
			// Default to 0 warnings and cautions
			Button->SetNumCautions(0);
			Button->SetNumWarnings(0);
		}
	}

	Buttons.Last()->SetDefaults(static_cast<uint8>(Buttons.Num() - 1), Buttons[0]);
	Buttons[LinkedCarousel->GetActiveWidgetIndex()]->SetActive();
}
