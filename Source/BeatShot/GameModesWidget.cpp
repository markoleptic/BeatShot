// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModesWidget.h"
#include "DefaultGameInstance.h"
#include "SaveGameCustomGameMode.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/ComboBoxString.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DefaultValueHelper.h"

void UGameModesWidget::NativeOnInitialized()
{
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));

	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::Custom, EGameModeDifficulty::Normal));
	GameModeActorDefaults.
		Add(GameModeActorStructConstructor(EGameModeActorName::BeatGrid, EGameModeDifficulty::Normal));
	GameModeActorDefaults.Add(
		GameModeActorStructConstructor(EGameModeActorName::BeatTrack, EGameModeDifficulty::Normal));
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::SingleBeat,
	                                                         EGameModeDifficulty::Normal,
	                                                         ESpreadType::DynamicEdgeOnly));
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::MultiBeat, EGameModeDifficulty::Normal,
	                                                         ESpreadType::DynamicRandom));

	GameModesToDisplay = GameModeActorDefaults;

	SpreadSelect->SetVisibility(ESlateVisibility::Collapsed);

	PlayFromStandard->SetIsEnabled(false);
	CustomizeFromStandard->SetIsEnabled(false);

	SaveCustom->SetIsEnabled(false);
	SaveCustomAndStart->SetIsEnabled(false);
	StartCustom->SetIsEnabled(false);

	LoadCustomGameModes();

	if (CustomGameModesMap.IsEmpty())
	{
		RemoveAllCustom->SetIsEnabled(false);
	}
	else
	{
		RemoveAllCustom->SetIsEnabled(false);
	}
	RemoveSelectedCustom->SetIsEnabled(false);

	CustomGameModeETB->OnTextChanged.AddDynamic(this, &UGameModesWidget::ChangeSaveButtonStates);
	GameModeNameComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::UpdateCustomGameModeOptions);
	SpawnHeightSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpawnAreaConstrained);
	SpawnWidthSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpawnAreaConstrained);
	BeatGridHorizontalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpacingConstrained);
	BeatGridVerticalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpacingConstrained);
	MinTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridTargetSizeConstrained);
	MaxTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridTargetSizeConstrained);
	MaxNumBeatGridTargetsComboBox->OnSelectionChanged.AddDynamic(
		this, &UGameModesWidget::BeatGridNumberOfTargetsConstrained);

	PopulateGameModeNameComboBox("Custom");
}

void UGameModesWidget::NativeConstruct()
{
}

FGameModeActorStruct UGameModesWidget::GameModeActorStructConstructor(EGameModeActorName GameModeActor,
                                                                      EGameModeDifficulty NewGameModeDifficulty,
                                                                      ESpreadType NewSpreadType)
{
	return FGameModeActorStruct(GameModeActor, NewGameModeDifficulty, NewSpreadType);
}

void UGameModesWidget::SaveCustomGameMode()
{
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(
		UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModesMap = CustomGameModesMap;
		if (UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveCustomGameModes Succeeded"));
		}
	}
}

TMap<FString, FGameModeActorStruct> UGameModesWidget::LoadCustomGameModes()
{
	USaveGameCustomGameMode* SaveGameCustomGameMode;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("CustomGameModesSlot"), 3))
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(
			UGameplayStatics::LoadGameFromSlot(TEXT("CustomGameModesSlot"), 3));
	}
	else
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(
			UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass()));
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

void UGameModesWidget::UpdateCustomGameModeOptions(FString SelectedGameModeActorName, ESelectInfo::Type SelectionType)
{
	if (!(SelectionType == ESelectInfo::Direct))
	{
		const FGameModeActorStruct FoundGameMode = FindGameModeFromString(SelectedGameModeActorName);
		SelectedGameMode = FoundGameMode.GameModeActorName;
		PopulateGameModeOptions(FindGameModeFromString(SelectedGameModeActorName));
	}

	if (IsCustomGameMode(SelectedGameModeActorName))
	{
		StartCustom->SetIsEnabled(true);
		SaveCustom->SetIsEnabled(true);
		SaveCustomAndStart->SetIsEnabled(true);
		RemoveSelectedCustom->SetIsEnabled(true);
		RemoveAllCustom->SetIsEnabled(true);
	}
	else
	{
		if (CustomGameModeETB->GetText().IsEmpty())
		{
			StartCustom->SetIsEnabled(false);
			SaveCustom->SetIsEnabled(false);
			SaveCustomAndStart->SetIsEnabled(false);
		}
		RemoveSelectedCustom->SetIsEnabled(false);
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

/* For when we only have a string, such as GameModeNameComboBox */
FGameModeActorStruct UGameModesWidget::FindGameModeFromString(const FString& GameModeName)
{
	// first check default game modes
	for (const EGameModeActorName GameModeActorName : TEnumRange<EGameModeActorName>())
	{
		if (GameModeName.Equals(UEnum::GetDisplayValueAsText(GameModeActorName).ToString()))
		{
			UE_LOG(LogTemp, Display, TEXT("%s"), *UEnum::GetDisplayValueAsText(GameModeActorName).ToString());
			return FindGameMode(GameModeActorName);
		}
	}
	// next check custom game modes
	for (TTuple<FString, FGameModeActorStruct>& Elem : CustomGameModesMap)
	{
		if (Elem.Key.Equals(GameModeName) &&
			!Elem.Key.IsEmpty())
		{
			return Elem.Value;
		}
	}
	return FGameModeActorStruct(EGameModeActorName::Custom, EGameModeDifficulty::Normal);
}

/* For when we have both a GameModeActorName and CustomGameModeName */
FGameModeActorStruct UGameModesWidget::FindGameMode(EGameModeActorName GameModeActorName, FString CustomGameModeName)
{
	// User created custom game mode
	if (GameModeActorName == EGameModeActorName::Custom)
	{
		if (!CustomGameModeName.IsEmpty())
		{
			for (TTuple<FString, FGameModeActorStruct>& Elem : CustomGameModesMap)
			{
				if (Elem.Key.Equals(CustomGameModeName))
				{
					return Elem.Value;
				}
			}
		}
	}
	// default game mode
	else
	{
		for (const FGameModeActorStruct& GameModeActor : GameModeActorDefaults)
		{
			if (GameModeActorName == GameModeActor.GameModeActorName &&
				GameModeActor.GameModeActorName != EGameModeActorName::Custom)
			{
				return GameModeActor;
			}
		}
	}
	return FGameModeActorStruct(EGameModeActorName::Custom, EGameModeDifficulty::Normal);
}

bool UGameModesWidget::IsCustomGameMode(const FString& CustomGameModeName)
{
	for (TTuple<FString, FGameModeActorStruct>& Elem : CustomGameModesMap)
	{
		if (Elem.Key.Equals(CustomGameModeName))
		{
			return true;
		}
	}
	return false;
}

bool UGameModesWidget::IsDefaultGameMode(const FString& GameModeName)
{
	for (const FGameModeActorStruct& GameModeActor : GameModeActorDefaults)
	{
		if (GameModeName == UEnum::GetDisplayValueAsText(GameModeActor.GameModeActorName).ToString()
			&& GameModeName != "Custom")
		{
			return true;
		}
	}
	return false;
}

void UGameModesWidget::ChangeSaveButtonStates(const FText& Text)
{
	if (!Text.IsEmpty())
	{
		SaveCustom->SetIsEnabled(true);
		SaveCustomAndStart->SetIsEnabled(true);
	}
	else
	{
		if (IsCustomGameMode(GameModeNameComboBox->GetSelectedOption()))
		{
			SaveCustom->SetIsEnabled(true);
			SaveCustomAndStart->SetIsEnabled(true);
		}
		else
		{
			SaveCustom->SetIsEnabled(false);
			SaveCustomAndStart->SetIsEnabled(false);
		}
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
