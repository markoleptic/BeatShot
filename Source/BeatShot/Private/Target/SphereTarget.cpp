// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SphereTarget.h"
#include "Character/BSHealthComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "BeatShot/BSGameplayTags.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystem/AttributeSets/BSAttributeSetBase.h"
#include "Kismet/KismetMathLibrary.h"

ASphereTarget::ASphereTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule Component");
	RootComponent = CapsuleComponent;
	CapsuleComponent->SetCapsuleRadius(SphereTargetRadius);
	CapsuleComponent->SetCapsuleHalfHeight(SphereTargetRadius);

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>("Sphere Mesh");
	SphereMesh->SetupAttachment(CapsuleComponent);

	HealthComponent = CreateDefaultSubobject<UBSHealthComponent>("Health Component");

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>("Ability System Component");
	
	// Create the attribute set, this replicates by default
	// Adding it as a sub object of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	HardRefAttributeSetBase = CreateDefaultSubobject<UBSAttributeSetBase>("Attribute Set Base");
	AbilitySystemComponent->SetIsReplicated(true);

	// Minimal Mode means that no GameplayEffects will replicate. They will only live on the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	InitialLifeSpan = 1.5f;
	Guid = FGuid::NewGuid();
}

void ASphereTarget::InitTarget(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings)
{
	BSConfig = InBSConfig;
	PlayerSettings = InPlayerSettings;
	if (InBSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatGrid)
	{
		GameplayTags.AddTag(FBSGameplayTags::Get().Target_State_Grid);
	}
	else if (InBSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
	{
		GameplayTags.AddTag(FBSGameplayTags::Get().Target_State_Tracking);
		GameplayTags.AddTag(FBSGameplayTags::Get().Target_State_PreGameModeStart);
		HardRefAttributeSetBase->InitMaxHealth(1000000);
		HardRefAttributeSetBase->InitHealth(1000000);
	}
	// TEMP
	else if (InBSConfig.TargetConfig.NumCharges > 0)
	{
		HardRefAttributeSetBase->InitMaxHealth(InBSConfig.TargetConfig.NumCharges * 100.f);
		HardRefAttributeSetBase->InitHealth(InBSConfig.TargetConfig.NumCharges * 100.f);
	}
	// END TEMP
	else
	{
		HardRefAttributeSetBase->InitMaxHealth(100.f);
		HardRefAttributeSetBase->InitHealth(100.f);
	}
}

UAbilitySystemComponent* ASphereTarget::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASphereTarget::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GameplayTags);
}

bool ASphereTarget::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return IGameplayTagAssetInterface::HasMatchingGameplayTag(TagToCheck);
}

bool ASphereTarget::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAllMatchingGameplayTags(TagContainer);
}

bool ASphereTarget::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAnyMatchingGameplayTags(TagContainer);
}

void ASphereTarget::RemoveGameplayTag(const FGameplayTag TagToRemove)
{
	GameplayTags.RemoveTag(TagToRemove);
}

void ASphereTarget::UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings)
{
	PlayerSettings = InPlayerSettings;
	SetUseSeparateOutlineColor(PlayerSettings.bUseSeparateOutlineColor);
}

void ASphereTarget::SetInitialSphereScale(const FVector& NewScale)
{
	InitialTargetScale = NewScale.X;
	CapsuleComponent->SetRelativeScale3D(NewScale);
}

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(GetAbilitySystemComponent()))
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
		HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent, GameplayTags);
		HealthComponent->OnOutOfHealth.AddUObject(this, &ASphereTarget::OnOutOfHealth);
		HealthComponent->OnHealthChanged.AddUObject(this, &ASphereTarget::OnHealthChanged);
	}
	
	/* Use Color Changing Material, this is required in order to change color using C++ */
	TargetColorChangeMaterial = UMaterialInstanceDynamic::Create(SphereMesh->GetMaterial(0), this);
	SphereMesh->SetMaterial(0, TargetColorChangeMaterial);

	/* Set Outline Color */
	SetUseSeparateOutlineColor(PlayerSettings.bUseSeparateOutlineColor);

	/* Start to Peak Target Color */
	FOnTimelineFloat OnStartToPeak;
	OnStartToPeak.BindDynamic(this, &ASphereTarget::InterpStartToPeak);
	StartToPeakTimeline.AddInterpFloat(StartToPeakCurve, OnStartToPeak);

	/* Play InterpPeakToEnd when InterpStartToPeak is finished */
	FOnTimelineEvent OnStartToPeakFinished;
	OnStartToPeakFinished.BindDynamic(this, &ASphereTarget::PlayPeakToEndTimeline);
	StartToPeakTimeline.SetTimelineFinishedFunc(OnStartToPeakFinished);

	/* Peak Color to Fade Color */
	FOnTimelineFloat OnPeakToFade;
	OnPeakToFade.BindDynamic(this, &ASphereTarget::InterpPeakToEnd);
	PeakToEndTimeline.AddInterpFloat(PeakToEndCurve, OnPeakToFade);

	/* Fade the target from ColorWhenDestroyed to BeatGridInactiveColor */
	FOnTimelineFloat OnShrinkQuickAndGrowSlow;
	OnShrinkQuickAndGrowSlow.BindDynamic(this, &ASphereTarget::InterpShrinkQuickAndGrowSlow);
	ShrinkQuickAndGrowSlowTimeline.AddInterpFloat(ShrinkQuickAndGrowSlowCurve, OnShrinkQuickAndGrowSlow);

	StartToPeakTimelinePlayRate = 1 / BSConfig.AudioConfig.PlayerDelay;
	PeakToEndTimelinePlayRate = 1 / (BSConfig.TargetConfig.TargetMaxLifeSpan - BSConfig.AudioConfig.PlayerDelay);

	StartToPeakTimeline.SetPlayRate(StartToPeakTimelinePlayRate);
	PeakToEndTimeline.SetPlayRate(PeakToEndTimelinePlayRate);
	ShrinkQuickAndGrowSlowTimeline.SetPlayRate(StartToPeakTimelinePlayRate);

	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatGrid)
	{
		SetLifeSpan(0);
		ApplyImmunityEffect();
		SetColorToBeatGridColor();
		FOnTimelineEvent OnPeakToFadeFinished;
		OnPeakToFadeFinished.BindDynamic(this, &ASphereTarget::SetColorToBeatGridColor);
		PeakToEndTimeline.SetTimelineFinishedFunc(OnPeakToFadeFinished);
	}
	else if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
	{
		SetLifeSpan(0);
		SetSphereColor(PlayerSettings.EndTargetColor);
	}
	// TEMP
	else if (BSConfig.TargetConfig.NumCharges > 0)
	{
		ApplyImmunityEffect();
		SetLifeSpan(0);
		SetSphereColor(PlayerSettings.EndTargetColor);
	}
	// END TEMP
	else
	{
		SetLifeSpan(BSConfig.TargetConfig.TargetMaxLifeSpan);
		PlayStartToPeakTimeline();
		GetWorldTimerManager().SetTimer(TimeSinceSpawn, BSConfig.TargetConfig.TargetMaxLifeSpan, false);
	}
}

void ASphereTarget::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	StartToPeakTimeline.TickTimeline(DeltaSeconds);
	PeakToEndTimeline.TickTimeline(DeltaSeconds);
	ShrinkQuickAndGrowSlowTimeline.TickTimeline(DeltaSeconds);
}

void ASphereTarget::ActivateBeatGridTarget(const float Lifespan)
{
	GetWorldTimerManager().SetTimer(TimeSinceSpawn, this, &ASphereTarget::OnBeatGridTargetTimeout, Lifespan, false);
	GameplayTags.AddTag(FBSGameplayTags::Get().Target_State_Damageable);
	PlayStartToPeakTimeline();
}

void ASphereTarget::ActivateChargedTarget(const float Lifespan)
{
	if (GetWorldTimerManager().IsTimerActive(TimeSinceSpawn))
	{
		return;
	}
	GetWorldTimerManager().SetTimer(TimeSinceSpawn, this, &ASphereTarget::OnChargedTargetTimeout, Lifespan, false);
	GameplayTags.AddTag(FBSGameplayTags::Get().Target_State_Damageable);
	PlayStartToPeakTimeline();
}

void ASphereTarget::PlayStartToPeakTimeline()
{
	if (ShrinkQuickAndGrowSlowTimeline.IsPlaying())
	{
		ShrinkQuickAndGrowSlowTimeline.Stop();
	}
	if (PeakToEndTimeline.IsPlaying())
	{
		PeakToEndTimeline.Stop();
	}
	StartToPeakTimeline.PlayFromStart();
}

void ASphereTarget::PlayPeakToEndTimeline()
{
	if (StartToPeakTimeline.IsPlaying())
	{
		StartToPeakTimeline.Stop();
	}
	if (ShrinkQuickAndGrowSlowTimeline.IsPlaying())
	{
		ShrinkQuickAndGrowSlowTimeline.Stop();
	}
	PeakToEndTimeline.PlayFromStart();
}

void ASphereTarget::PlayShrinkQuickAndGrowSlowTimeline()
{
	if (StartToPeakTimeline.IsPlaying())
	{
		StartToPeakTimeline.Stop();
	}
	if (PeakToEndTimeline.IsPlaying())
	{
		PeakToEndTimeline.Stop();
	}
	ShrinkQuickAndGrowSlowTimeline.PlayFromStart();
}

void ASphereTarget::SetColorToBeatGridColor()
{
	SetSphereColor(PlayerSettings.BeatGridInactiveTargetColor);
	if (PlayerSettings.bUseSeparateOutlineColor)
	{
		SetOutlineColor(PlayerSettings.TargetOutlineColor);
	}
}

void ASphereTarget::InterpStartToPeak(const float Alpha)
{
	SetSphereColor(UKismetMathLibrary::LinearColorLerp(PlayerSettings.StartTargetColor, PlayerSettings.PeakTargetColor, Alpha));
	if (BSConfig.TargetConfig.LifetimeTargetScaleMethod == ELifetimeTargetScaleMethod::Grow)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(
			InitialTargetScale, BSConfig.TargetConfig.MaxTargetScale,
			StartToPeakTimeline.GetPlaybackPosition() * BSConfig.AudioConfig.PlayerDelay / BSConfig.TargetConfig.TargetMaxLifeSpan)));
	}
	else if (BSConfig.TargetConfig.LifetimeTargetScaleMethod == ELifetimeTargetScaleMethod::Shrink)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(
			InitialTargetScale, BSConfig.TargetConfig.MinTargetScale,
			StartToPeakTimeline.GetPlaybackPosition() * BSConfig.AudioConfig.PlayerDelay / BSConfig.TargetConfig.TargetMaxLifeSpan)));
	}
}

void ASphereTarget::InterpPeakToEnd(const float Alpha)
{
	SetSphereColor(UKismetMathLibrary::LinearColorLerp(PlayerSettings.PeakTargetColor, PlayerSettings.EndTargetColor, Alpha));
	if (BSConfig.TargetConfig.LifetimeTargetScaleMethod == ELifetimeTargetScaleMethod::Grow)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(
			InitialTargetScale, BSConfig.TargetConfig.MaxTargetScale,
			(PeakToEndTimeline.GetPlaybackPosition() * (BSConfig.TargetConfig.TargetMaxLifeSpan - BSConfig.AudioConfig.PlayerDelay) + BSConfig.AudioConfig.PlayerDelay) /
			BSConfig.TargetConfig.TargetMaxLifeSpan
			)));
	}
	else if (BSConfig.TargetConfig.LifetimeTargetScaleMethod == ELifetimeTargetScaleMethod::Shrink)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(
			InitialTargetScale, BSConfig.TargetConfig.MinTargetScale,
			(PeakToEndTimeline.GetPlaybackPosition() * (BSConfig.TargetConfig.TargetMaxLifeSpan - BSConfig.AudioConfig.PlayerDelay) + BSConfig.AudioConfig.PlayerDelay) /
			BSConfig.TargetConfig.TargetMaxLifeSpan
			)));
	}
}

void ASphereTarget::InterpShrinkQuickAndGrowSlow(const float Alpha)
{
	SetSphereScale(FVector(UKismetMathLibrary::Lerp(MinShrinkTargetScale, InitialTargetScale, Alpha)));
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(ColorWhenDestroyed, PlayerSettings.BeatGridInactiveTargetColor, ShrinkQuickAndGrowSlowTimeline.GetPlaybackPosition());
	SetSphereColor(Color);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ASphereTarget::ApplyImmunityEffect()
{
	if (UAbilitySystemComponent* Comp = GetAbilitySystemComponent())
	{
		Comp->ApplyGameplayEffectToSelf(TargetImmunity.GetDefaultObject(), 1.f, GetAbilitySystemComponent()->MakeEffectContext());
	}
}

void ASphereTarget::SetSphereColor(const FLinearColor& Color)
{
	TargetColorChangeMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
}

void ASphereTarget::SetOutlineColor(const FLinearColor& Color)
{
	TargetColorChangeMaterial->SetVectorParameterValue(TEXT("OutlineColor"), Color);
}

void ASphereTarget::LifeSpanExpired()
{
	OnLifeSpanExpired.Broadcast(true, -1, this);
	Super::LifeSpanExpired();
}

void ASphereTarget::OnOutOfHealth()
{
	UE_LOG(LogTemp, Display, TEXT("Out of health"));
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack || BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatGrid)
	{
		return;
	}

	/* Get the time that the sphere was alive for */
	const float TimeAlive = GetWorldTimerManager().GetTimerElapsed(TimeSinceSpawn);
	if (TimeAlive < 0 || TimeAlive >= BSConfig.TargetConfig.TargetMaxLifeSpan)
	{
		Destroy();
		return;
	}

	/* Broadcast that the target has been destroyed by player */
	OnLifeSpanExpired.Broadcast(false, TimeAlive, this);
	GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
	PlayExplosionEffect(SphereMesh->GetComponentLocation(), SphereTargetRadius * GetCurrentTargetScale().X, TargetColorChangeMaterial->K2_GetVectorParameterValue(TEXT("BaseColor")));
	Destroy();
}

void ASphereTarget::OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue, const float TotalPossibleDamage)
{
	UE_LOG(LogTemp, Display, TEXT("Health changed"));
	/* If BeatGrid mode, don't destroy target, make it not damageable, and play RemoveAndReappear */
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatGrid)
	{
		/* Get the time that the sphere was alive for */
		const float TimeAlive = GetWorldTimerManager().GetTimerElapsed(TimeSinceSpawn);
		GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
		if (TimeAlive < 0 || TimeAlive >= BSConfig.TargetConfig.TargetMaxLifeSpan)
		{
			return;
		}

		/* Broadcast that the target has been destroyed by player */
		PeakToEndTimeline.Stop();
		ApplyImmunityEffect();
		ColorWhenDestroyed = TargetColorChangeMaterial->K2_GetVectorParameterValue(TEXT("BaseColor"));
		PlayExplosionEffect(SphereMesh->GetComponentLocation(), SphereTargetRadius * GetCurrentTargetScale().X, ColorWhenDestroyed);
		PlayShrinkQuickAndGrowSlowTimeline();
		OnLifeSpanExpired.Broadcast(false, TimeAlive, this);
	}

	// TEMP
	if (BSConfig.TargetConfig.NumCharges > 0)
	{
		const float TimeAlive = GetWorldTimerManager().GetTimerElapsed(TimeSinceSpawn);
		GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
		ColorWhenDestroyed = TargetColorChangeMaterial->K2_GetVectorParameterValue(TEXT("BaseColor"));
		PlayExplosionEffect(SphereMesh->GetComponentLocation(), SphereTargetRadius * GetCurrentTargetScale().X, ColorWhenDestroyed);


		BSConfig.TargetConfig.NumCharges -= 1;

		if (BSConfig.TargetConfig.NumCharges == 0)
		{
			OnLifeSpanExpired.Broadcast(false, TimeAlive, this);
			Destroy();
			return;
		}
		
		ApplyImmunityEffect();
		SetSphereScale(GetCurrentTargetScale() * 0.5f);
		SetSphereColor(PlayerSettings.EndTargetColor);
		OnLifeSpanExpired.Broadcast(false, TimeAlive, this);
	}
	// END TEMP
}

FLinearColor ASphereTarget::GetPeakTargetColor() const
{
	return PlayerSettings.PeakTargetColor;
}

FLinearColor ASphereTarget::GetEndTargetColor() const
{
	return PlayerSettings.EndTargetColor;
}

void ASphereTarget::OnBeatGridTargetTimeout()
{
	ApplyImmunityEffect();
	GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
	OnLifeSpanExpired.Broadcast(true, -1, this);
}

void ASphereTarget::OnChargedTargetTimeout()
{
	ApplyImmunityEffect();
	GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
	OnLifeSpanExpired.Broadcast(true, -1, this);
	SetSphereColor(PlayerSettings.EndTargetColor);
}

void ASphereTarget::SetUseSeparateOutlineColor(const bool bUseSeparateOutlineColor)
{
	if (bUseSeparateOutlineColor)
	{
		SetOutlineColor(PlayerSettings.TargetOutlineColor);
		TargetColorChangeMaterial->SetScalarParameterValue("bUseSeparateOutlineColor", 1.f);
		return;
	}
	TargetColorChangeMaterial->SetScalarParameterValue("bUseSeparateOutlineColor", 0.f);
}

void ASphereTarget::PlayExplosionEffect(const FVector& ExplosionLocation, const float SphereRadius, const FLinearColor& InColorWhenDestroyed) const
{
	if (TargetExplosion)
	{
		UNiagaraComponent* ExplosionComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TargetExplosion, ExplosionLocation);
		ExplosionComp->SetNiagaraVariableFloat(FString("SphereRadius"), SphereRadius);
		ExplosionComp->SetColorParameter(FName("SphereColor"), InColorWhenDestroyed);
	}
}

void ASphereTarget::SetSphereScale(const FVector& NewScale) const
{
	CapsuleComponent->SetRelativeScale3D(NewScale);
}

FVector ASphereTarget::GetCurrentTargetScale() const
{
	return CapsuleComponent->GetRelativeScale3D();
}
