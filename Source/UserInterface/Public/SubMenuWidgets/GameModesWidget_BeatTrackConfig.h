// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "GameModesWidget_BeatTrackConfig.generated.h"

class UDoubleSyncedSliderAndTextBox;
class UBorder;

/** SettingCategoryWidget for the GameModesWidget that holds BeatTrack configuration settings */
UCLASS()
class USERINTERFACE_API UGameModesWidget_BeatTrackConfig : public UBSSettingCategoryWidget
{
	GENERATED_BODY()
};
