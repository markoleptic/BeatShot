// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/VideoSettingButton.h"
#include "Components/Button.h"
#include <Blueprint/UserWidget.h>
#include "GlobalConstants.h"

void UVideoSettingButton::InitVideoSettingButton(const EVideoSettingType& InSettingType, const int32 InQuality, const TObjectPtr<UVideoSettingButton>& InNext)
{
	Quality = InQuality;
	SettingType = InSettingType;
	Next = InNext;
}

UVideoSettingButton* UVideoSettingButton::GetVideoSettingButtonFromQuality(const int32 InQuality)
{
	TObjectPtr<UVideoSettingButton> Head = this;
	while (Head->Quality != InQuality)
	{
		Head = Head->Next;
	}
	return Head.Get();
}

void UVideoSettingButton::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnClicked.AddUniqueDynamic(this, &UVideoSettingButton::OnButtonClickedCallback);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UVideoSettingButton::OnButtonClickedCallback()
{
	OnButtonClicked.Broadcast(this);
}

void UVideoSettingButton::SetButtonSettingCategoryBackgroundColors() const
{
	Button->SetBackgroundColor(Constants::BeatShotBlue);
	TObjectPtr<UVideoSettingButton> Head = Next;

	while (Head->Quality != Quality)
	{
		Head->Button->SetBackgroundColor(FLinearColor::White);
		Head = Head->Next;
	}
}
