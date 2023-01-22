#include "SteamManager.h"
#include "DefaultGameInstance.h"

USteamManager::USteamManager()
{
	UE_LOG(LogTemp, Display, TEXT("USteamManager Constructor Called"));
}

USteamManager::~USteamManager()
{
}

void USteamManager::InitializeSteamManager()
{
	UE_LOG(LogTemp, Display, TEXT("Initializing USteamManager"));
	OnSteamOverlayActiveCallback.Register(this, &USteamManager::OnSteamOverlayActive);
	UE_LOG(LogTemp, Display, TEXT("OnSteamOverlayActiveCallback.Register called"));
}

void USteamManager::AssignGameInstance(UDefaultGameInstance* InDefaultGameInstance)
{
	DefaultGameInstance = InDefaultGameInstance;
	UE_LOG(LogTemp, Display, TEXT("New Game Instance Assigned to USteamManager"));
}

void USteamManager::OnSteamOverlayActive(GameOverlayActivated_t* pCallbackData)
{
	const bool bIsCurrentOverlayActive = pCallbackData->m_bActive != 0;
	DefaultGameInstance; //So that the call list reference on the Lambda works
	if (bIsCurrentOverlayActive)
	{
		AsyncTask(ENamedThreads::GameThread, [&]()
		{
			DefaultGameInstance->OnSteamOverlayIsOn();
		});
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [&]()
		{
			DefaultGameInstance->OnSteamOverlayIsOff();
		});
	}
}
