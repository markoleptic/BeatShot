// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SphereTarget.h"
#include "BSHealthComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "BeatShot/BSGameplayTags.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameplayAbility/AttributeSets/BSAttributeSetBase.h"
#include "Kismet/KismetMathLibrary.h"

ASphereTarget::ASphereTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule Component");
	RootComponent = CapsuleComponent;

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
	if (InBSConfig.DefiningConfig.BaseGameMode == EDefaultMode::BeatGrid)
	{
		GameplayTags.AddTag(FBSGameplayTags::Get().Target_State_Grid);
	}
	else if (InBSConfig.DefiningConfig.BaseGameMode == EDefaultMode::BeatTrack)
	{
		GameplayTags.AddTag(FBSGameplayTags::Get().Target_State_Tracking);
		GameplayTags.AddTag(FBSGameplayTags::Get().Target_State_PreGameModeStart);
		HardRefAttributeSetBase->InitMaxHealth(1000000);
		HardRefAttributeSetBase->InitHealth(1000000);
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

void ASphereTarget::SetSphereScale(const FVector& NewScale)
{
	TargetScale = NewScale.X;
	CapsuleComponent->SetRelativeScale3D(NewScale);
}

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(GetAbilitySystemComponent()))
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
		HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent, GameplayTags);
		HealthComponent->OnOutOfHealth.BindUObject(this, &ASphereTarget::HandleDestruction);
		if (BSConfig.DefiningConfig.BaseGameMode == EDefaultMode::BeatGrid)
		{
			HealthComponent->OnHealthChanged.AddUObject(this, &ASphereTarget::HandleTemporaryDestruction);
		}
	}
	
	/* Use Color Changing Material, this is required in order to change color using C++ */
	Material = SphereMesh->GetMaterial(0);
	MID_TargetColorChanger = UMaterialInstanceDynamic::Create(Material, this);
	SphereMesh->SetMaterial(0, MID_TargetColorChanger);

	/* Set Outline Color */
	SetUseSeparateOutlineColor(PlayerSettings.bUseSeparateOutlineColor);
	
	const float StartToPeakMultiplier = 1 / BSConfig.AudioConfig.PlayerDelay;
	const float PeakToFadeMultiplier = 1 / (BSConfig.TargetConfig.TargetMaxLifeSpan - BSConfig.AudioConfig.PlayerDelay);

	/* Start to Peak Target Color */
	FOnTimelineFloat OnStartToPeak;
	OnStartToPeak.BindDynamic(this, &ASphereTarget::InterpStartToPeakColor);
	StartToPeakTimeline.AddInterpFloat(StartToPeakCurve, OnStartToPeak);

	/* Play InterpPeakToEndColor when InterpStartToPeakColor is finished */
	FOnTimelineEvent OnStartToPeakFinished;
	OnStartToPeakFinished.BindDynamic(this, &ASphereTarget::PlayPeakToEndTimeline);
	StartToPeakTimeline.SetTimelineFinishedFunc(OnStartToPeakFinished);

	/* Peak Color to Fade Color */
	FOnTimelineFloat OnPeakToFade;
	OnPeakToFade.BindDynamic(this, &ASphereTarget::InterpPeakToEndColor);
	PeakToEndTimeline.AddInterpFloat(PeakToEndCurve, OnPeakToFade);

	/* Fade the target from transparent to BeatGridInactiveColor */
	FOnTimelineFloat OnShrinkQuickAndGrowSlow;
	OnShrinkQuickAndGrowSlow.BindDynamic(this, &ASphereTarget::InterpShrinkQuickAndGrowSlow);
	ShrinkQuickAndGrowSlowTimeline.AddInterpFloat(ShrinkQuickAndGrowSlowCurve, OnShrinkQuickAndGrowSlow);
	
	// FOnTimelineEvent OnFadeAndReappearFinished;
	// OnFadeAndReappearFinished.BindDynamic(this, &ASphereTarget::ASphereTarget::SetUseSeparateOutlineColor);
	// ShrinkQuickAndGrowSlowTimeline.SetTimelineFinishedFunc(OnFadeAndReappearFinished);

	StartToPeakTimeline.SetPlayRate(StartToPeakMultiplier);
	PeakToEndTimeline.SetPlayRate(PeakToFadeMultiplier);
	ShrinkQuickAndGrowSlowTimeline.SetPlayRate(StartToPeakMultiplier);

	if (BSConfig.DefiningConfig.BaseGameMode == EDefaultMode::BeatGrid)
	{
		SetLifeSpan(0);
		ApplyImmunityEffect();
		SetColorToBeatGridColor();
		FOnTimelineEvent OnPeakToFadeFinished;
		OnPeakToFadeFinished.BindDynamic(this, &ASphereTarget::SetColorToBeatGridColor);
		PeakToEndTimeline.SetTimelineFinishedFunc(OnPeakToFadeFinished);
	}
	else if (BSConfig.DefiningConfig.BaseGameMode == EDefaultMode::BeatTrack)
	{
		SetLifeSpan(0);
		SetSphereColor(PlayerSettings.EndTargetColor);
	}
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

void ASphereTarget::StartBeatGridTimer(const float Lifespan)
{
	GetWorldTimerManager().SetTimer(TimeSinceSpawn, this, &ASphereTarget::OnBeatGridTimerCompleted, Lifespan, false);
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

void ASphereTarget::InterpStartToPeakColor(const float Alpha)
{
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(PlayerSettings.StartTargetColor, PlayerSettings.PeakTargetColor, Alpha);
	SetSphereColor(Color);
}

void ASphereTarget::InterpPeakToEndColor(const float Alpha)
{
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(PlayerSettings.PeakTargetColor, PlayerSettings.EndTargetColor, Alpha);
	SetSphereColor(Color);
}

void ASphereTarget::InterpShrinkQuickAndGrowSlow(const float Alpha)
{
	CapsuleComponent->SetRelativeScale3D(FVector(UKismetMathLibrary::Lerp(Constants::MinShrinkTargetScale, TargetScale, Alpha)));
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
	MID_TargetColorChanger->SetVectorParameterValue(TEXT("BaseColor"), Color);
}

void ASphereTarget::SetOutlineColor(const FLinearColor& Color)
{
	MID_TargetColorChanger->SetVectorParameterValue(TEXT("OutlineColor"), Color);
}

void ASphereTarget::LifeSpanExpired()
{
	OnLifeSpanExpired.Broadcast(true, -1, this);
	Super::LifeSpanExpired();
}

void ASphereTarget::HandleDestruction()
{
	if (BSConfig.DefiningConfig.BaseGameMode == EDefaultMode::BeatTrack || BSConfig.DefiningConfig.BaseGameMode == EDefaultMode::BeatGrid)
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
	PlayExplosionEffect(SphereMesh->GetComponentLocation(), Constants::SphereRadius * TargetScale, MID_TargetColorChanger->K2_GetVectorParameterValue(TEXT("BaseColor")));
	Destroy();
}

void ASphereTarget::HandleTemporaryDestruction(AActor* ActorInstigator, const float OldValue, const float NewValue, const float TotalPossibleDamage)
{
	/* If BeatGrid mode, don't destroy target, make it not damageable, and play RemoveAndReappear */
	if (BSConfig.DefiningConfig.BaseGameMode == EDefaultMode::BeatGrid)
	{
		/* Get the time that the sphere was alive for */
		const float TimeAlive = GetWorldTimerManager().GetTimerElapsed(TimeSinceSpawn);
		if (TimeAlive < 0 || TimeAlive >= BSConfig.TargetConfig.TargetMaxLifeSpan)
		{
			return;
		}

		/* Broadcast that the target has been destroyed by player */
		PeakToEndTimeline.Stop();
		GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
		ApplyImmunityEffect();
		PlayExplosionEffect(SphereMesh->GetComponentLocation(), Constants::SphereRadius * TargetScale, MID_TargetColorChanger->K2_GetVectorParameterValue(TEXT("BaseColor")));
		PlayShrinkQuickAndGrowSlowTimeline();
		OnLifeSpanExpired.Broadcast(false, TimeAlive, this);
	}
}

FLinearColor ASphereTarget::GetPeakTargetColor() const
{
	return PlayerSettings.PeakTargetColor;
}

FLinearColor ASphereTarget::GetEndTargetColor() const
{
	return PlayerSettings.EndTargetColor;
}

void ASphereTarget::OnBeatGridTimerCompleted()
{
	ApplyImmunityEffect();
	GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
	OnLifeSpanExpired.Broadcast(true, -1, this);
}

void ASphereTarget::SetUseSeparateOutlineColor(const bool bUseSeparateOutlineColor)
{
	if (bUseSeparateOutlineColor)
	{
		SetOutlineColor(PlayerSettings.TargetOutlineColor);
		MID_TargetColorChanger->SetScalarParameterValue("bUseSeparateOutlineColor", 1.f);
		return;
	}
	MID_TargetColorChanger->SetScalarParameterValue("bUseSeparateOutlineColor", 0.f);
}

void ASphereTarget::PlayExplosionEffect(const FVector& ExplosionLocation, const float SphereRadius, const FLinearColor& ColorWhenDestroyed) const
{
	if (TargetExplosion)
	{
		UNiagaraComponent* ExplosionComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TargetExplosion, ExplosionLocation);
		ExplosionComp->SetNiagaraVariableFloat(FString("SphereRadius"), SphereRadius);
		ExplosionComp->SetColorParameter(FName("SphereColor"), ColorWhenDestroyed);
	}
}
