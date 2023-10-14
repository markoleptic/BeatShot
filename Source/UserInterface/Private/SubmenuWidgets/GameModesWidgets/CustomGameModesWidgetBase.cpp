// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/CustomGameModesWidgetBase.h"
#include "Blueprint/WidgetTree.h"
#include "SubMenuWidgets/GameModesWidgets/Components/CustomGameModesWidgetComponent.h"
#include "SubMenuWidgets/GameModesWidgets/Components/CustomGameModesWidget_Start.h"

void UCustomGameModesWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		if (UCustomGameModesWidgetComponent* Component = Cast<UCustomGameModesWidgetComponent>(Widget))
		{
			AddChildWidget(Cast<UCustomGameModesWidgetComponent>(Component));
		}
	});

	Widget_Start->RequestGameModeTemplateUpdate.AddUObject(this, &ThisClass::OnRequestGameModeTemplateUpdate);
	Widget_Start->OnCustomGameModeNameChanged.AddUObject(this, &ThisClass::OnStartWidget_CustomGameModeNameChanged);
}

void UCustomGameModesWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();
	BSConfig = nullptr;
}

UBSGameModeDataAsset* UCustomGameModesWidgetBase::GetGameModeDataAsset() const
{
	return GameModeDataAsset ? GameModeDataAsset.Get() : nullptr;
}

void UCustomGameModesWidgetBase::Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset)
{
	BSConfig = InConfig;
	GameModeDataAsset = InGameModeDataAsset;

	for (const TPair<TObjectPtr<UCustomGameModesWidgetComponent>, FCustomGameModeCategoryInfo*>& ChildWidgetValidity :
	     ChildWidgetValidityMap)
	{
		if (const TObjectPtr<UCustomGameModesWidgetComponent> Component = ChildWidgetValidity.Key)
		{
			Component->InitComponent(InConfig);
		}
	}
}

void UCustomGameModesWidgetBase::UpdateOptionsFromConfig()
{
	for (const TPair<TObjectPtr<UCustomGameModesWidgetComponent>, FCustomGameModeCategoryInfo*>& ChildWidgetValidity :
	     ChildWidgetValidityMap)
	{
		if (const TObjectPtr<UCustomGameModesWidgetComponent> Component = ChildWidgetValidity.Key)
		{
			if (Component->IsInitialized())
			{
				Component->UpdateOptionsFromConfig();
			}
		}
	}
	UpdateAllChildWidgetOptionsValid();
}

void UCustomGameModesWidgetBase::UpdateAllChildWidgetOptionsValid()
{
	bool bAtLeastOneWarningPresent = false;
	uint8 TotalWarnings = 0;
	uint8 TotalCautions = 0;
	for (const TPair<TObjectPtr<UCustomGameModesWidgetComponent>, FCustomGameModeCategoryInfo*>& ChildWidgetValidity :
	     ChildWidgetValidityMap)
	{
		if (const TObjectPtr<UCustomGameModesWidgetComponent> Component = ChildWidgetValidity.Key)
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

FString UCustomGameModesWidgetBase::GetNewCustomGameModeName() const
{
	return Widget_Start->GetNewCustomGameModeName();
}

void UCustomGameModesWidgetBase::SetNewCustomGameModeName(const FString& InCustomGameModeName) const
{
	Widget_Start->SetNewCustomGameModeName(InCustomGameModeName);
}

FStartWidgetProperties UCustomGameModesWidgetBase::GetStartWidgetProperties() const
{
	return Widget_Start->GetStartWidgetProperties();
}

void UCustomGameModesWidgetBase::SetStartWidgetProperties(const FStartWidgetProperties& InProperties)
{
	Widget_Start->SetStartWidgetProperties(InProperties);
}

bool UCustomGameModesWidgetBase::GetAllNonStartChildWidgetOptionsValid() const
{
	for (const TPair<TObjectPtr<UCustomGameModesWidgetComponent>, FCustomGameModeCategoryInfo*>& ChildWidgetValidity :
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

void UCustomGameModesWidgetBase::RefreshGameModeTemplateComboBoxOptions() const
{
	Widget_Start->RefreshGameModeTemplateComboBoxOptions();
}

void UCustomGameModesWidgetBase::OnRequestGameModeTemplateUpdate(const FString& InGameMode,
	const EGameModeDifficulty& Difficulty)
{
	RequestGameModeTemplateUpdate.Broadcast(InGameMode, Difficulty);
}

void UCustomGameModesWidgetBase::OnStartWidget_CustomGameModeNameChanged()
{
	RequestButtonStateUpdate.Broadcast();
}

void UCustomGameModesWidgetBase::OnRequestComponentUpdate()
{
	if (!bIsUpdatingFromComponentRequest)
	{
		bIsUpdatingFromComponentRequest = true;
		UpdateAllChildWidgetOptionsValid();
		bIsUpdatingFromComponentRequest = false;
	}
}

void UCustomGameModesWidgetBase::OnRequestGameModePreviewUpdate()
{
	RequestGameModePreviewUpdate.Broadcast();
}

void UCustomGameModesWidgetBase::AddChildWidget(const TObjectPtr<UCustomGameModesWidgetComponent> Component)
{
	ChildWidgetValidityMap.FindOrAdd(Component) = Component->GetCustomGameModeCategoryInfo();
	Component->RequestComponentUpdate.AddUObject(this, &ThisClass::OnRequestComponentUpdate);
	Component->RequestGameModePreviewUpdate.AddUObject(this, &ThisClass::OnRequestGameModePreviewUpdate);
}

void UCustomGameModesWidgetBase::UpdateContainsGameModeBreakingOption(const bool bGameModeBreakingOptionPresent)
{
	if (bGameModeBreakingOptionPresent == bContainsGameModeBreakingOption)
	{
		return;
	}
	if (bGameModeBreakingOptionPresent)
	{
		UE_LOG(LogTemp, Display,
			TEXT("bContainsGameModeBreakingOption changed from false to true inside UCustomGameModesWidgetBase"));
	}
	else
	{
		UE_LOG(LogTemp, Display,
			TEXT("bContainsGameModeBreakingOption changed from true to false inside UCustomGameModesWidgetBase"));
	}
	bContainsGameModeBreakingOption = bGameModeBreakingOptionPresent;
	OnGameModeBreakingChange.Broadcast(bContainsGameModeBreakingOption);
}
