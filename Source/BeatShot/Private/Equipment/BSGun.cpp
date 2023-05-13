// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Equipment/BSGun.h"
#include "BSCharacter.h"
#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "SphereTarget.h"
#include "BeatShot/BSGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ABSGun::ABSGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("Gun");
	MeshComp->SetOnlyOwnerSee(false);
	MeshComp->CastShadow = false;
	RootComponent = MeshComp;
	MuzzleLocationComp = CreateDefaultSubobject<USceneComponent>("Muzzle Location");
	MuzzleLocationComp->SetupAttachment(MeshComp, "Muzzle");
	bCanFire = true;
	bIsFiring = false;
	KickbackAlpha = 0.f;
	KickbackAngle = 0.f;
	bShouldKickback = false;
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

	/* Bind UpdateRecoil to the Recoil vector curve and timeline */
	FOnTimelineVector RecoilProgressFunction;
	RecoilProgressFunction.BindUFunction(this, FName("UpdateRecoil"));
	RecoilTimeline.AddInterpVector(RecoilCurve, RecoilProgressFunction);
}

void ABSGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RecoilTimeline.TickTimeline(DeltaTime);
	if (ShouldRecoil())
	{
		UpdateKickbackAndRecoil(DeltaTime);
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
	ShotsFired++;
}

void ABSGun::StopFire()
{
	bIsFiring = false;
	CurrentShotRecoilRotation = FRotator(0, 0, 0);
	CurrentShotCameraRecoilRotation = FRotator(0, 0, 0);
	ShotsFired = 0;
	
	/* Reverse the timeline so that it takes time to recover to the beginning */
	RecoilTimeline.SetPlayRate(5.454545f);
	RecoilTimeline.Reverse();
}

ABSCharacter* ABSGun::GetBSCharacter() const
{
	return Cast<ABSCharacter>(GetOwner());
}

FRotator ABSGun::GetCurrentRecoilRotation() const
{
	return FRotator(-CurrentShotRecoilRotation.Pitch, CurrentShotRecoilRotation.Yaw, CurrentShotRecoilRotation.Roll);
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
	
	if (UCameraComponent* Camera = GetBSCharacter()->GetCamera())
	{
		Camera->SetRelativeRotation(FRotator(0, 0, 0));
	}
	if (USceneComponent* Camera = GetBSCharacter()->GetCameraRecoilComponent())
	{
		Camera->SetRelativeRotation(FRotator(0, 0, 0));
	}
	
	bAutomaticFire = bAutomatic;
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
	if (UCameraComponent* Camera = GetBSCharacter()->GetCamera())
	{
		Camera->SetRelativeRotation(FRotator(0, 0, 0));
	}
	if (USceneComponent* Camera = GetBSCharacter()->GetCameraRecoilComponent())
	{
		Camera->SetRelativeRotation(FRotator(0, 0, 0));
	}
	
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

void ABSGun::Recoil()
{
	RecoilTimeline.SetPlayRate(1.f);
	/* Resume timeline if it hasn't fully recovered */
	if (RecoilTimeline.IsReversing())
	{
		RecoilTimeline.Play();
	}
	else
	{
		RecoilTimeline.PlayFromStart();
	}
	bShouldKickback = true;
	KickbackAlpha = 0.f;
}

void ABSGun::UpdateKickbackAndRecoil(const float DeltaTime)
{
	UpdateKickback(DeltaTime);
	const FRotator Current = GetBSCharacter()->GetCamera()->GetRelativeRotation();
	const FRotator UpdatedRotation = UKismetMathLibrary::RInterpTo(Current, CurrentShotCameraRecoilRotation, DeltaTime, CameraRecoilInterpSpeed);
	GetBSCharacter()->GetCamera()->SetRelativeRotation(UpdatedRotation);
	GetBSCharacter()->GetCameraRecoilComponent()->SetRelativeRotation(FRotator(KickbackAngle, 0, 0));
}

void ABSGun::UpdateKickback(const float DeltaTime)
{
	if (bShouldKickback)
	{
		if (KickbackAlpha + DeltaTime >= KickbackDuration)
		{
			KickbackAlpha = KickbackDuration;
			bShouldKickback = false;
		}
		else
		{
			KickbackAlpha += DeltaTime;
		}
	}
	else
	{
		KickbackAngle = 0.f;
	}
	KickbackAngle = KickbackCurve->GetFloatValue(KickbackAlpha / KickbackDuration);
}

void ABSGun::UpdateRecoil(const FVector Output)
{
	if (!bIsFiring)
	{
		return;
	}
	/** Don't continue playing animation if semi-automatic */
	if (bAutomaticFire || ShotsFired < 1)
	{
		CurrentShotCameraRecoilRotation.Yaw = Output.X * 0.5;
		CurrentShotCameraRecoilRotation.Pitch = Output.Y * 0.5;
		CurrentShotRecoilRotation.Yaw = Output.X;
		CurrentShotRecoilRotation.Pitch = Output.Y;
		return;
	}
	CurrentShotCameraRecoilRotation = FRotator(0, 0, 0);
	CurrentShotRecoilRotation = FRotator(0, 0, 0);
}