// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/CGMW_Base.h"
#include "Blueprint/WidgetTree.h"
#include "SubMenuWidgets/GameModesWidgets/Components/CGMWC_Base.h"
#include "SubMenuWidgets/GameModesWidgets/Components/CGMWC_Start.h"

void UCGMW_Base::NativeConstruct()
{
	Super::NativeConstruct();
	
	Widget_Start->RequestGameModeTemplateUpdate.AddUObject(this, &ThisClass::OnRequestGameModeTemplateUpdate);
	Widget_Start->OnCustomGameModeNameChanged.AddUObject(this, &ThisClass::OnStartWidget_CustomGameModeNameChanged);
}

void UCGMW_Base::NativeDestruct()
{
	Super::NativeDestruct();
	BSConfig = nullptr;
}

UBSGameModeDataAsset* UCGMW_Base::GetGameModeDataAsset() const
{
	return GameModeDataAsset ? GameModeDataAsset.Get() : nullptr;
}

void UCGMW_Base::Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset)
{
	BSConfig = InConfig;
	GameModeDataAsset = InGameModeDataAsset;
	int32 Index = 0;

	WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (UCGMWC_Base* Component = Cast<UCGMWC_Base>(Widget))
		{
			const bool bIndexOnCarousel = Component->ShouldIndexOnCarousel();
			Component->InitComponent(InConfig, bIndexOnCarousel ? Index : -1);
			Component->RequestComponentUpdate.AddUObject(this, &ThisClass::OnRequestComponentUpdate);
			Component->RequestGameModePreviewUpdate.AddUObject(this, &ThisClass::OnRequestGameModePreviewUpdate);
			ChildWidgetValidityMap.FindOrAdd(Component) = Component->GetCustomGameModeCategoryInfo();
			if (bIndexOnCarousel) Index++;
		}
	});
}

void UCGMW_Base::UpdateOptionsFromConfig()
{
	for (const TPair<TObjectPtr<UCGMWC_Base>, FCustomGameModeCategoryInfo*>& ChildWidgetValidity :
	     ChildWidgetValidityMap)
	{
		if (const TObjectPtr<UCGMWC_Base> Component = ChildWidgetValidity.Key)
		{
			if (Component->IsInitialized())
			{
				Component->UpdateOptionsFromConfig();
			}
		}
	}
	UpdateAllChildWidgetOptionsValid();
}

void UCGMW_Base::UpdateAllChildWidgetOptionsValid()
{
	bool bAtLeastOneWarningPresent = false;
	uint8 TotalWarnings = 0;
	uint8 TotalCautions = 0;
	for (const TPair<TObjectPtr<UCGMWC_Base>, FCustomGameModeCategoryInfo*>& ChildWidgetValidity :
	     ChildWidgetValidityMap)
	{
		if (const TObjectPtr<UCGMWC_Base> Component = ChildWidgetValidity.Key)
		{
			if (Component->IsInitialized() && Component != Widget_Start)
			{
				Component->UpdateAllOptionsValid();
				TotalWarnings += ChildWidgetValidity.Value->NumWarnings;
				TotalCautions += ChildWidgetValidity.Value->NumCautions;
				if (ChildWidgetValidity.Value->NumWarnings > 0)
				{
					bAtLeastOneWarningPresent = true;
				}
				if (ChildWidgetValidity.Value->NumWarnings > 0 || ChildWidgetValidity.Value->NumCautions)
				{
					UE_LOG(LogTemp, Display, TEXT("%s has %d cautions and %d warnings"), *Component->GetName(),
						ChildWidgetValidity.Value->NumCautions, ChildWidgetValidity.Value->NumWarnings);
				}
			}
		}
	}
	UE_LOG(LogTemp, Display, TEXT("TotalWarnings: %d TotalCautions: %d"), TotalWarnings, TotalCautions);
	UpdateContainsGameModeBreakingOption(bAtLeastOneWarningPresent);
	RequestButtonStateUpdate.Broadcast();
}

FString UCGMW_Base::GetNewCustomGameModeName() const
{
	return Widget_Start->GetNewCustomGameModeName();
}

void UCGMW_Base::SetNewCustomGameModeName(const FString& InCustomGameModeName) const
{
	Widget_Start->SetNewCustomGameModeName(InCustomGameModeName);
}

FStartWidgetProperties UCGMW_Base::GetStartWidgetProperties() const
{
	return Widget_Start->GetStartWidgetProperties();
}

void UCGMW_Base::SetStartWidgetProperties(const FStartWidgetProperties& InProperties)
{
	Widget_Start->SetStartWidgetProperties(InProperties);
}

bool UCGMW_Base::GetAllNonStartChildWidgetOptionsValid() const
{
	for (const TPair<TObjectPtr<UCGMWC_Base>, FCustomGameModeCategoryInfo*>& ChildWidgetValidity :
	     ChildWidgetValidityMap)
	{
		if (ChildWidgetValidity.Key == Widget_Start)
		{
			continue;
		}
		if (!ChildWidgetValidity.Value->GetAllOptionsValid())
		{
			return false;
		}
	}
	return true;
}

void UCGMW_Base::RefreshGameModeTemplateComboBoxOptions() const
{
	Widget_Start->RefreshGameModeTemplateComboBoxOptions();
}

void UCGMW_Base::OnRequestGameModeTemplateUpdate(const FString& InGameMode,
	const EGameModeDifficulty& Difficulty)
{
	RequestGameModeTemplateUpdate.Broadcast(InGameMode, Difficulty);
}

void UCGMW_Base::OnStartWidget_CustomGameModeNameChanged()
{
	RequestButtonStateUpdate.Broadcast();
}

void UCGMW_Base::OnRequestComponentUpdate()
{
	if (!bIsUpdatingFromComponentRequest)
	{
		bIsUpdatingFromComponentRequest = true;
		UpdateAllChildWidgetOptionsValid();
		bIsUpdatingFromComponentRequest = false;
	}
}

void UCGMW_Base::OnRequestGameModePreviewUpdate()
{
	RequestGameModePreviewUpdate.Broadcast();
}

void UCGMW_Base::UpdateContainsGameModeBreakingOption(const bool bGameModeBreakingOptionPresent)
{
	if (bGameModeBreakingOptionPresent == bContainsGameModeBreakingOption)
	{
		return;
	}
	if (bGameModeBreakingOptionPresent)
	{
		UE_LOG(LogTemp, Display,
			TEXT("bContainsGameModeBreakingOption changed from false to true inside UCGMW_Base"));
	}
	else
	{
		UE_LOG(LogTemp, Display,
			TEXT("bContainsGameModeBreakingOption changed from true to false inside UCGMW_Base"));
	}
	bContainsGameModeBreakingOption = bGameModeBreakingOptionPresent;
	OnGameModeBreakingChange.Broadcast(bContainsGameModeBreakingOption);
}
