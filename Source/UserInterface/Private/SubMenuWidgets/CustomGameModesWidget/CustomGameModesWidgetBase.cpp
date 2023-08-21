// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidgetBase.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidgetComponent.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Start.h"

void UCustomGameModesWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	AddComponent(Cast<UCustomGameModesWidgetComponent>(Widget_Start));
	AddComponent(Cast<UCustomGameModesWidgetComponent>(Widget_SpawnArea));
	AddComponent(Cast<UCustomGameModesWidgetComponent>(Widget_Activation));
	AddComponent(Cast<UCustomGameModesWidgetComponent>(Widget_Deactivation));
	AddComponent(Cast<UCustomGameModesWidgetComponent>(Widget_General));
	AddComponent(Cast<UCustomGameModesWidgetComponent>(Widget_Spawning));
	AddComponent(Cast<UCustomGameModesWidgetComponent>(Widget_Target));

	Widget_Start->RequestGameModeTemplateUpdate.AddUObject(this, &ThisClass::OnRequestGameModeTemplateUpdate);
}

UBSGameModeDataAsset* UCustomGameModesWidgetBase::GetGameModeDataAsset() const
{
	if (GameModeDataAsset)
	{
		return GameModeDataAsset.Get();
	}
	return nullptr;
}

void UCustomGameModesWidgetBase::OnRequestGameModeTemplateUpdate(const FString& InGameMode, const EGameModeDifficulty& Difficulty)
{
	RequestGameModeTemplateUpdate.Broadcast(InGameMode, Difficulty);
}

void UCustomGameModesWidgetBase::AddComponent(const TObjectPtr<UCustomGameModesWidgetComponent> Component)
{
	Components.AddUnique(Component);
	Component->RequestUpdateAfterConfigChange.AddUObject(this, &ThisClass::UpdateAfterConfigChange);
}

void UCustomGameModesWidgetBase::Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset)
{
	CurrentConfigPtr = InConfig;
	GameModeDataAsset = InGameModeDataAsset;
	for (const TObjectPtr<UCustomGameModesWidgetComponent> Component : Components)
	{
		Component->InitComponent(InConfig, nullptr);
	}
}

void UCustomGameModesWidgetBase::Update()
{
	for (const TObjectPtr<UCustomGameModesWidgetComponent> Component : Components)
	{
		Component->UpdateOptions();
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

void UCustomGameModesWidgetBase::UpdateAfterConfigChange()
{
	Update();
}


