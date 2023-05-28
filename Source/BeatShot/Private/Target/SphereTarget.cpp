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
	HardRefAttributeSetBase->InitMaxHealth(InBSConfig.TargetConfig.Attribute_MaxHealth);
	HardRefAttributeSetBase->InitHealth(InBSConfig.TargetConfig.Attribute_MaxHealth);
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
		GetAbilitySystemComponent()->RemoveLooseGameplayTag(TagToRemove);
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
		
		/* Apply gameplay tags */
		AbilitySystemComponent->AddLooseGameplayTags(BSConfig.TargetConfig.OnSpawn_ApplyTags);

		switch (BSConfig.TargetConfig.TargetDamageType)
		{
		case ETargetDamageType::None:
		case ETargetDamageType::Hit:
			break;
		case ETargetDamageType::Tracking:
		case ETargetDamageType::Combined:
			AbilitySystemComponent->AddLooseGameplayTag(FBSGameplayTags::Get().Target_State_Tracking);
			break;
		}
		
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

	switch (BSConfig.TargetConfig.TargetActivationPolicy)
	{
	case ETargetActivationPolicy::None:
		break;
	case ETargetActivationPolicy::OnSpawn:
		ActivateTarget(BSConfig.TargetConfig.TargetMaxLifeSpan);
		break;
	case ETargetActivationPolicy::OnCooldown:
		SetColorToInactiveColor();
		break;
	}

	if (BSConfig.TargetConfig.bApplyImmunityOnSpawn)
	{
		ApplyImmunityEffect();
	}
	
	if (BSConfig.TargetConfig.bOnDamageEvent_ShrinkQuickAndGrowSlow)
	{
		/* Fade the target from ColorWhenDestroyed to BeatGridInactiveColor */
		OnShrinkQuickAndGrowSlow.BindDynamic(this, &ASphereTarget::InterpShrinkQuickAndGrowSlow);
		ShrinkQuickAndGrowSlowTimeline.AddInterpFloat(ShrinkQuickAndGrowSlowCurve, OnShrinkQuickAndGrowSlow);
	}
}

void ASphereTarget::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	StartToPeakTimeline.TickTimeline(DeltaSeconds);
	PeakToEndTimeline.TickTimeline(DeltaSeconds);
	ShrinkQuickAndGrowSlowTimeline.TickTimeline(DeltaSeconds);
}

void ASphereTarget::ActivateTarget(const float Lifespan)
{
	if (IsTargetActiveAndDamageable())
	{
		return;
	}

	if (HasMatchingGameplayTag(FBSGameplayTags::Get().Target_State_Immune))
	{
		RemoveImmunityEffect();
	}

	GetWorldTimerManager().SetTimer(DamageableWindow, this, &ASphereTarget::OnTargetMaxLifeSpanExpired, Lifespan, false);
	PlayStartToPeakTimeline();
}

void ASphereTarget::DeactivateTarget(const bool bExpired)
{
	ApplyImmunityEffect();
	SetSphereColor(BSConfig.TargetConfig.InActiveTargetColor);
	SetSphereScale(GetCurrentTargetScale() * BSConfig.TargetConfig.ConsecutiveChargeScaleMultiplier);
	StopAllTimelines();
	
	if (BSConfig.TargetConfig.bOnDamageEvent_ShrinkQuickAndGrowSlow && !bExpired)
	{
		PlayShrinkQuickAndGrowSlowTimeline();
	}

	if (BSConfig.TargetConfig.bResetPositionOnDeactivation)
	{
		SetActorLocation(InitialTargetLocation);
	}
}

void ASphereTarget::DamageSelf(const float Damage)
{
	if (UAbilitySystemComponent* Comp = GetAbilitySystemComponent())
	{
		FGameplayEffectContextHandle EffectContextHandle = Comp->MakeEffectContext();
		EffectContextHandle.Get()->AddInstigator(this, this);
		const FGameplayEffectSpecHandle Handle = Comp->MakeOutgoingSpec(ExpirationHealthPenalty, 1.f, EffectContextHandle);
		FGameplayEffectSpec* Spec = Handle.Data.Get();
		Spec->SetSetByCallerMagnitude(FBSGameplayTags::Get().Data_Damage, Damage);
		Comp->ApplyGameplayEffectSpecToSelf(*Spec);
	}
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

void ASphereTarget::SetColorToInactiveColor()
{
	SetSphereColor(BSConfig.TargetConfig.InActiveTargetColor);
	SetOutlineColor(BSConfig.TargetConfig.InActiveTargetOutlineColor);
}

void ASphereTarget::InterpStartToPeak(const float Alpha)
{
	SetSphereColor(UKismetMathLibrary::LinearColorLerp(BSConfig.TargetConfig.StartToPeak_StartColor, BSConfig.TargetConfig.StartToPeak_EndColor, Alpha));
	if (BSConfig.TargetConfig.LifetimeTargetScalePolicy == ELifetimeTargetScalePolicy::Grow)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale, BSConfig.TargetConfig.MaxTargetScale,
		                                                StartToPeakTimeline.GetPlaybackPosition() * BSConfig.AudioConfig.PlayerDelay / BSConfig.TargetConfig.TargetMaxLifeSpan)));
	}
	else if (BSConfig.TargetConfig.LifetimeTargetScalePolicy == ELifetimeTargetScalePolicy::Shrink)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale, BSConfig.TargetConfig.MinTargetScale,
		                                                StartToPeakTimeline.GetPlaybackPosition() * BSConfig.AudioConfig.PlayerDelay / BSConfig.TargetConfig.TargetMaxLifeSpan)));
	}
}

void ASphereTarget::InterpPeakToEnd(const float Alpha)
{
	SetSphereColor(UKismetMathLibrary::LinearColorLerp(BSConfig.TargetConfig.PeakToEnd_StartColor, BSConfig.TargetConfig.PeakToEnd_EndColor, Alpha));
	if (BSConfig.TargetConfig.LifetimeTargetScalePolicy == ELifetimeTargetScalePolicy::Grow)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale, BSConfig.TargetConfig.MaxTargetScale,
		                                                (PeakToEndTimeline.GetPlaybackPosition() * (BSConfig.TargetConfig.TargetMaxLifeSpan - BSConfig.AudioConfig.PlayerDelay) + BSConfig.AudioConfig.
			                                                PlayerDelay) / BSConfig.TargetConfig.TargetMaxLifeSpan)));
	}
	else if (BSConfig.TargetConfig.LifetimeTargetScalePolicy == ELifetimeTargetScalePolicy::Shrink)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale, BSConfig.TargetConfig.MinTargetScale,
		                                                (PeakToEndTimeline.GetPlaybackPosition() * (BSConfig.TargetConfig.TargetMaxLifeSpan - BSConfig.AudioConfig.PlayerDelay) + BSConfig.AudioConfig.
			                                                PlayerDelay) / BSConfig.TargetConfig.TargetMaxLifeSpan)));
	}
}

void ASphereTarget::InterpShrinkQuickAndGrowSlow(const float Alpha)
{
	SetSphereScale(FVector(UKismetMathLibrary::Lerp(MinShrinkTargetScale, InitialTargetScale, Alpha)));
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(ColorWhenDestroyed, BSConfig.TargetConfig.InActiveTargetColor, ShrinkQuickAndGrowSlowTimeline.GetPlaybackPosition());
	SetSphereColor(Color);
}

void ASphereTarget::ApplyImmunityEffect() const
{
	if (UAbilitySystemComponent* Comp = GetAbilitySystemComponent())
	{
		Comp->ApplyGameplayEffectToSelf(TargetImmunity.GetDefaultObject(), 1.f, Comp->MakeEffectContext());
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
	DamageSelf(BSConfig.TargetConfig.ExpirationHealthPenalty);
}

void ASphereTarget::OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue, const float TotalPossibleDamage)
{
	const float TimeAlive = GetWorldTimerManager().GetTimerElapsed(DamageableWindow);
	GetWorldTimerManager().ClearTimer(DamageableWindow);
	
	const bool Expired = ActorInstigator == this;

	ColorWhenDestroyed = TargetColorChangeMaterial->K2_GetVectorParameterValue("BaseColor");
	if (Expired)
	{
		OnTargetDamageEventOrTimeout.Broadcast(FTargetDamageEvent(-1, NewValue, GetActorLocation(), GetCurrentTargetScale(), GetGuid()));
	}
	else
	{
		OnTargetDamageEventOrTimeout.Broadcast(FTargetDamageEvent(TimeAlive, NewValue, GetActorLocation(), GetCurrentTargetScale(), GetGuid(),
			abs(OldValue - NewValue), TotalPossibleDamage));
		PlayExplosionEffect(SphereMesh->GetComponentLocation(), SphereTargetRadius * GetCurrentTargetScale().X, ColorWhenDestroyed);
	}
	
	/* Target Destruction Policy */
	switch (BSConfig.TargetConfig.TargetDestructionPolicy)
	{
	case ETargetDestructionPolicy::None:
	case ETargetDestructionPolicy::Persistant:
		break;
	case ETargetDestructionPolicy::OnExpiration:
		if (Expired)
		{
			Destroy();
			return;
		}
		break;
	case ETargetDestructionPolicy::OnHealthReachedZero:
		if (NewValue <= 0.f)
		{
			Destroy();
			return;
		}
		break;
	case ETargetDestructionPolicy::OnHealthReachedZeroOrExpiration:
		if (NewValue <= 0.f || Expired)
		{
			Destroy();
			return;
		}
		break;
	case ETargetDestructionPolicy::OnAnyDamageEventOrExpiration:
		Destroy();
		return;
	}
	/* Target Deactivation Policy */
	switch (BSConfig.TargetConfig.TargetDeactivationPolicy)
	{
	case ETargetDeactivationPolicy::None:
	case ETargetDeactivationPolicy::Persistant:
		break;
	case ETargetDeactivationPolicy::OnExpiration:
		if (Expired)
		{
			DeactivateTarget(Expired);
		}
		break;
	case ETargetDeactivationPolicy::OnHealthReachedZero:
		if (NewValue <= 0.f)
		{
			DeactivateTarget(Expired);
		}
		break;
	case ETargetDeactivationPolicy::OnHealthReachedZeroOrExpiration:
		if (Expired || NewValue <= 0.f)
		{
			DeactivateTarget(Expired);
		}
		break;
	case ETargetDeactivationPolicy::OnAnyDamageEventOrExpiration:
		DeactivateTarget(Expired);
		break;
	}

}

FLinearColor ASphereTarget::GetPeakTargetColor() const
{
	return BSConfig.TargetConfig.StartToPeak_EndColor;
}

FLinearColor ASphereTarget::GetEndTargetColor() const
{
	return BSConfig.TargetConfig.PeakToEnd_EndColor;
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
	if (TargetExplosion && BSConfig.TargetConfig.TargetDamageType == ETargetDamageType::Hit)
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
