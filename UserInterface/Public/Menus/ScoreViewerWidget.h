// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "ScoreViewerWidget.generated.h"

class UScoreTable;
class UTextBlock;
struct FAxisLabelOptions;
enum class EAxisType : uint8;
struct FBarChartData;
class UBSButton;
class UCustomGameModeScoreViewerWidget;
class UBarChartWidget;
class UDefaultGameModeScoreViewerWidget;
class UGameModeScoreViewerWidget;
class UVerticalBox;
class UWidgetSwitcher;
class UMenuButton;
struct FHeatMapAxisLabelOptions;
struct FHeatMapData;
class UHeatMapWidget;

struct FGameModePlayTime
{
	float PlayTime;
	EGameModeType GameModeType;
	FString CustomGameModeName;
	EBaseGameMode BaseGameMode;
};

UCLASS()
class USERINTERFACE_API UScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void LoadScores(USaveGamePlayerScore* InSaveGamePlayerScore, bool SwitchToMostRecent);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* Switcher;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Overview;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_History;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_DefaultModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_CustomModes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Overview;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UDefaultGameModeScoreViewerWidget* DefaultGameModeScoreViewerWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UCustomGameModeScoreViewerWidget* CustomGameModeScoreViewerWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScoreTable* ScoreTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_History;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_NoScores;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBarChartWidget* MostPlayedDefaultGameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBarChartWidget* MostPlayedCustomGameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UHeatMapWidget* PlayFrequency;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TotalTimeInAnyGameMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedDefaultMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedDefaultModeName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedCustomMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedCustomModeName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TimePlayedForMostPlayedDefaultMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TimePlayedForMostPlayedCustomMode;

	UPROPERTY(EditDefaultsOnly)
	TMap<EBaseGameMode, FText> BaseGameModeText;
	UPROPERTY(EditDefaultsOnly)
	TMap<EGameModeDifficulty, FText> DifficultyText;

	void OnButtonClicked_BSButton(const UBSButton* Button);

private:
	FText HandlePlayFrequencyDisplayText(int32, int32);

	FText HandlePlayFrequencyValueText(int32, int32, float);

	FText HandleMostPlayedDefaultGameModesDisplayText(int32);

	FText HandleMostPlayedDefaultGameModesValueText(int32, float);

	FText HandleMostPlayedCustomGameModesDisplayText(int32);

	FText HandleMostPlayedCustomGameModesValueText(int32, float);

	FText HandleMostPlayedDefaultGameModesXAxisFormatter(int32, float);

	FText HandleMostPlayedDefaultGameModesYAxisFormatter(int32, float);

	FText HandleMostPlayedCustomGameModesXAxisFormatter(int32, float);

	FText HandleMostPlayedCustomGameModesYAxisFormatter(int32, float);

	UPROPERTY()
	USaveGamePlayerScore* SaveGamePlayerScore;

	FDateTime StartDate;

	int32 StartDow;

	const FTextFormat LocationAccuracyDisplayFormat = FTextFormat::FromString("{0}\n{1} {2}");

	const FTextFormat LocationAccuracyValueFormat = FTextFormat::FromString("Total for week: {0} {1}");

	TSharedPtr<FHeatMapData> PlayFrequencyData;

	TSharedPtr<FHeatMapAxisLabelOptions> PlayFrequencyAxisData;

	TSharedPtr<FBarChartData> MostPlayedDefaultGameModesData;

	TSharedPtr<TMap<EAxisType, FAxisLabelOptions>> MostPlayedDefaultGameModesAxisData;

	TSharedPtr<FBarChartData> MostPlayedCustomGameModesData;

	TSharedPtr<TMap<EAxisType, FAxisLabelOptions>> MostPlayedCustomGameModesAxisData;

	TArray<FGameModePlayTime> DefaultGameModePlayTime;

	TArray<FGameModePlayTime> CustomGameModePlayTime;
};
