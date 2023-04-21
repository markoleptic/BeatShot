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
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>("Capsule Collider");
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	BaseMesh->SetupAttachment(CapsuleComp);

	OutlineMesh = CreateDefaultSubobject<UStaticMeshComponent>("Outline Mesh");
	OutlineMesh->SetupAttachment(BaseMesh);

	HealthComponent = CreateDefaultSubobject<UBSHealthComponent>("Health Component");

	// Create ability system component, and set it to be explicitly replicated
	HardRefAbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// Create the attribute set, this replicates by default
	// Adding it as a sub object of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	HardRefAttributeSetBase = CreateDefaultSubobject<UBSAttributeSetBase>(TEXT("AttributeSetBase"));
	HardRefAbilitySystemComponent->SetIsReplicated(true);

	// Minimal Mode means that no GameplayEffects will replicate. They will only live on the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	HardRefAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
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
	return HardRefAbilitySystemComponent;
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

void ASphereTarget::SetSphereScale(const FVector& NewScale)
{
	TargetScale = NewScale.X;
	BaseMesh->SetRelativeScale3D(NewScale * Constants::BaseToOutlineRatio);
	OutlineMesh->SetRelativeScale3D(FVector(1 / Constants::BaseToOutlineRatio));
}

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(GetAbilitySystemComponent()))
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
		HealthComponent->InitializeWithAbilitySystem(HardRefAbilitySystemComponent, GameplayTags);
		HealthComponent->OnOutOfHealth.BindUObject(this, &ASphereTarget::HandleDestruction);
		if (BSConfig.DefiningConfig.BaseGameMode == EDefaultMode::BeatGrid)
		{
			HealthComponent->OnHealthChanged.AddUObject(this, &ASphereTarget::HandleTemporaryDestruction);
		}
	}
	
	/* Use Color Changing Material, this is required in order to change color using C++ */
	Material = BaseMesh->GetMaterial(0);
	MID_TargetColorChanger = UMaterialInstanceDynamic::Create(Material, this);
	BaseMesh->SetMaterial(0, MID_TargetColorChanger);

	/* Use Color Changing Material, this is required in order to change color using C++ */
	OutlineMaterial = OutlineMesh->GetMaterial(0);
	MID_TargetOutline = UMaterialInstanceDynamic::Create(OutlineMaterial, this);
	OutlineMesh->SetMaterial(0, MID_TargetOutline);

	/* Set Outline Color */
	if (PlayerSettings.bUseSeparateOutlineColor)
	{
		SetOutlineColor(PlayerSettings.TargetOutlineColor);
	}
	
	const float WhiteToPeakMultiplier = 1 / BSConfig.AudioConfig.PlayerDelay;
	const float PeakToFadeMultiplier = 1 / (BSConfig.TargetConfig.TargetMaxLifeSpan - BSConfig.AudioConfig.PlayerDelay);

	/* White to Peak Target Color */
	FOnTimelineFloat OnStartToPeak;
	OnStartToPeak.BindDynamic(this, &ASphereTarget::StartToPeak);
	StartToPeakTimeline.AddInterpFloat(StartToPeakCurve, OnStartToPeak);

	/* Play PeakToEnd when StartToPeak is finished */
	FOnTimelineEvent OnWhiteToPeakFinished;
	OnWhiteToPeakFinished.BindDynamic(this, &ASphereTarget::PlayPeakToEndTimeline);
	StartToPeakTimeline.SetTimelineFinishedFunc(OnWhiteToPeakFinished);

	/* Peak Color to Fade Color */
	FOnTimelineFloat OnPeakToFade;
	OnPeakToFade.BindDynamic(this, &ASphereTarget::PeakToEnd);
	PeakToEndTimeline.AddInterpFloat(PeakToEndCurve, OnPeakToFade);

	/* Fade the target from transparent to BeatGridInactiveColor */
	FOnTimelineFloat OnFadeAndReappear;
	OnFadeAndReappear.BindDynamic(this, &ASphereTarget::FadeAndReappear);
	FadeAndReappearTimeline.AddInterpFloat(FadeAndReappearCurve, OnFadeAndReappear);
	
	FOnTimelineEvent OnFadeAndReappearFinished;
	OnFadeAndReappearFinished.BindDynamic(this, &ASphereTarget::ASphereTarget::ShowTargetOutline);
	FadeAndReappearTimeline.SetTimelineFinishedFunc(OnFadeAndReappearFinished);

	StartToPeakTimeline.SetPlayRate(WhiteToPeakMultiplier);
	PeakToEndTimeline.SetPlayRate(PeakToFadeMultiplier);
	FadeAndReappearTimeline.SetPlayRate(WhiteToPeakMultiplier);

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
		SetOutlineColor(PlayerSettings.EndTargetColor);
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
	FadeAndReappearTimeline.TickTimeline(DeltaSeconds);
}

void ASphereTarget::StartBeatGridTimer(const float Lifespan)
{
	GetWorldTimerManager().SetTimer(TimeSinceSpawn, this, &ASphereTarget::OnBeatGridTimerTimeOut, Lifespan, false);
	GameplayTags.AddTag(FBSGameplayTags::Get().Target_State_Damageable);
	PlayStartToPeakTimeline();
}

void ASphereTarget::PlayStartToPeakTimeline()
{
	if (FadeAndReappearTimeline.IsPlaying())
	{
		FadeAndReappearTimeline.Stop();
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
	if (FadeAndReappearTimeline.IsPlaying())
	{
		FadeAndReappearTimeline.Stop();
	}
	PeakToEndTimeline.PlayFromStart();
}

void ASphereTarget::PlayFadeAndReappearTimeline()
{
	if (StartToPeakTimeline.IsPlaying())
	{
		StartToPeakTimeline.Stop();
	}
	if (PeakToEndTimeline.IsPlaying())
	{
		PeakToEndTimeline.Stop();
	}
	MID_TargetOutline->SetScalarParameterValue(TEXT("ShowOutline"), 0.f);
	FadeAndReappearTimeline.PlayFromStart();
}

void ASphereTarget::SetColorToBeatGridColor()
{
	SetSphereColor(PlayerSettings.BeatGridInactiveTargetColor);
	SetOutlineColor(PlayerSettings.BeatGridInactiveTargetColor);
}

void ASphereTarget::StartToPeak(const float Alpha)
{
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(PlayerSettings.StartTargetColor, PlayerSettings.PeakTargetColor, Alpha);
	SetSphereColor(Color);
	if (!PlayerSettings.bUseSeparateOutlineColor)
	{
		SetOutlineColor(Color);
	}
}

void ASphereTarget::PeakToEnd(const float Alpha)
{
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(PlayerSettings.PeakTargetColor, PlayerSettings.EndTargetColor, Alpha);
	SetSphereColor(Color);
	if (!PlayerSettings.bUseSeparateOutlineColor)
	{
		SetOutlineColor(Color);
	}
}

void ASphereTarget::FadeAndReappear(const float Alpha)
{
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(FLinearColor::Transparent, PlayerSettings.BeatGridInactiveTargetColor, Alpha);
	SetSphereColor(Color);
	SetOutlineColor(Color);
}

void ASphereTarget::ApplyImmunityEffect()
{
	GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(TargetImmunity.GetDefaultObject(), 1.f, GetAbilitySystemComponent()->MakeEffectContext());
}

void ASphereTarget::SetSphereColor(const FLinearColor& Output)
{
	MID_TargetColorChanger->SetVectorParameterValue(TEXT("Color"), Output);
}

void ASphereTarget::SetOutlineColor(const FLinearColor& Output)
{
	MID_TargetOutline->SetVectorParameterValue(TEXT("Color"), Output);
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
	PlayExplosionEffect(BaseMesh->GetComponentLocation(), Constants::SphereRadius * TargetScale, MID_TargetColorChanger->K2_GetVectorParameterValue(TEXT("Color")));
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
		PlayExplosionEffect(BaseMesh->GetComponentLocation(), Constants::SphereRadius * TargetScale, MID_TargetColorChanger->K2_GetVectorParameterValue(TEXT("Color")));
		ApplyImmunityEffect();
		PlayFadeAndReappearTimeline();
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

void ASphereTarget::OnBeatGridTimerTimeOut()
{
	ApplyImmunityEffect();
	GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
	OnLifeSpanExpired.Broadcast(true, -1, this);
}

void ASphereTarget::ShowTargetOutline()
{
	MID_TargetOutline->SetScalarParameterValue(TEXT("ShowOutline"), 1.f);
}

void ASphereTarget::PlayExplosionEffect(const FVector& ExplosionLocation, const float SphereRadius, const FLinearColor& ColorWhenDestroyed) const
{
	if (NS_Standard_Explosion)
	{
		UNiagaraComponent* ExplosionComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_Standard_Explosion, ExplosionLocation);
		ExplosionComp->SetNiagaraVariableFloat(FString("SphereRadius"), SphereRadius);
		ExplosionComp->SetColorParameter(FName("SphereColor"), ColorWhenDestroyed);
	}
}
