// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModesWidget.h"
#include "DefaultGameInstance.h"
#include "SaveGameCustomGameMode.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/ComboBoxString.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DefaultValueHelper.h"

void UGameModesWidget::NativeConstruct()
{
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));

	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::Custom, EGameModeDifficulty::Normal));
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::BeatGrid, EGameModeDifficulty::Normal));
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::BeatTrack, EGameModeDifficulty::Normal));
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::SingleBeat, EGameModeDifficulty::Normal, ESpreadType::DynamicEdgeOnly));
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::MultiBeat, EGameModeDifficulty::Normal, ESpreadType::DynamicRandom));

	GameModesToDisplay = GameModeActorDefaults;

	LoadCustomGameModes();
	PopulateGameModeSettings();

	SpawnHeightSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpawnAreaConstrained);
	SpawnWidthSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpawnAreaConstrained);

	BeatGridHorizontalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpacingConstrained);
	BeatGridVerticalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpacingConstrained);

	MinTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridTargetSizeConstrained);
	MaxTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridTargetSizeConstrained);

	MaxNumBeatGridTargetsComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::BeatGridNumberOfTargetsConstrained);
}

FGameModeActorStruct UGameModesWidget::GameModeActorStructConstructor(EGameModeActorName GameModeActor,
	EGameModeDifficulty NewGameModeDifficulty, ESpreadType NewSpreadType)
{
	return FGameModeActorStruct(GameModeActor, NewGameModeDifficulty, NewSpreadType);
}

void UGameModesWidget::SaveCustomGameMode()
{
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModesMap = CustomGameModesMap;
		if (UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveCustomGameModes Succeeded"));
		}
	}
}

void UGameModesWidget::ResetCustomGameMode()
{
	CustomGameMode.ResetStruct();
	PopulateGameModeSettings();
}

TMap<FString, FGameModeActorStruct> UGameModesWidget::LoadCustomGameModes()
{
	USaveGameCustomGameMode* SaveGameCustomGameMode;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("CustomGameModesSlot"), 3))
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(UGameplayStatics::LoadGameFromSlot(TEXT("CustomGameModesSlot"), 3));
	}
	else
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass()));
	}
	if (SaveGameCustomGameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Custom Game Modes loaded to Game Modes Widget"));
		CustomGameModesMap = SaveGameCustomGameMode->CustomGameModesMap;
		return SaveGameCustomGameMode->CustomGameModesMap;
	}
	return TMap<FString, FGameModeActorStruct>();
}

void UGameModesWidget::BeatGridTargetSizeConstrained(float value)
{
	if (GameModeCategoryComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}
	const float Width = round(SpawnWidthSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(SpawnHeightSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = MaxNumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	if (TargetWidth >= (Width - 200 - HSpacing * MaxTargets + HSpacing) / MaxTargets)
	{
		UE_LOG(LogTemp, Display, TEXT("Scale width contraint %f"), TargetWidth);
	}
	if (TargetHeight >= (Height - 200 - VSpacing * MaxTargets + VSpacing) / MaxTargets)
	{
		UE_LOG(LogTemp, Display, TEXT("Scale height contraint %f"), TargetHeight);
	}
}

void UGameModesWidget::BeatGridSpawnAreaConstrained(float value)
{
	if (GameModeCategoryComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}

	const float Width = round(SpawnWidthSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(SpawnHeightSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = MaxNumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	if (Width <= TargetWidth * MaxTargets + HSpacing * MaxTargets - HSpacing + 200)
	{
		UE_LOG(LogTemp, Display, TEXT("Width contraint"));
	}
	if (Height <= TargetHeight * MaxTargets + VSpacing * MaxTargets - VSpacing + 200)
	{
		UE_LOG(LogTemp, Display, TEXT("Height contraint"));
	}
}

void UGameModesWidget::BeatGridNumberOfTargetsConstrained(FString SelectedSong, ESelectInfo::Type SelectionType)
{
	if (GameModeCategoryComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}

	const float Width = round(SpawnWidthSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(SpawnHeightSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = MaxNumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	// WidthORHeight = TargetScale*MaxTargets + Spacing*MaxTargets - Spacing + 200

	if (MaxTargets >= (Width - 200 - HSpacing * MaxTargets + HSpacing) / TargetWidth)
	{
		UE_LOG(LogTemp, Display, TEXT("MaxTargets height contraint"));
	}
	if (MaxTargets >= (Height - 200 - VSpacing * MaxTargets + VSpacing) / TargetHeight)
	{
		UE_LOG(LogTemp, Display, TEXT("MaxTargets width contraint"));
	}
}

void UGameModesWidget::BeatGridSpacingConstrained(float value)
{
	if (GameModeCategoryComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}

	const float Width = round(SpawnWidthSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(SpawnHeightSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = MaxNumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	if (HSpacing >= (Width - TargetWidth * MaxTargets - 200) / (MaxTargets - 1))
	{
		UE_LOG(LogTemp, Display, TEXT("Spacing width contraint %f"), HSpacing);
	}
	if (VSpacing >= (Height - TargetHeight * MaxTargets - 200) / (MaxTargets - 1))
	{
		UE_LOG(LogTemp, Display, TEXT("Spacing height contraint %f"), VSpacing);
	}
}

bool UGameModesWidget::CheckAllBeatGridConstraints()
{
	if (GameModeCategoryComboBox->GetSelectedOption() != "Beat Grid")
	{
		return true;
	}

	const float Width = round(SpawnWidthSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(SpawnHeightSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = MaxNumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	if (
		TargetWidth >= ((Width - 200 - HSpacing * MaxTargets + HSpacing) / MaxTargets) ||
		TargetHeight >= ((Height - 200 - VSpacing * MaxTargets + VSpacing) / MaxTargets) ||
		Width <= (TargetWidth * MaxTargets + HSpacing * MaxTargets - HSpacing + 200) ||
		Height <= (TargetHeight * MaxTargets + VSpacing * MaxTargets - VSpacing + 200) ||
		MaxTargets >= ((Width - 200 - HSpacing * MaxTargets + HSpacing) / TargetWidth) ||
		MaxTargets >= ((Height - 200 - VSpacing * MaxTargets + VSpacing) / TargetHeight) ||
		HSpacing >= ((Width - TargetWidth * MaxTargets - 200) / (MaxTargets - 1)) ||
		VSpacing >= ((Height - TargetHeight * MaxTargets - 200) / (MaxTargets - 1))
		)
	{
		return false;
	}
	return true;
}