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

UBSGameModeDataAsset* UCustomGameModesWidgetBase::GetGameModeDataAsset() const
{
	return GameModeDataAsset ? GameModeDataAsset.Get() : nullptr;
}

void UCustomGameModesWidgetBase::Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset)
{
	CurrentConfigPtr = InConfig;
	GameModeDataAsset = InGameModeDataAsset;
	
	for (const TPair<TObjectPtr<UCustomGameModesWidgetComponent>, bool>& ChildWidgetValidity : ChildWidgetValidityMap)
	{
		if (const TObjectPtr<UCustomGameModesWidgetComponent> Component = ChildWidgetValidity.Key)
		{
			Component->InitComponent(InConfig, nullptr);
		}
	}
}

void UCustomGameModesWidgetBase::Update()
{
	for (const TPair<TObjectPtr<UCustomGameModesWidgetComponent>, bool>& ChildWidgetValidity : ChildWidgetValidityMap)
	{
		if (const TObjectPtr<UCustomGameModesWidgetComponent> Component = ChildWidgetValidity.Key)
		{
			Component->UpdateOptionsFromConfig();
		}
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
			UE_LOG(LogTemp, Warning, TEXT("%s has an invalid setting!"), *ChildWidgetValidity.Key->GetFName().ToString());
			bReturnTrue = false;
		}
	}
	if (bReturnTrue)
	{
		UE_LOG(LogTemp, Warning, TEXT("No invalid settings!"));
	}
	return bReturnTrue ? true : false;
}

void UCustomGameModesWidgetBase::RefreshGameModeTemplateOptions() const
{
	Widget_Start->RefreshGameModeTemplateOptions();
}

void UCustomGameModesWidgetBase::OnRequestGameModeTemplateUpdate(const FString& InGameMode, const EGameModeDifficulty& Difficulty)
{
	RequestGameModeTemplateUpdate.Broadcast(InGameMode, Difficulty);
}

void UCustomGameModesWidgetBase::OnValidOptionsStateChanged(const TObjectPtr<UCustomGameModesWidgetComponent> Widget, const bool bAllOptionsValid)
{
	if (Widget == Widget_Start)
	{
		if (ChildWidgetValidityMap.FindRef(Widget) != bAllOptionsValid)
		{
			UE_LOG(LogTemp, Display, TEXT("RequestButtonStateUpdate"));
			ChildWidgetValidityMap.FindChecked(Widget) = bAllOptionsValid;
			RequestButtonStateUpdate.Broadcast();
			return;
		}
	}
	ChildWidgetValidityMap.FindChecked(Widget) = bAllOptionsValid;
}

void UCustomGameModesWidgetBase::AddChildWidget(const TObjectPtr<UCustomGameModesWidgetComponent> Component)
{
	ChildWidgetValidityMap.FindOrAdd(Component) = false;
	Component->RequestUpdateAfterConfigChange.AddUObject(this, &ThisClass::Update);
	Component->OnValidOptionsStateChanged.AddUObject(this, &ThisClass::OnValidOptionsStateChanged);
}