// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/PlayerHUD.h"
#include "SaveGamePlayerScore.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "WidgetComponents/HitTimingWidget.h"
#include "Kismet/KismetTextLibrary.h"
#include "Kismet/KismetStringLibrary.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	/** Initial value update */
	ProgressBar_Accuracy->SetPercent(0.f);
	TextBlock_Accuracy->SetText(FText::AsPercent(0.f));
	TextBlock_SongTimeElapsed->SetText(FText::FromString(UKismetStringLibrary::TimeSecondsToString(0).LeftChop(3)));
}

void UPlayerHUD::Init(const FBSConfig& InConfig)
{
	switch (InConfig.TargetConfig.TargetDamageType)
	{
	case ETargetDamageType::Tracking:
		Box_TargetsSpawned->SetVisibility(ESlateVisibility::Collapsed);
		Box_Streak->SetVisibility(ESlateVisibility::Collapsed);
		Box_TargetsHit->SetVisibility(ESlateVisibility::Collapsed);
		Box_ShotsFired->SetVisibility(ESlateVisibility::Collapsed);
		HitTimingWidget->SetIsEnabled(false);
		HitTimingWidget->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case ETargetDamageType::Hit:
		{
			const FString MinTick = FString::FromInt(-static_cast<int32>(InConfig.TargetConfig.SpawnBeatDelay * 100.f)) + "ms";
			const FString MaxTick = FString::FromInt(static_cast<int32>((InConfig.TargetConfig.TargetMaxLifeSpan - InConfig.TargetConfig.SpawnBeatDelay) * 100.f)) + "ms";
			HitTimingWidget->Init(FText::FromString(MinTick), FText::FromString(MaxTick));
		}
		break;
	case ETargetDamageType::Combined:
	case ETargetDamageType::None:
		break;
	}

	// Display default game mode names if not custom
	if (InConfig.DefiningConfig.GameModeType == EGameModeType::Preset)
	{
		TextBlock_GameModeName->SetText(UEnum::GetDisplayValueAsText(InConfig.DefiningConfig.BaseGameMode));
	}
	// Display custom game mode if not a default game mode
	else
	{
		TextBlock_GameModeName->SetText(UKismetTextLibrary::Conv_StringToText(InConfig.DefiningConfig.CustomGameModeName));
	}

	TextBlock_SongTitle->SetText(UKismetTextLibrary::Conv_StringToText(InConfig.AudioConfig.SongTitle));
	TextBlock_TotalSongLength->SetText(UKismetTextLibrary::Conv_StringToText(UKismetStringLibrary::LeftChop(UKismetStringLibrary::TimeSecondsToString(InConfig.AudioConfig.SongLength), 3)));

	Config = InConfig;
}

void UPlayerHUD::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	if (GameSettings.bShowHitTimingWidget && Config.TargetConfig.TargetDamageType == ETargetDamageType::Hit)
	{
		if (!HitTimingWidget->GetIsEnabled())
		{
			HitTimingWidget->SetIsEnabled(true);
			HitTimingWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else if (!GameSettings.bShowHitTimingWidget && Config.TargetConfig.TargetDamageType == ETargetDamageType::Hit)
	{
		if (HitTimingWidget->GetIsEnabled())
		{
			HitTimingWidget->SetIsEnabled(false);
			HitTimingWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UPlayerHUD::UpdateAllElements(const FPlayerScore& NewPlayerScoreStruct, const float NormalizedHitTimingError, const float HitTimingError)
{
	switch (Config.TargetConfig.TargetDamageType)
	{
	case ETargetDamageType::Tracking:
		{
			const float Score = roundf(NewPlayerScoreStruct.Score);
			const float TotalPossibleDamage = NewPlayerScoreStruct.TotalPossibleDamage;
			const float HighScore = roundf(NewPlayerScoreStruct.HighScore);
			/** Update Accuracy progress bar and Accuracy percentage text */
			if (!isnan(Score / TotalPossibleDamage))
			{
				ProgressBar_Accuracy->SetPercent(Score / TotalPossibleDamage);
				TextBlock_Accuracy->SetText(FText::AsPercent(Score / TotalPossibleDamage));
			}
			/** Update current score */
			TextBlock_CurrentScore->SetText(FText::AsNumber(Score));
			/** Update high score */
			if (HighScore < Score)
			{
				TextBlock_HighScore->SetText(FText::AsNumber(Score));
			}
			else
			{
				TextBlock_HighScore->SetText(FText::AsNumber(HighScore));
			}
		}
		break;
	case ETargetDamageType::Hit:
		{
			const float TargetsHit = NewPlayerScoreStruct.TargetsHit;
			const float Score = round(NewPlayerScoreStruct.Score);
			const float ShotsFired = NewPlayerScoreStruct.ShotsFired;
			const float TargetsSpawned = NewPlayerScoreStruct.TargetsSpawned;
			const float HighScore = round(NewPlayerScoreStruct.HighScore);
			/** Update Accuracy progress bar and Accuracy percentage text */
			if (!isnan(TargetsHit / ShotsFired))
			{
				ProgressBar_Accuracy->SetPercent(TargetsHit / ShotsFired);
				TextBlock_Accuracy->SetText(FText::AsPercent(TargetsHit / ShotsFired));
			}
			/* Update number of targets hit */
			TextBlock_TargetsHit->SetText(FText::AsNumber(TargetsHit));
			/* Update number of shots fired */
			TextBlock_ShotsFired->SetText(FText::AsNumber(ShotsFired));
			/* Update number of targets spawned */
			TextBlock_TargetsSpawned->SetText(FText::AsNumber(TargetsSpawned));
			/* update the current player score */
			TextBlock_CurrentScore->SetText(FText::AsNumber(Score));
			/* update the high score */
			if (HighScore < Score)
			{
				TextBlock_HighScore->SetText(FText::AsNumber(Score));
			}
			else
			{
				TextBlock_HighScore->SetText(FText::AsNumber(HighScore));
			}
			/* update streak */
			TextBlock_CurrentStreakBest->SetText(FText::AsNumber(NewPlayerScoreStruct.Streak));
		}
		break;
	case ETargetDamageType::Combined:
	case ETargetDamageType::None:
		break;
	}
	if (HitTimingWidget->GetIsEnabled())
	{
		HitTimingWidget->UpdateHitTiming(NormalizedHitTimingError, HitTimingError);
	}
	
}

void UPlayerHUD::UpdateSongProgress(const float PlaybackTime)
{
	TextBlock_SongTimeElapsed->SetText(FText::FromString(UKismetStringLibrary::TimeSecondsToString(PlaybackTime).LeftChop(3)));
}
