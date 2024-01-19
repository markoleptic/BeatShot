// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Equipment/BSGun.h"
#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "BeatShot/BSGameplayTags.h"
#include "Character/BSCharacterBase.h"

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
	GI->GetPublicGameSettingsChangedDelegate().AddUObject(this, &ABSGun::OnPlayerSettingsChanged_Game);
}

void ABSGun::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	SetShouldRecoil(GameSettings.bShouldRecoil);
	SetFireRate(GameSettings.bAutomaticFire);
	SetShowDecals(GameSettings.bShowBulletDecals);
	SetShowTracers(GameSettings.bShowBulletTracers && GameSettings.bShowWeaponMesh && GameSettings.bShowCharacterMesh);
	SetShowMuzzleFlash(
		GameSettings.bShowMuzzleFlash && GameSettings.bShowWeaponMesh && GameSettings.bShowCharacterMesh);
	SetShowWeaponMesh(GameSettings.bShowWeaponMesh);
}

void ABSGun::Fire()
{
	if (!bCanFire)
	{
		return;
	}
	bIsFiring = true;
	if (OnShotFired.IsBound())
	{
		check(GetOwner());
		if (const ABSCharacterBase* Character = Cast<ABSCharacterBase>(GetOwner()))
		{
			OnShotFired.Execute(Character->GetBSPlayerController());
		}
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

void ABSGun::SetShowWeaponMesh(const bool bShow)
{
	SetActorHiddenInGame(!bShow);
	if (bShow)
	{
		AddGameplayTag(FBSGameplayTags().Get().State_Weapon_ShowMesh);
		return;
	}
	RemoveGameplayTag(FBSGameplayTags().Get().State_Weapon_ShowMesh);
}

void ABSGun::SetShowMuzzleFlash(const bool bShow)
{
	if (bShow)
	{
		AddGameplayTag(FBSGameplayTags().Get().State_Weapon_ShowMuzzleFlash);
		return;
	}
	RemoveGameplayTag(FBSGameplayTags().Get().State_Weapon_ShowMuzzleFlash);
}
