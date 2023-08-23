// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Preview.h"

void UCustomGameModesWidget_Preview::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_Preview::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UCustomGameModesWidget_Preview::UpdateAllOptionsValid()
{
	return Super::UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Preview::UpdateOptionsFromConfig()
{
	Super::UpdateOptionsFromConfig();
	UpdateImages(*BSConfig);
}
