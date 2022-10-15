// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"
#include "BeatTrack.h"
#include "DefaultGameInstance.h"
#include "GameModeActorStruct.h"
#include "NarrowSpreadMultiBeat.h"
#include "CustomBeat.h"
#include "DefaultPlayerController.h"
#include "NarrowSpreadSingleBeat.h"
#include "WideSpreadSingleBeat.h"
#include "WideSpreadMultiBeat.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "Kismet/KismetStringLibrary.h"

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();
	// Store instance of GameModeBase in Game Instance
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterGameModeBase(this);
		GI->OnAASettingsChange.AddDynamic(this, &ADefaultGameMode::RefreshAASettings);
	}
	GameModeActorAlive = false;

	InitializeGameMode(GI->GameModeActorStruct.GameModeActorName);

	//TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();
	//RequestObj->SetObjectField("name", JSONPayload);
	//UE_LOG(LogTemp, Display, TEXT("%s"), *JSONPayload);

	//FString RequestBody;
	//TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	//FJsonSerializer::Serialize(RequestObj, Writer);

	//UE_LOG(LogTemp, Display, TEXT("%s"), *RequestBody);
	TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> Map = GI->LoadPlayerScores();
	// iterate through all elements in PlayerScoreMap

	FJsonScore JsonScore;

	for (TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : Map)
	{
		FJsonGameModeScore JsonGameModeScore;
		FString GameModeActorName = UKismetStringLibrary::RightChop(StaticEnum<EGameModeActorName>()->GetValueAsString(Elem.Key.GameModeActorName), 20);
		JsonGameModeScore.GameMode_SongTitle = UKismetStringLibrary::Concat_StrStr(UKismetStringLibrary::Concat_StrStr(GameModeActorName, "_"), Elem.Key.SongTitle);
		//FString JSONGameModeActorStruct;
		//FJsonObjectConverter::UStructToJsonObjectString(Elem.Key, JSONGameModeActorStruct);
		// get array of player scores from current key value
		JsonGameModeScore.PlayerScoreArray = Elem.Value.PlayerScoreArray;
		// iterate through array of player scores to find high score for current game mode / song
		JsonScore.GameModeScorePairs.Add(JsonGameModeScore);
	}

	//FString JSONPayload;
	//FJsonObjectConverter::UStructToJsonObjectString(JsonScore, JSONPayload);

	TSharedRef<FJsonObject> OutJsonObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FJsonScore::StaticStruct(), &JsonScore, OutJsonObject, 0, 0);

	FString OutputString;

	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(OutJsonObject, Writer);

	UE_LOG(LogTemp, Display, TEXT("%s"), *OutputString);

	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ADefaultGameMode::OnResponseReceived);
	Request->SetURL("http://localhost:3000/api/savescores");
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString( OutputString );
	Request->ProcessRequest();
}

void ADefaultGameMode::InitializeGameMode(EGameModeActorName GameModeActorName)
{
	//if (GameModeActorAlive == true)
	//{
	//	GI->GameModeActorBaseRef->Destroy();
	//	GameModeActorAlive = false;
	//}

	// Wide Spread Multi Beat
	if (GameModeActorName == EGameModeActorName::WideSpreadMultiBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<AWideSpreadMultiBeat>(WideSpreadMultiBeatClass);
	}
	// Narrow Spread Multi Beat
	else if (GameModeActorName == EGameModeActorName::NarrowSpreadMultiBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ANarrowSpreadMultiBeat>(NarrowSpreadMultiBeatClass);
	}
	// Narrow Spread Single Beat
	else if (GameModeActorName == EGameModeActorName::NarrowSpreadSingleBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ANarrowSpreadSingleBeat>(NarrowSpreadSingleBeatClass);
	}
	// Wide Spread Single Beat
	else if (GameModeActorName == EGameModeActorName::WideSpreadSingleBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<AWideSpreadSingleBeat>(WideSpreadSingleBeatClass);
	}
	else if (GameModeActorName == EGameModeActorName::BeatTrack)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ABeatTrack>(BeatTrackClass);
	}
	else if (GameModeActorName == EGameModeActorName::BeatGrid)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ACustomBeat>(CustomBeatClass);
	}
	else if (GameModeActorName == EGameModeActorName::Custom)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ACustomBeat>(CustomBeatClass);
	}
	else
	{
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("GameMode Init Error"));
		}
	}

	if (GameModeActorBase)
	{
		GameModeActorAlive = true;
	}

	GameModeActorBase->OnDestroyed.AddDynamic(this, &ADefaultGameMode::SetGameModeActorDestroyed);

	// initialize AA settings
	RefreshAASettings();

	// spawn visualizer
	const FVector VisualizerLocation = { 3910,0,1490 };
	const FRotator Rotation = FRotator::ZeroRotator;
	const FActorSpawnParameters SpawnParameters;
	Visualizer = GetWorld()->SpawnActor(VisualizerClass, &VisualizerLocation , &Rotation, SpawnParameters);

	// call blueprint function
	InitializeAudioManagers();

	// Call blueprint function
	PauseAAManagers();

	// Show crosshair and countdown
	Cast<ADefaultPlayerController>(GetWorld()->GetFirstPlayerController())->ShowCrosshair();
	Cast<ADefaultPlayerController>(GetWorld()->GetFirstPlayerController())->ShowCountdown();
}

void ADefaultGameMode::RefreshAASettings()
{
	AASettings = GI->LoadAASettings();
}

void ADefaultGameMode::SetGameModeActorDestroyed(AActor* DestroyedActor)
{
	GameModeActorAlive = false;
}

void ADefaultGameMode::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	UE_LOG(LogTemp, Display, TEXT("Response: %s"), *Response->GetContentAsString());
}

