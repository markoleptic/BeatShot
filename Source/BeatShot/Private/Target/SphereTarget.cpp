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
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


ASphereTarget::ASphereTarget()
{
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule Component");
		CapsuleComponent->SetCapsuleRadius(SphereTargetRadius);
		CapsuleComponent->SetCapsuleHalfHeight(SphereTargetRadius);
		RootComponent = CapsuleComponent;
	}

	if (!SphereMesh)
	{
		SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>("Sphere Mesh");
		SphereMesh->SetupAttachment(CapsuleComponent);
	}

	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
		ProjectileMovementComponent->SetUpdatedComponent(CapsuleComponent);
		ProjectileMovementComponent->InitialSpeed = 0.0f;
		ProjectileMovementComponent->MaxSpeed = 0.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = false;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 1.f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
		ProjectileMovementComponent->Velocity = FVector(0.f);
	}

	if (!HealthComponent)
	{
		HealthComponent = CreateDefaultSubobject<UBSHealthComponent>("Health Component");
	}

	// Create ability system component, and set it to be explicitly replicated
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>("Ability System Component");
		// Minimal Mode means that no GameplayEffects will replicate. They will only live on the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	}
	
	// Create the attribute set, this replicates by default
	// Adding it as a sub object of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	if (!HardRefAttributeSetBase)
	{
		HardRefAttributeSetBase = CreateDefaultSubobject<UBSAttributeSetBase>("Attribute Set Base");
		AbilitySystemComponent->SetIsReplicated(true);
	}
	
	InitialLifeSpan = 0.f;
	Guid = FGuid::NewGuid();
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
	
	InitialTargetScale = GetActorScale();
	InitialTargetLocation = GetActorLocation();
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

void ASphereTarget::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	const FVector NewVelocity = ProjectileMovementComponent->Velocity.GetSafeNormal() * ProjectileMovementComponent->InitialSpeed;
	ProjectileMovementComponent->Velocity = NewVelocity;
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

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();
	
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

	if (!Config.bMoveTargets && ProjectileMovementComponent)
	{
		ProjectileMovementComponent->Deactivate();
	}

	if (ProjectileMovementComponent->IsActive())
	{
		ProjectileMovementComponent->InitialSpeed = Config.MinTargetSpeed;
	}
	
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ShrinkQuickGrowSlow))
	{
		/* Fade the target from ColorWhenDestroyed to BeatGridInactiveColor */
		OnShrinkQuickAndGrowSlow.BindDynamic(this, &ASphereTarget::InterpShrinkQuickAndGrowSlow);
		ShrinkQuickAndGrowSlowTimeline.AddInterpFloat(ShrinkQuickAndGrowSlowCurve, OnShrinkQuickAndGrowSlow);
	}
}

void ASphereTarget::PostInitializeComponents()
{
	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
		
		HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
		HealthComponent->OnHealthChanged.AddUObject(this, &ASphereTarget::OnHealthChanged);
		GetAbilitySystemComponent()->OnImmunityBlockGameplayEffectDelegate.AddUObject(this, &ASphereTarget::OnImmunityBlockGameplayEffect);

		switch (Config.TargetDamageType)
		{
		case ETargetDamageType::None:
		case ETargetDamageType::Hit:
			GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(TrackingImmunity.GetDefaultObject(), 1.f, GetAbilitySystemComponent()->MakeEffectContext());
			break;
		case ETargetDamageType::Tracking:
			GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(FireGunImmunity.GetDefaultObject(), 1.f, GetAbilitySystemComponent()->MakeEffectContext());
			break;
		case ETargetDamageType::Combined:
			break;
		}
		
		if (Config.bApplyImmunityOnSpawn)
		{
			ApplyImmunityEffect();
		}
	}

	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &ASphereTarget::OnProjectileBounce);
		if (!Config.bMoveTargetsForward)
		{
			ProjectileMovementComponent->bConstrainToPlane = true;
			ProjectileMovementComponent->SetPlaneConstraintNormal(FVector(1.f, 0.f, 0.f));
		}
	}
	
	Super::PostInitializeComponents();
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
	DamageSelf(Config.ExpirationHealthPenalty);
}

void ASphereTarget::OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue)
{
	const float TimeAlive = ActorInstigator == this ? -1.f : GetWorldTimerManager().GetTimerElapsed(DamageableWindow);
	GetWorldTimerManager().ClearTimer(DamageableWindow);
	const FTargetDamageEvent TargetDamageEvent(TimeAlive, NewValue, GetActorTransform(), GetGuid(), abs(OldValue - NewValue));
	ColorWhenDestroyed = TargetColorChangeMaterial->K2_GetVectorParameterValue("BaseColor");
	HandleDeactivation(ActorInstigator == this, NewValue);
	OnTargetDamageEventOrTimeout.Broadcast(TargetDamageEvent);
	bCanBeReactivated = true;
	HandleDestruction(ActorInstigator == this, NewValue);
}

bool ASphereTarget::ActivateTarget(const float Lifespan)
{
	if (!bCanBeReactivated)
	{
		return false;
	}
	if (GetWorldTimerManager().GetTimerRemaining(DamageableWindow) > 0.f)
	{
		return false;
	}
	if (Lifespan > 0)
	{
		GetWorldTimerManager().ClearTimer(DamageableWindow);
		GetWorldTimerManager().SetTimer(DamageableWindow, this, &ASphereTarget::OnTargetMaxLifeSpanExpired, Lifespan, false);
		PlayStartToPeakTimeline();
	}
	bCanBeReactivated = false;
	return true;
}

void ASphereTarget::HandleDeactivation(const bool bExpired, const float CurrentHealth)
{
	if (ShouldDeactivate(bExpired, CurrentHealth))
	{
		StopAllTimelines();
		HandleDeactivationResponses(bExpired);
	}
}

void ASphereTarget::HandleDestruction(const bool bExpired, const float CurrentHealth)
{
	if (ShouldDestroy(bExpired, CurrentHealth))
	{
		Destroy();
	}
}

bool ASphereTarget::ShouldDeactivate(const bool bExpired, const float CurrentHealth) const
{
	if (Config.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		return false;
	}
	if (bExpired && Config.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::OnExpiration))
	{
		return true;
	}
	if (!bExpired && Config.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::OnAnyExternalDamageTaken))
	{
		return true;
	}
	return false;
}

bool ASphereTarget::ShouldDestroy(const bool bExpired, const float CurrentHealth) const
{
	if (Config.TargetDestructionConditions.Contains(ETargetDestructionCondition::Persistant))
	{
		return false;
	}
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::Destroy))
	{
		return true;
	}
	if (Config.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnDeactivation))
	{
		return true;
	}
	if (Config.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnExpiration) && bExpired)
	{
		return true;
	}
	if (Config.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnHealthReachedZero) && CurrentHealth <= 0.f)
	{
		return true;
	}
	if (Config.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnAnyExternalDamageTaken) && !bExpired)
	{
		return true;
	}
	return false;
}

void ASphereTarget::HandleDeactivationResponses(const bool bExpired)
{
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
		IsTargetImmune() ? RemoveImmunityEffect() : ApplyImmunityEffect();
	}

	// Scale
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ResetScale))
	{
		SetSphereScale(InitialTargetScale);
	}
	else if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ApplyDeactivatedTargetScaleMultiplier))
	{
		SetSphereScale(GetCurrentTargetScale() * Config.ConsecutiveChargeScaleMultiplier);
	}

	// Position and Velocity
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ResetPosition))
	{
		SetActorLocation(InitialTargetLocation);
	}

	// Effects
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
		if (!IsTargetImmune())
		{
			Comp->ApplyGameplayEffectToSelf(TargetImmunity.GetDefaultObject(), 1.f, Comp->MakeEffectContext());
		}
	}
}

void ASphereTarget::RemoveImmunityEffect() const
{
	if (UAbilitySystemComponent* Comp = GetAbilitySystemComponent())
	{
		if (IsTargetImmune())
		{
			Comp->RemoveActiveGameplayEffectBySourceEffect(TargetImmunity, Comp);
		}
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

FLinearColor ASphereTarget::GetInActiveTargetColor() const
{
	return Config.InactiveTargetColor;
}

bool ASphereTarget::IsTargetImmune() const
{
	return AbilitySystemComponent->HasExactMatchingGameplayTag(FBSGameplayTags::Get().Target_State_Immune);
}

bool ASphereTarget::IsTargetImmuneToTracking() const
{
	return AbilitySystemComponent->HasExactMatchingGameplayTag(FBSGameplayTags::Get().Target_State_Immune) ||
		AbilitySystemComponent->HasExactMatchingGameplayTag(FBSGameplayTags::Get().Target_State_Immune_Tracking);
}

bool ASphereTarget::IsDamageWindowActive() const
{
	return GetWorldTimerManager().IsTimerActive(DamageableWindow);
}

FVector ASphereTarget::GetTargetDirection() const
{
	if (ProjectileMovementComponent->IsActive())
	{
		return ProjectileMovementComponent->Velocity / ProjectileMovementComponent->InitialSpeed;
	}
	return FVector(0.f);
}

float ASphereTarget::GetTargetSpeed() const
{
	if (ProjectileMovementComponent->IsActive())
	{
		return ProjectileMovementComponent->InitialSpeed;
	}
	return 0.f;
}

FVector ASphereTarget::GetTargetVelocity() const
{
	if (ProjectileMovementComponent->IsActive())
	{
		return ProjectileMovementComponent->Velocity;
	}
	return FVector(0.f);
}

void ASphereTarget::SetTargetDirection(const FVector& NewDirection) const
{
	if (ProjectileMovementComponent->IsActive())
	{
		ProjectileMovementComponent->Velocity = NewDirection * ProjectileMovementComponent->InitialSpeed;
	}
}

void ASphereTarget::SetTargetSpeed(const float NewMovingTargetSpeed) const
{
	if (ProjectileMovementComponent->IsActive())
	{
		ProjectileMovementComponent->InitialSpeed = NewMovingTargetSpeed;
	}
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
