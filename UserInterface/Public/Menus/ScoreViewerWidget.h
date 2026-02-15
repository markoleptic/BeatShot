// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreViewerWidget.generated.h"


struct FHeatMapAxisLabelOptions;
struct FHeatMapData;
class UHeatMapWidget;
class USaveGamePlayerScore;

UCLASS()
class USERINTERFACE_API UScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void LoadScores(USaveGamePlayerScore* SaveGamePlayerScore);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UHeatMapWidget* PlayFrequency;

private:
	TSharedPtr<FHeatMapData> PlayFrequencyData;

	TSharedPtr<FHeatMapAxisLabelOptions> PlayFrequencyAxisData;
};
