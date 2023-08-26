// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidgetBase.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidgetComponent.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Start.h"

void UCustomGameModesWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	AddChildWidget(Cast<UCustomGameModesWidgetComponent>(Widget_Start));
	AddChildWidget(Cast<UCustomGameModesWidgetComponent>(Widget_SpawnArea));
	AddChildWidget(Cast<UCustomGameModesWidgetComponent>(Widget_Activation));
	AddChildWidget(Cast<UCustomGameModesWidgetComponent>(Widget_Deactivation));
	AddChildWidget(Cast<UCustomGameModesWidgetComponent>(Widget_General));
	AddChildWidget(Cast<UCustomGameModesWidgetComponent>(Widget_Spawning));
	AddChildWidget(Cast<UCustomGameModesWidgetComponent>(Widget_Target));

	Widget_Start->RequestGameModeTemplateUpdate.AddUObject(this, &ThisClass::OnRequestGameModeTemplateUpdate);
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
	
	for (const TPair<TObjectPtr<UCustomGameModesWidgetComponent>, bool>& ChildWidgetValidity : ChildWidgetValidityMap)
	{
		if (const TObjectPtr<UCustomGameModesWidgetComponent> Component = ChildWidgetValidity.Key)
		{
			Component->InitComponent(InConfig, nullptr);
		}
	}
}

void UCustomGameModesWidgetBase::UpdateOptionsFromConfig()
{
	for (const TPair<TObjectPtr<UCustomGameModesWidgetComponent>, bool>& ChildWidgetValidity : ChildWidgetValidityMap)
	{
		if (const TObjectPtr<UCustomGameModesWidgetComponent> Component = ChildWidgetValidity.Key)
		{
			if (Component->IsInitialized())
			{
				Component->UpdateOptionsFromConfig();
			}
		}
	}
	if (bShouldUpdateFromComponentRequest && !bIsUpdatingFromComponentRequest)
	{
		bIsUpdatingFromComponentRequest = true;
		UpdateOptionsFromConfig();
		bIsUpdatingFromComponentRequest = false;
	}
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

bool UCustomGameModesWidgetBase::GetAllChildWidgetOptionsValid() const
{
	bool bReturnTrue = true;
	for (const TPair<TObjectPtr<UCustomGameModesWidgetComponent>, bool>& ChildWidgetValidity : ChildWidgetValidityMap)
	{
		if (ChildWidgetValidity.Value == false)
		{
			UE_LOG(LogTemp, Display, TEXT("%s has an invalid setting."), *ChildWidgetValidity.Key->GetFName().ToString());
			bReturnTrue = false;
		}
	}
	if (bReturnTrue)
	{
		UE_LOG(LogTemp, Display, TEXT("No invalid settings."));
	}
	return bReturnTrue ? true : false;
}

void UCustomGameModesWidgetBase::RefreshGameModeTemplateComboBoxOptions() const
{
	Widget_Start->RefreshGameModeTemplateComboBoxOptions();
}

void UCustomGameModesWidgetBase::OnRequestGameModeTemplateUpdate(const FString& InGameMode, const EGameModeDifficulty& Difficulty)
{
	RequestGameModeTemplateUpdate.Broadcast(InGameMode, Difficulty);
}

void UCustomGameModesWidgetBase::OnValidOptionsStateChanged(const TObjectPtr<UCustomGameModesWidgetComponent> Widget, const bool bAllOptionsValid)
{
	const bool bOldAllOptionsValid = ChildWidgetValidityMap.FindRef(Widget);
	ChildWidgetValidityMap.FindChecked(Widget) = bAllOptionsValid;
	if (bOldAllOptionsValid != bAllOptionsValid)
	{
		const FString OldString = bOldAllOptionsValid ? "True" : "False";
		const FString NewString = bOldAllOptionsValid ? "False" : "True";
		UE_LOG(LogTemp, Display, TEXT("OnValidOptionsStateChanged %s changing from %s to %s"), *Widget->GetName(), *OldString, *NewString);
		UpdateOptionsFromConfig();
		RequestButtonStateUpdate.Broadcast();
	}
}

void UCustomGameModesWidgetBase::OnRequestComponentUpdate()
{
	if (!bIsUpdatingFromComponentRequest)
	{
		bShouldUpdateFromComponentRequest = true;
	}
}

void UCustomGameModesWidgetBase::AddChildWidget(const TObjectPtr<UCustomGameModesWidgetComponent> Component)
{
	ChildWidgetValidityMap.FindOrAdd(Component) = false;
	Component->RequestComponentUpdate.AddUObject(this, &ThisClass::OnRequestComponentUpdate);
	Component->OnValidOptionsStateChanged.AddUObject(this, &ThisClass::OnValidOptionsStateChanged);
}
