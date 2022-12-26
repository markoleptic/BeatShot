// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModesWidget.h"
#include "DefaultGameInstance.h"
#include "SaveGameCustomGameMode.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/Border.h"
#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DefaultValueHelper.h"

void UGameModesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::Custom, EGameModeDifficulty::Normal));
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::BeatGrid, EGameModeDifficulty::Normal));
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::BeatTrack, EGameModeDifficulty::Normal));
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::SingleBeat,
	                                                         EGameModeDifficulty::Normal,
	                                                         ESpreadType::DynamicEdgeOnly));
	GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::MultiBeat, EGameModeDifficulty::Normal,
	                                                         ESpreadType::DynamicRandom));
	GameModesToDisplay = GameModeActorDefaults;
	SpreadSelect->SetVisibility(ESlateVisibility::Collapsed);
	PlayFromStandardButton->SetIsEnabled(false);
	CustomizeFromStandardButton->SetIsEnabled(false);
	SaveCustomButton->SetIsEnabled(false);
	SaveCustomAndStartButton->SetIsEnabled(false);
	StartCustomButton->SetIsEnabled(false);
	LoadCustomGameModes();

	if (CustomGameModesMap.IsEmpty())
	{
		RemoveAllCustomButton->SetIsEnabled(false);
	}
	else
	{
		RemoveAllCustomButton->SetIsEnabled(false);
	}
	RemoveSelectedCustomButton->SetIsEnabled(false);

	CustomGameModeETB->OnTextChanged.AddDynamic(this, &UGameModesWidget::ChangeSaveButtonStates);
	GameModeNameComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::UpdateCustomGameModeOptions);
	VerticalSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpawnAreaConstrained);
	HorizontalSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpawnAreaConstrained);
	BeatGridHorizontalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpacingConstrained);
	BeatGridVerticalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpacingConstrained);
	MinTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridTargetSizeConstrained);
	MaxTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridTargetSizeConstrained);
	NumBeatGridTargetsComboBox->OnSelectionChanged.AddDynamic(
		this, &UGameModesWidget::BeatGridNumberOfTargetsConstrained);

	PopulateGameModeNameComboBox("Custom");
}

FGameModeActorStruct UGameModesWidget::GameModeActorStructConstructor(const EGameModeActorName GameModeActor,
                                                                      const EGameModeDifficulty NewGameModeDifficulty,
                                                                      const ESpreadType NewSpreadType)
{
	return FGameModeActorStruct(GameModeActor, NewGameModeDifficulty, NewSpreadType);
}

void UGameModesWidget::SaveCustomGameMode() const
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

void UGameModesWidget::PopulateGameModeNameComboBox(const FString& GameModeOptionToSelect)
{
	
}

FGameModeActorStruct UGameModesWidget::GetGameMode(const EGameModeActorName& GameModeActorName, const FString& GameModeName)
{
	// User created custom game mode
	if (GameModeActorName == EGameModeActorName::Custom)
	{
		if (!GameModeName.IsEmpty())
		{
			for (TTuple<FString, FGameModeActorStruct>& Elem : CustomGameModesMap)
			{
				if (Elem.Key.Equals(GameModeName))
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

FGameModeActorStruct UGameModesWidget::GetCustomGameMode(const FString& CustomGameModeName)
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
	return FGameModeActorStruct(EGameModeActorName::Custom, EGameModeDifficulty::Normal);
}


void UGameModesWidget::UpdateCustomGameModeOptions(const FString SelectedGameModeActorName, const ESelectInfo::Type SelectionType)
{
	if (SelectionType != ESelectInfo::Direct)
	{
		const FGameModeActorStruct FoundGameMode = GetCustomGameMode(SelectedGameModeActorName);
		//SelectedGameMode = FoundGameMode.GameModeActorName;
		PopulateGameModeNameComboBox(SelectedGameModeActorName);
	}
	if (IsDefaultGameMode(SelectedGameModeActorName) ||
		(SelectedGameModeActorName == "Custom" && CustomGameModeETB->GetText().IsEmptyOrWhitespace()))
	{
		if (CustomGameModeETB->GetText().IsEmptyOrWhitespace())
		{
			StartCustomButton->SetIsEnabled(false);
			SaveCustomButton->SetIsEnabled(false);
			SaveCustomAndStartButton->SetIsEnabled(false);
		}
		RemoveSelectedCustomButton->SetIsEnabled(false);
	}
	else
	{
		StartCustomButton->SetIsEnabled(true);
		SaveCustomButton->SetIsEnabled(true);
		SaveCustomAndStartButton->SetIsEnabled(true);
		RemoveSelectedCustomButton->SetIsEnabled(true);
		RemoveAllCustomButton->SetIsEnabled(true);
	}
}

// /* For when we only have a string, such as GameModeNameComboBox */
// FGameModeActorStruct UGameModesWidget::FindGameMode(const FString& GameModeName)
// {
// 	// first check default game modes
// 	for (const EGameModeActorName GameModeActorName : TEnumRange<EGameModeActorName>())
// 	{
// 		if (GameModeName.Equals(UEnum::GetDisplayValueAsText(GameModeActorName).ToString()))
// 		{
// 			UE_LOG(LogTemp, Display, TEXT("%s"), *UEnum::GetDisplayValueAsText(GameModeActorName).ToString());
// 			return FindGameMode(GameModeActorName);
// 		}
// 	}
// 	// next check custom game modes
// 	for (TTuple<FString, FGameModeActorStruct>& Elem : CustomGameModesMap)
// 	{
// 		if (Elem.Key.Equals(GameModeName) &&
// 			!Elem.Key.IsEmpty())
// 		{
// 			return Elem.Value;
// 		}
// 	}
// 	return FGameModeActorStruct(EGameModeActorName::Custom, EGameModeDifficulty::Normal);
// }

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
	if (Text.IsEmptyOrWhitespace())
	{
		if (IsDefaultGameMode(GameModeNameComboBox->GetSelectedOption()) ||
		(GameModeNameComboBox->GetSelectedOption() == "Custom" && CustomGameModeETB->GetText().IsEmptyOrWhitespace()))
		{
			SaveCustomButton->SetIsEnabled(false);
			SaveCustomAndStartButton->SetIsEnabled(false);
			return;
		}
	}
	SaveCustomButton->SetIsEnabled(true);
	SaveCustomAndStartButton->SetIsEnabled(true);
}

void UGameModesWidget::PopulateGameModeOptions(FGameModeActorStruct InputGameModeActorStruct)
{
}

void UGameModesWidget::BeatGridSpacingConstrained(float value)
{
	if (BaseGameModeComboBox->GetSelectedOption() != "BeatGrid")
	{
		return;
	}

	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
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

void UGameModesWidget::BeatGridTargetSizeConstrained(float value)
{
	if (BaseGameModeComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}
	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
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
	if (BaseGameModeComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}

	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
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
	if (BaseGameModeComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}

	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
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

bool UGameModesWidget::CheckAllBeatGridConstraints()
{
	if (BaseGameModeComboBox->GetSelectedOption() != "BeatGrid")
	{
		return true;
	}

	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
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
