#include "DefaultCheatManager.h"
#include "DefaultCharacter.h"
#include "DefaultGameMode.h"
#include "DefaultPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UDefaultCheatManager::InitCheatManager()
{
	ReceiveInitCheatManager();
}

void UDefaultCheatManager::SetAimBotEnabled(const bool bEnable) const
{
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ADefaultCharacter* Character = Cast<ADefaultCharacter>(Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn());
	
	if (!GameMode || !Character)
	{
		return;
	}
	if (bEnable)
	{
		if (!GameMode->OnTargetSpawned.IsBoundToObject(Character))
		{
			GameMode->OnTargetSpawned.AddUFunction(Character, FName("OnTargetSpawned_AimBot"));
		}
	}
	else
	{
		if (GameMode->OnTargetSpawned.IsBoundToObject(Character))
		{
			GameMode->OnTargetSpawned.RemoveAll(Character);
		}
	}
	Character->SetEnabled_AimBot(bEnable);
}
