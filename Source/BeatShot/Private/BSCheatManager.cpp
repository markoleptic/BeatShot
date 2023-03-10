#include "BSCheatManager.h"
#include "BSCharacter.h"
#include "BSGameMode.h"
#include "BSPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UBSCheatManager::InitCheatManager()
{
	ReceiveInitCheatManager();
}

void UBSCheatManager::SetAimBotEnabled(const bool bEnable) const
{
	ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ABSCharacter* Character = Cast<ABSCharacter>(Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn());

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
