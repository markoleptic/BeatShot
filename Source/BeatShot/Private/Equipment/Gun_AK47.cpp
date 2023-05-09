// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Equipment/Gun_AK47.h"
#include "BSCharacter.h"
#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "SphereTarget.h"
#include "BeatShot/BSGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AGun_AK47::AGun_AK47()
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

void AGun_AK47::BeginPlay()
{
	Super::BeginPlay();

	OnPlayerSettingsChanged_Game(LoadPlayerSettings().Game);

	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	GI->GetPublicGameSettingsChangedDelegate().AddUniqueDynamic(this, &AGun_AK47::OnPlayerSettingsChanged_Game);

	if (ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld())); !OnShotFired.IsBoundToObject(GameMode))
	{
		GameMode->RegisterWeapon(OnShotFired);
	}

	/* Bind UpdateRecoil to the Recoil vector curve and timeline */
	FOnTimelineVector RecoilProgressFunction;
	RecoilProgressFunction.BindUFunction(this, FName("UpdateRecoil"));
	RecoilTimeline.AddInterpVector(RecoilCurve, RecoilProgressFunction);
}

void AGun_AK47::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RecoilTimeline.TickTimeline(DeltaTime);
	if (HasMatchingGameplayTag(FBSGameplayTags::Get().State_Weapon_Recoil))
	{
		UpdateKickbackAndRecoil(DeltaTime);
	}
}

void AGun_AK47::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GameplayTags);
}

bool AGun_AK47::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return IGameplayTagAssetInterface::HasMatchingGameplayTag(TagToCheck);
}

bool AGun_AK47::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAllMatchingGameplayTags(TagContainer);
}

bool AGun_AK47::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAnyMatchingGameplayTags(TagContainer);
}

void AGun_AK47::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	SetShouldRecoil(GameSettings.bShouldRecoil);
	SetFireRate(GameSettings.bAutomaticFire);
	SetShowDecals(GameSettings.bShowBulletDecals);
	SetShowTracers(GameSettings.bShowBulletTracers);
}

void AGun_AK47::Fire()
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

void AGun_AK47::StopFire()
{
	bIsFiring = false;
	CurrentShotRecoilRotation = FRotator(0, 0, 0);
	CurrentShotCameraRecoilRotation = FRotator(0, 0, 0);
	ShotsFired = 0;
	
	/* Reverse the timeline so that it takes time to recover to the beginning */
	RecoilTimeline.SetPlayRate(5.454545f);
	RecoilTimeline.Reverse();
}

ABSCharacter* AGun_AK47::GetBSCharacter() const
{
	return Cast<ABSCharacter>(GetOwner());
}

FRotator AGun_AK47::GetCurrentRecoilRotation() const
{
	return FRotator(-CurrentShotRecoilRotation.Pitch, CurrentShotRecoilRotation.Yaw, CurrentShotRecoilRotation.Roll);
}

FVector AGun_AK47::GetMuzzleLocation() const
{
	return MuzzleLocationComp->GetComponentLocation();
}

void AGun_AK47::SetFireRate(const bool bAutomatic)
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

void AGun_AK47::SetShouldRecoil(const bool bRecoil)
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

void AGun_AK47::SetShowDecals(const bool bShowDecals)
{
	if (bShowDecals)
	{
		GameplayTags.AddTag(FBSGameplayTags().Get().State_Weapon_ShowDecals);
		return;
	}
	GameplayTags.RemoveTag(FBSGameplayTags().Get().State_Weapon_ShowDecals);
}

void AGun_AK47::SetShowTracers(const bool bShowTracers)
{
	if (bShowTracers)
	{
		GameplayTags.AddTag(FBSGameplayTags().Get().State_Weapon_ShowTracers);
		return;
	}
	GameplayTags.RemoveTag(FBSGameplayTags().Get().State_Weapon_ShowTracers);
}

void AGun_AK47::Recoil()
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

void AGun_AK47::UpdateKickbackAndRecoil(const float DeltaTime)
{
	UpdateKickback(DeltaTime);
	const FRotator Current = GetBSCharacter()->GetCamera()->GetRelativeRotation();
	const FRotator UpdatedRotation = UKismetMathLibrary::RInterpTo(Current, CurrentShotCameraRecoilRotation, DeltaTime, CameraRecoilInterpSpeed);
	GetBSCharacter()->GetCamera()->SetRelativeRotation(UpdatedRotation);
	GetBSCharacter()->GetCameraRecoilComponent()->SetRelativeRotation(FRotator(KickbackAngle, 0, 0));
}

void AGun_AK47::UpdateKickback(const float DeltaTime)
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

void AGun_AK47::UpdateRecoil(const FVector Output)
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