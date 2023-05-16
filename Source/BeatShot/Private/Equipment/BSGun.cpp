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

void ABSGun::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GameplayTags);
}

bool ABSGun::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return IGameplayTagAssetInterface::HasMatchingGameplayTag(TagToCheck);
}

bool ABSGun::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAllMatchingGameplayTags(TagContainer);
}

bool ABSGun::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAnyMatchingGameplayTags(TagContainer);
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
		GameplayTags.AddTag(FBSGameplayTags().Get().State_Weapon_AutomaticFire);
		return;
	}
	GameplayTags.RemoveTag(FBSGameplayTags().Get().State_Weapon_AutomaticFire);
}

void ABSGun::SetShouldRecoil(const bool bRecoil)
{
	StopFire();
	
	if (bRecoil)
	{
		GameplayTags.AddTag(FBSGameplayTags().Get().State_Weapon_Recoil);
		return;
	}
	GameplayTags.RemoveTag(FBSGameplayTags().Get().State_Weapon_Recoil);
}

void ABSGun::SetShowDecals(const bool bShowDecals)
{
	if (bShowDecals)
	{
		GameplayTags.AddTag(FBSGameplayTags().Get().State_Weapon_ShowDecals);
		return;
	}
	GameplayTags.RemoveTag(FBSGameplayTags().Get().State_Weapon_ShowDecals);
}

void ABSGun::SetShowTracers(const bool bShowTracers)
{
	if (bShowTracers)
	{
		GameplayTags.AddTag(FBSGameplayTags().Get().State_Weapon_ShowTracers);
		return;
	}
	GameplayTags.RemoveTag(FBSGameplayTags().Get().State_Weapon_ShowTracers);
}