// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Equipment/BSGun.h"
#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "BeatShot/BSGameplayTags.h"
#include "Kismet/GameplayStatics.h"

ABSGun::ABSGun()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("Gun");
	MeshComp->SetOnlyOwnerSee(false);
	MeshComp->CastShadow = false;
	RootComponent = MeshComp;
	MuzzleLocationComp = CreateDefaultSubobject<USceneComponent>("Muzzle Location");
	MuzzleLocationComp->SetupAttachment(MeshComp, "Muzzle");
	bCanFire = true;
	bIsFiring = false;
}

void ABSGun::BeginPlay()
{
	Super::BeginPlay();

	OnPlayerSettingsChanged_Game(LoadPlayerSettings().Game);

	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	GI->GetPublicGameSettingsChangedDelegate().AddUniqueDynamic(this, &ABSGun::OnPlayerSettingsChanged_Game);

	if (ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld())); !OnShotFired.IsBoundToObject(GameMode))
	{
		GameMode->RegisterWeapon(OnShotFired);
	}
}

void ABSGun::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	SetShouldRecoil(GameSettings.bShouldRecoil);
	SetFireRate(GameSettings.bAutomaticFire);
	SetShowDecals(GameSettings.bShowBulletDecals);
	SetShowTracers(GameSettings.bShowBulletTracers);
}

void ABSGun::Fire()
{
	if (!bCanFire)
	{
		return;
	}
	bIsFiring = true;
	if (!OnShotFired.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("OnShotFired not bound"));
	}
}

void ABSGun::StopFire()
{
	bIsFiring = false;
}

FVector ABSGun::GetMuzzleLocation() const
{
	return MuzzleLocationComp->GetComponentLocation();
}

bool ABSGun::ShouldRecoil() const
{
	return HasMatchingGameplayTag(FBSGameplayTags::Get().State_Weapon_Recoil);
}

bool ABSGun::ShouldShowDecals() const
{
	return HasMatchingGameplayTag(FBSGameplayTags::Get().State_Weapon_ShowDecals);
}

bool ABSGun::ShouldShowTracers() const
{
	return HasMatchingGameplayTag(FBSGameplayTags::Get().State_Weapon_ShowTracers);
}

bool ABSGun::IsAutoFireRate() const
{
	return HasMatchingGameplayTag(FBSGameplayTags::Get().State_Weapon_AutomaticFire);
}

void ABSGun::SetFireRate(const bool bAutomatic)
{
	StopFire();
	
	if (bAutomatic)
	{
		AddGameplayTag(FBSGameplayTags().Get().State_Weapon_AutomaticFire);
		return;
	}
	RemoveGameplayTag(FBSGameplayTags().Get().State_Weapon_AutomaticFire);
}

void ABSGun::SetShouldRecoil(const bool bRecoil)
{
	StopFire();
	
	if (bRecoil)
	{
		AddGameplayTag(FBSGameplayTags().Get().State_Weapon_Recoil);
		return;
	}
	RemoveGameplayTag(FBSGameplayTags().Get().State_Weapon_Recoil);
}

void ABSGun::SetShowDecals(const bool bShowDecals)
{
	if (bShowDecals)
	{
		AddGameplayTag(FBSGameplayTags().Get().State_Weapon_ShowDecals);
		return;
	}
	RemoveGameplayTag(FBSGameplayTags().Get().State_Weapon_ShowDecals);
}

void ABSGun::SetShowTracers(const bool bShowTracers)
{
	if (bShowTracers)
	{
		AddGameplayTag(FBSGameplayTags().Get().State_Weapon_ShowTracers);
		return;
	}
	RemoveGameplayTag(FBSGameplayTags().Get().State_Weapon_ShowTracers);
}