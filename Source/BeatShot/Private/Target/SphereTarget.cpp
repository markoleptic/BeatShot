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
	NumCharges = InBSConfig.TargetConfig.NumCharges;

	switch (InBSConfig.DefiningConfig.BaseGameMode)
	{
	case EBaseGameMode::None:
		break;
	case EBaseGameMode::SingleBeat:
	case EBaseGameMode::MultiBeat:
	case EBaseGameMode::BeatGrid:
		HardRefAttributeSetBase->InitMaxHealth(100.f);
		HardRefAttributeSetBase->InitHealth(100.f);
		break;
	case EBaseGameMode::BeatTrack:
		HardRefAttributeSetBase->InitMaxHealth(1000000);
		HardRefAttributeSetBase->InitHealth(1000000);
		break;
	case EBaseGameMode::ChargedBeatTrack:
		HardRefAttributeSetBase->InitMaxHealth(InBSConfig.TargetConfig.NumCharges * 100.f);
		HardRefAttributeSetBase->InitHealth(InBSConfig.TargetConfig.NumCharges * 100.f);
		break;
	}
}

UAbilitySystemComponent* ASphereTarget::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASphereTarget::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	GetAbilitySystemComponent()->GetOwnedGameplayTags(TagContainer);
}

bool ASphereTarget::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(TagToCheck);
}

bool ASphereTarget::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GetAbilitySystemComponent()->HasAllMatchingGameplayTags(TagContainer);
}

bool ASphereTarget::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GetAbilitySystemComponent()->HasAnyMatchingGameplayTags(TagContainer);
}

void ASphereTarget::RemoveGameplayTag(const FGameplayTag TagToRemove) const
{
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(TagToRemove))
	{
		GetAbilitySystemComponent()->RemoveReplicatedLooseGameplayTag(TagToRemove);
	}
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
	
	InitialTargetLocation = GetActorLocation();
	SetLifeSpan(0);

	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
		FGameplayTagContainer Container;
		GetOwnedGameplayTags(Container);
		HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent, Container);
		HealthComponent->OnHealthChanged.AddUObject(this, &ASphereTarget::OnHealthChanged);
		GetAbilitySystemComponent()->OnImmunityBlockGameplayEffectDelegate.AddUObject(this, &ASphereTarget::OnImmunityBlockGameplayEffect);
	}

	/* Use Color Changing Material, this is required in order to change color using C++ */
	TargetColorChangeMaterial = UMaterialInstanceDynamic::Create(SphereMesh->GetMaterial(0), this);
	SphereMesh->SetMaterial(0, TargetColorChangeMaterial);

	/* Set Outline Color */
	SetUseSeparateOutlineColor(PlayerSettings.bUseSeparateOutlineColor);

	/* Start to Peak Target Color */
	OnStartToPeak.BindDynamic(this, &ASphereTarget::InterpStartToPeak);
	StartToPeakTimeline.AddInterpFloat(StartToPeakCurve, OnStartToPeak);

	/* Play InterpPeakToEnd when InterpStartToPeak is finished */
	OnStartToPeakFinished.BindDynamic(this, &ASphereTarget::PlayPeakToEndTimeline);
	StartToPeakTimeline.SetTimelineFinishedFunc(OnStartToPeakFinished);

	/* Peak Color to Fade Color */
	OnPeakToFade.BindDynamic(this, &ASphereTarget::InterpPeakToEnd);
	PeakToEndTimeline.AddInterpFloat(PeakToEndCurve, OnPeakToFade);

	/* Set the playback rates based on TargetMaxLifeSpan */
	StartToPeakTimelinePlayRate = 1 / BSConfig.AudioConfig.PlayerDelay;
	PeakToEndTimelinePlayRate = 1 / (BSConfig.TargetConfig.TargetMaxLifeSpan - BSConfig.AudioConfig.PlayerDelay);
	StartToPeakTimeline.SetPlayRate(StartToPeakTimelinePlayRate);
	PeakToEndTimeline.SetPlayRate(PeakToEndTimelinePlayRate);
	ShrinkQuickAndGrowSlowTimeline.SetPlayRate(StartToPeakTimelinePlayRate);
	
	switch (BSConfig.DefiningConfig.BaseGameMode)
	{
	case EBaseGameMode::None:
		break;
	case EBaseGameMode::SingleBeat:
	case EBaseGameMode::MultiBeat:
		{
			PlayStartToPeakTimeline();
			GetWorldTimerManager().SetTimer(DamageableWindow, this, &ASphereTarget::OnTargetMaxLifeSpanExpired, BSConfig.TargetConfig.TargetMaxLifeSpan, false);
		}
		break;
	case EBaseGameMode::BeatGrid:
		{
			ApplyImmunityEffect();
			SetColorToBeatGridColor();
			
			/* Set color to BeatGridInactiveColor after PeakToFade has finished playing */
			OnPeakToFadeFinished.BindDynamic(this, &ASphereTarget::SetColorToBeatGridColor);
			PeakToEndTimeline.SetTimelineFinishedFunc(OnPeakToFadeFinished);
			
			/* Fade the target from ColorWhenDestroyed to BeatGridInactiveColor */
			OnShrinkQuickAndGrowSlow.BindDynamic(this, &ASphereTarget::InterpShrinkQuickAndGrowSlow);
			ShrinkQuickAndGrowSlowTimeline.AddInterpFloat(ShrinkQuickAndGrowSlowCurve, OnShrinkQuickAndGrowSlow);
			AbilitySystemComponent->AddReplicatedLooseGameplayTag(FBSGameplayTags::Get().Target_State_Grid);
		}
		break;
	case EBaseGameMode::BeatTrack:
		{
			ApplyImmunityEffect();
			SetSphereColor(PlayerSettings.EndTargetColor);
			AbilitySystemComponent->AddReplicatedLooseGameplayTag(FBSGameplayTags::Get().Target_State_Tracking);
		}
		break;
	case EBaseGameMode::ChargedBeatTrack:
		{
			ApplyImmunityEffect();
			SetSphereColor(PlayerSettings.EndTargetColor);
			AbilitySystemComponent->AddReplicatedLooseGameplayTag(FBSGameplayTags::Get().Target_State_Charged);
		}
		break;
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
	GetWorldTimerManager().SetTimer(DamageableWindow, this, &ASphereTarget::OnTargetMaxLifeSpanExpired, Lifespan, false);
	RemoveImmunityEffect();
	PlayStartToPeakTimeline();
}

void ASphereTarget::ActivateChargedTarget(const float Lifespan)
{
	if (IsTargetActiveAndDamageable())
	{
		return;
	}
	GetWorldTimerManager().SetTimer(DamageableWindow, this, &ASphereTarget::OnTargetMaxLifeSpanExpired, Lifespan, false);
	RemoveImmunityEffect();
	PlayStartToPeakTimeline();
}

void ASphereTarget::PlayStartToPeakTimeline()
{
	StopAllTimelines();
	StartToPeakTimeline.PlayFromStart();
}

void ASphereTarget::PlayPeakToEndTimeline()
{
	StopAllTimelines();
	PeakToEndTimeline.PlayFromStart();
}

void ASphereTarget::PlayShrinkQuickAndGrowSlowTimeline()
{
	StopAllTimelines();
	ShrinkQuickAndGrowSlowTimeline.PlayFromStart();
}

void ASphereTarget::StopAllTimelines()
{
	if (StartToPeakTimeline.IsPlaying())
	{
		StartToPeakTimeline.Stop();
	}
	if (ShrinkQuickAndGrowSlowTimeline.IsPlaying())
	{
		ShrinkQuickAndGrowSlowTimeline.Stop();
	}
	if (PeakToEndTimeline.IsPlaying())
	{
		PeakToEndTimeline.Stop();
	}
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
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale, BSConfig.TargetConfig.MaxTargetScale,
		                                                StartToPeakTimeline.GetPlaybackPosition() * BSConfig.AudioConfig.PlayerDelay / BSConfig.TargetConfig.TargetMaxLifeSpan)));
	}
	else if (BSConfig.TargetConfig.LifetimeTargetScaleMethod == ELifetimeTargetScaleMethod::Shrink)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale, BSConfig.TargetConfig.MinTargetScale,
		                                                StartToPeakTimeline.GetPlaybackPosition() * BSConfig.AudioConfig.PlayerDelay / BSConfig.TargetConfig.TargetMaxLifeSpan)));
	}
}

void ASphereTarget::InterpPeakToEnd(const float Alpha)
{
	SetSphereColor(UKismetMathLibrary::LinearColorLerp(PlayerSettings.PeakTargetColor, PlayerSettings.EndTargetColor, Alpha));
	if (BSConfig.TargetConfig.LifetimeTargetScaleMethod == ELifetimeTargetScaleMethod::Grow)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale, BSConfig.TargetConfig.MaxTargetScale,
		                                                (PeakToEndTimeline.GetPlaybackPosition() * (BSConfig.TargetConfig.TargetMaxLifeSpan - BSConfig.AudioConfig.PlayerDelay) + BSConfig.AudioConfig.
			                                                PlayerDelay) / BSConfig.TargetConfig.TargetMaxLifeSpan)));
	}
	else if (BSConfig.TargetConfig.LifetimeTargetScaleMethod == ELifetimeTargetScaleMethod::Shrink)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale, BSConfig.TargetConfig.MinTargetScale,
		                                                (PeakToEndTimeline.GetPlaybackPosition() * (BSConfig.TargetConfig.TargetMaxLifeSpan - BSConfig.AudioConfig.PlayerDelay) + BSConfig.AudioConfig.
			                                                PlayerDelay) / BSConfig.TargetConfig.TargetMaxLifeSpan)));
	}
}

void ASphereTarget::InterpShrinkQuickAndGrowSlow(const float Alpha)
{
	SetSphereScale(FVector(UKismetMathLibrary::Lerp(MinShrinkTargetScale, InitialTargetScale, Alpha)));
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(ColorWhenDestroyed, PlayerSettings.BeatGridInactiveTargetColor, ShrinkQuickAndGrowSlowTimeline.GetPlaybackPosition());
	SetSphereColor(Color);
}

void ASphereTarget::ApplyImmunityEffect() const
{
	if (UAbilitySystemComponent* Comp = GetAbilitySystemComponent())
	{
		Comp->ApplyGameplayEffectToSelf(TargetImmunity.GetDefaultObject(), 1.f, GetAbilitySystemComponent()->MakeEffectContext());
	}
}

void ASphereTarget::RemoveImmunityEffect() const
{
	if (UAbilitySystemComponent* Comp = GetAbilitySystemComponent())
	{
		Comp->RemoveActiveEffectsWithTags(FGameplayTagContainer(FBSGameplayTags::Get().Target_State_Immune));
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

void ASphereTarget::OnTargetMaxLifeSpanExpired()
{
	ApplyImmunityEffect();
	GetWorldTimerManager().ClearTimer(DamageableWindow);
	
	OnTargetDamageEventOrTimeout.Broadcast(FTargetDamageEvent(-1, -1, -1,
		GetActorLocation(), GetCurrentTargetScale(), GetGuid(), GetNumCharges()));

	switch (BSConfig.DefiningConfig.BaseGameMode)
	{
	case EBaseGameMode::None:
	case EBaseGameMode::SingleBeat:
	case EBaseGameMode::MultiBeat:
	case EBaseGameMode::BeatTrack:
		Destroy();
		break;
	case EBaseGameMode::BeatGrid:
		OnHealthChanged_BeatGrid();
		break;
	case EBaseGameMode::ChargedBeatTrack:
		OnHealthChanged_ChargedBeatTrack();
		break;
	}
}

void ASphereTarget::OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue, const float TotalPossibleDamage)
{
	const FTargetDamageEvent Event(GetWorldTimerManager().GetTimerElapsed(DamageableWindow),abs(OldValue - NewValue),
		TotalPossibleDamage, GetActorLocation(), GetCurrentTargetScale(), GetGuid(), GetNumCharges());

	ApplyImmunityEffect();
	GetWorldTimerManager().ClearTimer(DamageableWindow);
	ColorWhenDestroyed = TargetColorChangeMaterial->K2_GetVectorParameterValue("BaseColor");
	PlayExplosionEffect(SphereMesh->GetComponentLocation(), SphereTargetRadius * GetCurrentTargetScale().X, ColorWhenDestroyed);
	
	OnTargetDamageEventOrTimeout.Broadcast(Event);

	switch (BSConfig.DefiningConfig.BaseGameMode)
	{
	case EBaseGameMode::None:
		break;
	case EBaseGameMode::SingleBeat:
	case EBaseGameMode::MultiBeat:
		if (NewValue <= 0.f)
		{
			Destroy();
		}
		break;
	case EBaseGameMode::BeatTrack:
		break;
	case EBaseGameMode::BeatGrid:
		OnHealthChanged_BeatGrid();
		break;
	case EBaseGameMode::ChargedBeatTrack:
		OnHealthChanged_ChargedBeatTrack();
		break;
	}
}

void ASphereTarget::OnHealthChanged_BeatGrid()
{
	SetActorLocation(InitialTargetLocation);
	PlayShrinkQuickAndGrowSlowTimeline();
}

void ASphereTarget::OnHealthChanged_ChargedBeatTrack()
{
	NumCharges -= 1;
	if (NumCharges <= 0)
	{
		Destroy();
		return;
	}
	StopAllTimelines();
	SetSphereScale(GetCurrentTargetScale() * BSConfig.TargetConfig.ConsecutiveChargeScaleMultiplier);
	SetSphereColor(PlayerSettings.EndTargetColor);
}

FLinearColor ASphereTarget::GetPeakTargetColor() const
{
	return PlayerSettings.PeakTargetColor;
}

FLinearColor ASphereTarget::GetEndTargetColor() const
{
	return PlayerSettings.EndTargetColor;
}

bool ASphereTarget::IsTargetActiveAndDamageable() const
{
	return GetWorldTimerManager().IsTimerActive(DamageableWindow) && !HasMatchingGameplayTag(FBSGameplayTags::Get().Target_State_Immune);
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

void ASphereTarget::OnImmunityBlockGameplayEffect(const FGameplayEffectSpec& Spec, const FActiveGameplayEffect* Effect)
{
	UE_LOG(LogTemp, Display, TEXT("Blocked tag"));
}
