// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEnums.h"
#include "GlobalStructs.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "GameModesWidget_BeatTrackConfig.generated.h"

class UDoubleSyncedSliderAndTextBox;
class UBorder;

/** SettingCategoryWidget for the GameModesWidget that holds BeatTrack configuration settings */
UCLASS()
class USERINTERFACE_API UGameModesWidget_BeatTrackConfig : public UBSSettingCategoryWidget
{
	GENERATED_BODY()

	friend class UGameModesWidget;
	virtual void NativeConstruct() override;
	virtual void InitSettingCategoryWidget() override;
	
public:
	void InitializeBeatTrackConfig(const FBS_BeatTrackConfig& InBeatTrackConfig, const EDefaultMode& BaseGameMode);
	FBS_BeatTrackConfig GetBeatTrackConfig() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Target Spread")
	TSubclassOf<UDoubleSyncedSliderAndTextBox> TargetSpeedConstrainedClass;
	
	TSoftObjectPtr<UDoubleSyncedSliderAndTextBox> TargetSpeedConstrained;
};
