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

	InitialLifeSpan = 0.f;
	Guid = FGuid::NewGuid();
	MovingTargetDirection = FVector();
	MovingTargetSpeed = 0.f;
	InitialTargetScale = FVector();
	InitialTargetLocation = FVector();
	ColorWhenDestroyed = FLinearColor();
	StartToPeakTimelinePlayRate = 1.f;
	PeakToEndTimelinePlayRate = 1.f;
}

void ASphereTarget::InitTarget(const FBS_TargetConfig& InTargetConfig)
{
	Config = InTargetConfig;
	HardRefAttributeSetBase->InitMaxHealth(Config.MaxHealth);
	HardRefAttributeSetBase->InitHealth(Config.MaxHealth);
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
	SetUseSeparateOutlineColor(InPlayerSettings.bUseSeparateOutlineColor);
}

void ASphereTarget::SetInitialSphereScale(const FVector& NewScale)
{
	InitialTargetScale = NewScale;
	CapsuleComponent->SetRelativeScale3D(NewScale);
}

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();

	InitialTargetLocation = GetActorLocation();

	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
		
		/* Apply gameplay tags */
		AbilitySystemComponent->AddLooseGameplayTags(Config.OnSpawn_ApplyTags);

		switch (Config.TargetDamageType)
		{
		case ETargetDamageType::None:
		case ETargetDamageType::Hit:
			break;
		case ETargetDamageType::Tracking:
		case ETargetDamageType::Combined:
			AbilitySystemComponent->AddLooseGameplayTag(FBSGameplayTags::Get().Target_State_Tracking);
			OnTargetActivationStateChanged.AddUObject(HealthComponent, &UBSHealthComponent::SetShouldUpdateTotalPossibleDamage);
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
	StartToPeakTimelinePlayRate = 1 / Config.SpawnBeatDelay;
	PeakToEndTimelinePlayRate = 1 / (Config.TargetMaxLifeSpan -  Config.SpawnBeatDelay);
	StartToPeakTimeline.SetPlayRate(StartToPeakTimelinePlayRate);
	PeakToEndTimeline.SetPlayRate(PeakToEndTimelinePlayRate);
	ShrinkQuickAndGrowSlowTimeline.SetPlayRate(StartToPeakTimelinePlayRate);

	SetSphereColor(Config.OnSpawnColor);

	if (Config.bUseSeparateOutlineColor)
	{
		SetUseSeparateOutlineColor(true);
	}

	if (Config.bApplyImmunityOnSpawn)
	{
		ApplyImmunityEffect();
	}
	
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ShrinkQuickGrowSlow))
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

void ASphereTarget::OnTargetMaxLifeSpanExpired()
{
	ApplyImmunityEffect();
	DamageSelf(Config.ExpirationHealthPenalty);
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
	}
	
	// Destroy if conditions are met
	if (Config.TargetDestructionConditions.Contains(ETargetDestructionCondition::Persistant))
	{
	}
	else if (Expired && Config.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnExpiration))
	{
		BeginDestroy();
	}
	else if (NewValue <= 0.f && Config.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnHealthReachedZero))
    {
		BeginDestroy();
    }
	else if (Config.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnAnyExternalDamageTaken))
	{
		BeginDestroy();
	}

	// Deactivate if conditions are met
	if (Config.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		return;
	}
	if (Expired && Config.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::OnExpiration))
	{
		DeactivateTarget(Expired);
	}
	else if (!Expired && Config.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::OnAnyExternalDamageTaken))
	{
		DeactivateTarget(Expired);
	}
	else if (NewValue <= 0.f && Config.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::OnHealthReachedZero))
	{
		DeactivateTarget(Expired);
	}
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

	//Config.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::OnExpiration)
	if (Lifespan > 0)
	{
		GetWorldTimerManager().SetTimer(DamageableWindow, this, &ASphereTarget::OnTargetMaxLifeSpanExpired, Lifespan, false);
		PlayStartToPeakTimeline();
	}
	
	FGameplayTagContainer TagContainer;
	GetOwnedGameplayTags(TagContainer);
	OnTargetActivationStateChanged.Broadcast(true, TagContainer);
}

void ASphereTarget::DeactivateTarget(const bool bExpired)
{
	StopAllTimelines();
	
	FGameplayTagContainer TagContainer;
	GetOwnedGameplayTags(TagContainer);
	
	OnTargetActivationStateChanged.Broadcast(false, TagContainer);
	
	// Immunity
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::RemoveImmunity))
	{
		RemoveImmunityEffect();
	}
	else if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::AddImmunity))
	{
		ApplyImmunityEffect();
	}
	else if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ToggleImmunity))
	{
		HasMatchingGameplayTag(FBSGameplayTags::Get().Target_State_Immune) ? RemoveImmunityEffect() : ApplyImmunityEffect();
	}

	// Scale
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ResetScale))
	{
		SetSphereScale(InitialTargetScale);
	}
	else if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeScale))
	{
		SetSphereScale(GetCurrentTargetScale() * Config.ConsecutiveChargeScaleMultiplier);
	}
	
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeDirection))
	{
		// TODO: Change target direction
	}
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ResetPosition))
	{
		SetActorLocation(InitialTargetLocation);
	}
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeVelocity))
	{
		// TODO: Change target Velocity
	}
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ShrinkQuickGrowSlow) && !bExpired)
	{
		PlayShrinkQuickAndGrowSlowTimeline();
	}
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::PlayExplosionEffect) && !bExpired)
	{
		PlayExplosionEffect(SphereMesh->GetComponentLocation(), SphereTargetRadius * GetCurrentTargetScale().X, ColorWhenDestroyed);
	}

	// Colors
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ResetColorToInactiveColor))
	{
		SetSphereColor(Config.InactiveTargetColor);
	}
	else if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ResetColorToStartColor))
	{
		SetSphereColor(Config.StartColor);
	}
	else if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::Hide))
	{
		// TODO: Hide target
		SetSphereColor(FLinearColor::Transparent);
	}
	
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::Destroy) ||
	Config.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnDeactivation))
	{
		Destroy();
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
	SetSphereColor(Config.InactiveTargetColor);
}

void ASphereTarget::InterpStartToPeak(const float Alpha)
{
	SetSphereColor(UKismetMathLibrary::LinearColorLerp(Config.StartColor, Config.PeakColor, Alpha));
	if (Config.LifetimeTargetScalePolicy == ELifetimeTargetScalePolicy::Grow)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale.X, Config.MaxTargetScale,
		                                                StartToPeakTimeline.GetPlaybackPosition() * Config.SpawnBeatDelay / Config.TargetMaxLifeSpan)));
	}
	else if (Config.LifetimeTargetScalePolicy == ELifetimeTargetScalePolicy::Shrink)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale.X, Config.MinTargetScale,
		                                                StartToPeakTimeline.GetPlaybackPosition() * Config.SpawnBeatDelay / Config.TargetMaxLifeSpan)));
	}
}

void ASphereTarget::InterpPeakToEnd(const float Alpha)
{
	SetSphereColor(UKismetMathLibrary::LinearColorLerp(Config.PeakColor, Config.EndColor, Alpha));
	if (Config.LifetimeTargetScalePolicy == ELifetimeTargetScalePolicy::Grow)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale.X, Config.MaxTargetScale,
		                                                (PeakToEndTimeline.GetPlaybackPosition() * (Config.TargetMaxLifeSpan - Config.SpawnBeatDelay) + Config.SpawnBeatDelay) / Config.TargetMaxLifeSpan)));
	}
	else if (Config.LifetimeTargetScalePolicy == ELifetimeTargetScalePolicy::Shrink)
	{
		SetSphereScale(FVector(UKismetMathLibrary::Lerp(InitialTargetScale.X, Config.MinTargetScale,
		                                                (PeakToEndTimeline.GetPlaybackPosition() * (Config.TargetMaxLifeSpan - Config.SpawnBeatDelay) + Config.SpawnBeatDelay) / Config.TargetMaxLifeSpan)));
	}
}

void ASphereTarget::InterpShrinkQuickAndGrowSlow(const float Alpha)
{
	SetSphereScale(FVector(UKismetMathLibrary::Lerp(MinShrinkTargetScale, InitialTargetScale.X, Alpha)));
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(ColorWhenDestroyed, Config.InactiveTargetColor, ShrinkQuickAndGrowSlowTimeline.GetPlaybackPosition());
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

FLinearColor ASphereTarget::GetPeakTargetColor() const
{
	return Config.PeakColor;
}

FLinearColor ASphereTarget::GetEndTargetColor() const
{
	return Config.EndColor;
}

bool ASphereTarget::IsTargetActiveAndDamageable() const
{
	return GetWorldTimerManager().IsTimerActive(DamageableWindow) && !HasMatchingGameplayTag(FBSGameplayTags::Get().Target_State_Immune);
}

void ASphereTarget::SetMovingTargetDirection(const FVector& NewDirection)
{
	MovingTargetDirection = NewDirection;
}

void ASphereTarget::SetMovingTargetSpeed(const float NewMovingTargetSpeed)
{
	MovingTargetSpeed = NewMovingTargetSpeed;
}

void ASphereTarget::SetUseSeparateOutlineColor(const bool bUseSeparateOutlineColor)
{
	if (bUseSeparateOutlineColor)
	{
		SetOutlineColor(Config.OutlineColor);
		TargetColorChangeMaterial->SetScalarParameterValue("bUseSeparateOutlineColor", 1.f);
		return;
	}
	TargetColorChangeMaterial->SetScalarParameterValue("bUseSeparateOutlineColor", 0.f);
}

void ASphereTarget::PlayExplosionEffect(const FVector& ExplosionLocation, const float SphereRadius, const FLinearColor& InColorWhenDestroyed) const
{
	if (TargetExplosion && Config.TargetDamageType == ETargetDamageType::Hit)
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
