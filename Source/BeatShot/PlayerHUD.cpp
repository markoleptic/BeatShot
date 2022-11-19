// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameModeActorBase.h"
#include "DefaultGameInstance.h"
#include "Components/HorizontalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "Kismet/KismetStringLibrary.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));

	// update Scores when GameModeActorBase calls for an update
	GI->GameModeActorBaseRef->UpdateScoresToHUD.AddDynamic(this, &UPlayerHUD::UpdateAllElements);

	TargetBar->SetPercent(0.f);
	Accuracy->SetText(FText::AsPercent(0.f));

	if (GI->GameModeActorStruct.IsBeatTrackMode)
	{
		TargetsSpawnedBox->SetVisibility(ESlateVisibility::Collapsed);
		StreakBox->SetVisibility(ESlateVisibility::Collapsed);
		TargetsHitBox->SetVisibility(ESlateVisibility::Collapsed);
		ShotsFiredBox->SetVisibility(ESlateVisibility::Collapsed);
	}

	// initial value update
	UpdateAllElements(GI->GameModeActorBaseRef->PlayerScores);
}

void UPlayerHUD::UpdateAllElements(FPlayerScore NewPlayerScoreStruct)
{
	// display default game mode names if not custom
	if (NewPlayerScoreStruct.CustomGameModeName.Equals(""))
	{
		GameModeNameText->SetText(UEnum::GetDisplayValueAsText(NewPlayerScoreStruct.GameModeActorName));
	}
	// display custom game mode if not a default game mode
	else
	{
		GameModeNameText->SetText(UKismetTextLibrary::Conv_StringToText(NewPlayerScoreStruct.CustomGameModeName));
	}

	// show song title and total song length
	SongTitle->SetText(UKismetTextLibrary::Conv_StringToText(NewPlayerScoreStruct.SongTitle));
	TotalSongLength->SetText(UKismetTextLibrary::Conv_StringToText(
		UKismetStringLibrary::LeftChop(UKismetStringLibrary::TimeSecondsToString(NewPlayerScoreStruct.SongLength),3)));

	// Beat Track changes how stats are displayed
	if (NewPlayerScoreStruct.TotalPossibleDamage > 0.01f)
	{
		const float Score = round(NewPlayerScoreStruct.Score);
		const float TotalPossibleDamage = NewPlayerScoreStruct.TotalPossibleDamage;
		const float HighScore = round(NewPlayerScoreStruct.HighScore);

		// Update Accuracy progress bar and Accuracy percentage text
		if (!(isnan(Score / TotalPossibleDamage)))
		{
			TargetBar->SetPercent(Score / TotalPossibleDamage);
			Accuracy->SetText(FText::AsPercent(Score / TotalPossibleDamage));
		}

		// Update current score
		CurrentScoreText->SetText(FText::AsNumber(Score));

		// Update high score
		if (HighScore < Score)
		{
			HighScoreText->SetText(FText::AsNumber(Score));
		}
		else
		{
			HighScoreText->SetText(FText::AsNumber(HighScore));
		}
	}
	else
	{
		const float TargetsHit = NewPlayerScoreStruct.TargetsHit;
		const float Score = round(NewPlayerScoreStruct.Score);
		const float ShotsFired = NewPlayerScoreStruct.ShotsFired;
		const float TargetsSpawned = NewPlayerScoreStruct.TargetsSpawned;
		const float HighScore = round(NewPlayerScoreStruct.HighScore);

		// Update Accuracy progress bar and Accuracy percentage text
		if (!(isnan(TargetsHit / ShotsFired)))
		{
			TargetBar->SetPercent(TargetsHit / ShotsFired);
			Accuracy->SetText(FText::AsPercent(TargetsHit / ShotsFired));
		}

		// Update number of targets hit
		TargetsHitText->SetText(FText::AsNumber(TargetsHit));

		// Update number of shots fired
		ShotsFiredText->SetText(FText::AsNumber(ShotsFired));

		// Update number of targets spawned
		TargetsSpawnedText->SetText(FText::AsNumber(TargetsSpawned));

		// update the current player score
		CurrentScoreText->SetText(FText::AsNumber(Score));

		// update the high score
		if (HighScore < Score)
		{
			HighScoreText->SetText(FText::AsNumber(Score));
		}
		else
		{
			HighScoreText->SetText(FText::AsNumber(HighScore));
		}
		CurrentStreakBestText->SetText(FText::AsNumber(NewPlayerScoreStruct.Streak));
	}
}

