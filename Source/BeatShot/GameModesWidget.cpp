// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModesWidget.h"
#include "DefaultGameInstance.h"
#include "SaveGameCustomGameMode.h"
#include "Components/Slider.h"
#include "Components/ComboBoxString.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DefaultValueHelper.h"

void UGameModesWidget::NativeConstruct()
{
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (UGameplayStatics::DoesSaveGameExist(TEXT("CustomGameModesSlot"), 3))
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(UGameplayStatics::LoadGameFromSlot(TEXT("CustomGameModesSlot"), 3));
	}
	else
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass()));
	}

	LoadCustomGameModes();
	PopulateGameModeSettings();


	//still need other bindings
	//SpawnHeightSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::UpdateBeatGridHeightConstraints);
	//SpawnWidthSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::UpdateBeatGridWidthConstraints);
	//BeatGridHorizontalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::UpdateBeatGridWidthConstraints);
	//BeatGridVerticalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::UpdateBeatGridHeightConstraints);
	//MinTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::UpdateBeatGridWidthConstraints);
	//MaxTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::UpdateBeatGridWidthConstraints);
	//MinTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::UpdateBeatGridHeightConstraints);
	//MaxTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::UpdateBeatGridHeightConstraints);

	SpawnHeightSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpawnAreaConstrained);
	SpawnWidthSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpawnAreaConstrained);

	BeatGridHorizontalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpacingConstrained);
	BeatGridVerticalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpacingConstrained);

	MinTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridTargetSizeConstrained);
	MaxTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridTargetSizeConstrained);

	MaxNumBeatGridTargetsComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::BeatGridNumberOfTargetsConstrained);
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
	if (USaveGameCustomGameMode* SaveGameCustomGameModeObject = Cast<USaveGameCustomGameMode>(UGameplayStatics::LoadGameFromSlot(TEXT("CustomGameModesSlot"), 3)))
	{
		CustomGameModesMap = SaveGameCustomGameModeObject->CustomGameModesMap;
		return SaveGameCustomGameModeObject->CustomGameModesMap;
	}
	return CustomGameModesMap;
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

void UGameModesWidget::UpdateBeatGridHeightConstraints(float value)
{
	if (GameModeCategoryComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}
	const float Height = round(SpawnHeightSlider->GetValue());
	const float TargetHeight = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = MaxNumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	// WidthORHeight = TargetScale*MaxTargets + Spacing*MaxTargets - Spacing + 200

	if (Height <= (TargetHeight * MaxTargets) + (VSpacing * (MaxTargets + 1)) )
	{
	}
	if (TargetHeight <= ( (  Height - (VSpacing * (MaxTargets + 1) ) ) / MaxTargets) )
	{
	}
	if (VSpacing <= ( ( Height - (TargetHeight * MaxTargets) ) / (MaxTargets + 1) ) )
	{
	}
	if (MaxTargets <= ( ( Height - VSpacing) / (TargetHeight + (2 * VSpacing) ) ) )
	{
	}
}

void UGameModesWidget::UpdateBeatGridWidthConstraints(float value)
{
	if (GameModeCategoryComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}

	const float Width = round(SpawnWidthSlider->GetValue());
	const float TargetWidth = ceil(MaxTargetScaleSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const FString ToConvert = MaxNumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	// WidthORHeight = TargetScale*MaxTargets + Spacing*MaxTargets - Spacing + 200

	if (Width <= (TargetWidth * MaxTargets) + (HSpacing * (MaxTargets + 1)))
	{
	}
	if (TargetWidth <= ((Width - (HSpacing * (MaxTargets + 1))) / MaxTargets))
	{
	}
	if (HSpacing <= ((Width - (TargetWidth * MaxTargets)) / (MaxTargets + 1)))
	{
	}
	if (MaxTargets <= ((Width - HSpacing) / (TargetWidth + (2 * HSpacing))))
	{
	}
}