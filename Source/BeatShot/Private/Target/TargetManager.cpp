// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Target/TargetManager.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "GlobalConstants.h"
#include "Target/Target.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Target/ReinforcementLearningComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Target/SpawnAreaManagerComponent.h"

DEFINE_LOG_CATEGORY(LogTargetManager);
using namespace Constants;

ATargetManager::ATargetManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Box"));
	RootComponent = SpawnBox;
	
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Volume"));
	TopBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Top Box"));
	BottomBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Bottom Box"));
	LeftBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Left Box"));
	RightBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Box"));
	ForwardBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Forward Box"));
	BackwardBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Backward Box"));

	SpawnBox->SetLineThickness(5.f);
	TopBox->SetLineThickness(5.f);
	BottomBox->SetLineThickness(5.f);
	LeftBox->SetLineThickness(5.f);
	RightBox->SetLineThickness(5.f);
	ForwardBox->SetLineThickness(5.f);
	BackwardBox->SetLineThickness(5.f);

	SpawnAreaManager = CreateDefaultSubobject<USpawnAreaManagerComponent>(TEXT("Spawn Area Manager Component"));
	ReinforcementLearningComponent = CreateDefaultSubobject<UReinforcementLearningComponent>(TEXT("Reinforcement Learning Component"));
	
	ConsecutiveTargetsHit = 0;
	BSConfigLocal = FBSConfig();
	BSConfig = nullptr;
	PlayerSettings = FPlayerSettings_Game();
	LastTargetSpawnedCenter = false;
	ShouldSpawn = false;
	bShowDebug_SpawnBox = false;
	bShowDebug_SpawnMemory = false;
	bShowDebug_ReinforcementLearningWidget = false;
	CurrentSpawnArea = nullptr;
	PreviousSpawnArea = nullptr;
	CurrentTargetScale = FVector(1.f);
	StaticExtrema = FExtrema();
	StaticExtents = FVector();
	ConsecutiveTargetsHit = 0;
	DynamicSpawnScale = 0;
	ManagedTargets = TArray<ATarget*>();
	TotalPossibleDamage = 0.f;
	bLastSpawnedTargetDirectionChangeHorizontal = false;
	bLastActivatedTargetDirectionChangeHorizontal = false;
}

void ATargetManager::BeginPlay()
{
	Super::BeginPlay();
}

void ATargetManager::Destroyed()
{
	if (ShouldSpawn)
	{
		SetShouldSpawn(false);
	}
	
	if (!GetManagedTargets().IsEmpty())
	{
		for (TObjectPtr<ATarget> Target : GetManagedTargets())
		{
			if (Target)
			{
				Target->Destroy();
			}
		}
		ManagedTargets.Empty();
	}

	BSConfig = nullptr;
	
	Super::Destroyed();
}

void ATargetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TrackingTargetIsDamageable())
	{
		UpdateTotalPossibleDamage();
	}
}

void ATargetManager::Init(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings)
{
	if (ShouldSpawn)
	{
		SetShouldSpawn(false);
	}
	// Initialize local copy of BSConfig
	BSConfigLocal = InBSConfig;
	BSConfig = &BSConfigLocal;
	PlayerSettings = InPlayerSettings;

	Init_Internal();
}

void ATargetManager::Init(FBSConfig* InBSConfig, const FPlayerSettings_Game& InPlayerSettings)
{
	if (ShouldSpawn)
	{
		SetShouldSpawn(false);
	}
	// Initialize pointer to another BSConfig
	BSConfig = InBSConfig;
	PlayerSettings = InPlayerSettings;

	Init_Internal();
}

void ATargetManager::Init_Internal()
{
	// Clean Up
	if (!GetManagedTargets().IsEmpty())
	{
		for (TObjectPtr<ATarget> Target : GetManagedTargets())
		{
			if (Target)
			{
				Target->Destroy();
			}
		}
		ManagedTargets.Empty();
	}
	ConsecutiveTargetsHit = 0;
	LastTargetSpawnedCenter = false;
	CurrentSpawnArea = nullptr;
	PreviousSpawnArea = nullptr;
	CurrentTargetScale = FVector(1.f);
	StaticExtrema = FExtrema();
	StaticExtents = FVector();
	ConsecutiveTargetsHit = 0;
	DynamicSpawnScale = 0;
	TotalPossibleDamage = 0.f;
	
	GetBSConfig()->TargetConfig.bUseSeparateOutlineColor = PlayerSettings.bUseSeparateOutlineColor;
	if (GetBSConfig()->TargetConfig.TargetSpawningPolicy == ETargetSpawningPolicy::UpfrontOnly)
	{
		GetBSConfig()->TargetConfig.OnSpawnColor = PlayerSettings.InactiveTargetColor;
	}
	else
	{
		GetBSConfig()->TargetConfig.OnSpawnColor = PlayerSettings.StartTargetColor;
	}
	GetBSConfig()->TargetConfig.InactiveTargetColor = PlayerSettings.InactiveTargetColor;
	GetBSConfig()->TargetConfig.StartColor = PlayerSettings.StartTargetColor;
	GetBSConfig()->TargetConfig.PeakColor = PlayerSettings.PeakTargetColor;
	GetBSConfig()->TargetConfig.EndColor = PlayerSettings.EndTargetColor;
	
	// GameMode menu uses the full width, while box bounds are only half width / half height
	StaticExtents = GetBSConfig()->TargetConfig.GenerateTargetManagerBoxBounds();

	// Set new location & box extent
	SpawnBox->SetRelativeLocation(GetBSConfig()->TargetConfig.GenerateSpawnBoxLocation());
	SpawnBox->SetBoxExtent(StaticExtents);
	StaticExtrema.Min = SpawnBox->Bounds.GetBoxExtrema(0);
	StaticExtrema.Max = SpawnBox->Bounds.GetBoxExtrema(1);

	UpdateSpawnVolume();

	// Initialize SpawnAreaManager and SpawnAreas
	const FExtrema Extrema = GetBSConfig()->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid ? GenerateBoxExtremaGrid() : GetBoxExtrema(false);
	SpawnAreaManager->Init(GetBSConfig(), GetBoxOrigin(), StaticExtents, Extrema);

	// Initialize Dynamic SpawnBox
	if (IsDynamicBoundsScalingPolicy(GetBSConfig()->TargetConfig.BoundsScalingPolicy))
	{
		SetBoxExtents_Dynamic();
	}

	// Enable / Disable Reinforcement Learning
	if (GetBSConfig()->AIConfig.bEnableReinforcementLearning)
	{
		FRLAgentParams Params;
		Params.InQTable = GetScoreInfoFromDefiningConfig(GetBSConfig()->DefiningConfig).QTable;
		Params.DefaultMode = GetBSConfig()->DefiningConfig.BaseGameMode;
		Params.CustomGameModeName = GetBSConfig()->DefiningConfig.CustomGameModeName;
		Params.Size = SpawnAreaManager->GetSpawnAreas().Num();
		Params.SpawnAreasHeight = SpawnAreaManager->GetSpawnAreasHeight();
		Params.SpawnAreasWidth = SpawnAreaManager->GetSpawnAreasWidth();
		Params.InEpsilon = GetBSConfig()->AIConfig.Epsilon;
		Params.InGamma = GetBSConfig()->AIConfig.Gamma;
		Params.InAlpha = GetBSConfig()->AIConfig.Alpha;
		ReinforcementLearningComponent->SetActive(true);
		ReinforcementLearningComponent->Init(Params);
	}
	else
	{
		ReinforcementLearningComponent->SetActive(false);
		ReinforcementLearningComponent->Deactivate();
	}

	// Initialize CurrentSpawnArea, and spawn any targets if needed
	switch(GetBSConfig()->TargetConfig.TargetSpawningPolicy)
	{
	case ETargetSpawningPolicy::None:
	case ETargetSpawningPolicy::RuntimeOnly:
		FindNextTargetProperties();
		break;
	case ETargetSpawningPolicy::UpfrontOnly:
		SpawnUpfrontOnlyTargets();
	}
	
	UE_LOG(LogTargetManager, Display, TEXT("SpawnBoxWidth %f SpawnBoxHeight %f"), StaticExtents.Y * 2, StaticExtents.Z * 2);
}

void ATargetManager::SetShouldSpawn(const bool bShouldSpawn)
{
	if (bShouldSpawn)
	{
		if (GetBSConfig()->TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::HideTarget))
		{
			for (const TObjectPtr<ATarget> Target : GetManagedTargets())
			{
				Target->SetActorHiddenInGame(true);
			}
		}
	}
	ShouldSpawn = bShouldSpawn;
}

void ATargetManager::OnPlayerStopTrackingTarget()
{
	if (GetBSConfig()->TargetConfig.TargetDamageType != ETargetDamageType::Tracking)
	{
		return;
	}
	for (const TObjectPtr<ATarget> Target : GetManagedTargets())
	{
		if (Target && !Target->IsTargetImmuneToTracking())
		{
			Target->SetTargetColor(GetBSConfig()->TargetConfig.EndColor);
		}
	}
}

// Runtime target spawning and activation

void ATargetManager::OnAudioAnalyzerBeat()
{
	if (!ShouldSpawn) return;
	
	// We're relying on FindNextTargetProperties to have a fresh SpawnArea lined up
	if (!CurrentSpawnArea)
	{
		UE_LOG(LogTemp, Display, TEXT("Invalid SpawnArea"));
		FindNextTargetProperties();
		return;
	}

	// Handles activating targets that were previously spawned, but are inactive
	HandleActivateExistingTargets();
	
	// Handles spawning (and activating) RuntimeOnly targets
	if (GetBSConfig()->TargetConfig.TargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly)
	{
		HandleRuntimeSpawnAndActivation();
	}
	
	// Debug stuff
	if (bShowDebug_SpawnMemory)
	{
		ShowDebug_NumRecentNumActivated();
	}

	// Remove recent targets if based on number of targets
	if (GetBSConfig()->TargetConfig.RecentTargetMemoryPolicy == ERecentTargetMemoryPolicy::NumTargetsBased)
	{
		SpawnAreaManager->RefreshRecentFlags();
	}
}

ATarget* ATargetManager::SpawnTarget(USpawnArea* InSpawnArea)
{
	if (!InSpawnArea)
	{
		return nullptr;
	}
	ATarget* Target = GetWorld()->SpawnActorDeferred<ATarget>(TargetToSpawn,
		FTransform(FRotator::ZeroRotator, InSpawnArea->GetChosenPoint(), InSpawnArea->GetTargetScale()),
		this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Target->Init(GetBSConfig()->TargetConfig);
	Target->OnTargetDamageEventOrTimeout.AddDynamic(this, &ATargetManager::OnTargetHealthChangedOrExpired);
	if (BSConfig->TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeDirection))
	{
		Target->OnDeactivationResponse_ChangeDirection.AddUObject(this, &ATargetManager::ChangeTargetDirection);
	}
	InSpawnArea->SetTargetGuid(Target->GetGuid());
	Target->FinishSpawning(FTransform(), true);
	AddToManagedTargets(Target);
	if (BSConfig->TargetConfig.bApplyVelocityWhenSpawned)
	{
		Target->SetTargetSpeed(FMath::FRandRange(GetBSConfig()->TargetConfig.MinSpawnedTargetSpeed, GetBSConfig()->TargetConfig.MaxSpawnedTargetSpeed));
		ChangeTargetDirection(Target, 0);
	}
	return Target;
}

bool ATargetManager::ActivateTarget(ATarget* InTarget) const
{
	// TargetManager handles all TargetActivationResponses
	// Each target handles their own TargetDeactivationResponses & TargetDestructionConditions
	
	if (!InTarget || !SpawnAreaManager->IsSpawnAreaValid(SpawnAreaManager->FindSpawnAreaFromGuid(InTarget->GetGuid())))
	{
		return false;
	}

	// Make sure it isn't hidden
	if (InTarget->IsHidden())
	{
		InTarget->SetActorHiddenInGame(false);
	}

	if (GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::AddImmunity))
	{
		InTarget->ApplyImmunityEffect();
	}
	if (GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::RemoveImmunity))
	{
		InTarget->RemoveImmunityEffect();
	}
	if (GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ToggleImmunity))
	{
		InTarget->IsTargetImmune() ? InTarget->RemoveImmunityEffect() : InTarget->ApplyImmunityEffect();
	}
	if (GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeVelocity))
	{
		InTarget->SetTargetSpeed(FMath::FRandRange(GetBSConfig()->TargetConfig.MinActivatedTargetSpeed, GetBSConfig()->TargetConfig.MaxActivatedTargetSpeed));
		if (!GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection) &&
			GetBSConfig()->TargetConfig.MovingTargetDirectionMode != EMovingTargetDirectionMode::None)
		{
			ChangeTargetDirection(InTarget, 1);
		}
	}
	if (GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection))
	{
		ChangeTargetDirection(InTarget, 1);
	}
	
	if (InTarget->HasTargetBeenActivatedBefore() && GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ApplyConsecutiveTargetScale))
	{
		InTarget->SetTargetScale(GetNextTargetScale());
	}
	
	if (InTarget->ActivateTarget(GetBSConfig()->TargetConfig.TargetMaxLifeSpan))
	{
		SpawnAreaManager->FlagSpawnAreaAsActivated(InTarget->GetGuid());
		OnTargetActivated.Broadcast();
		if (ReinforcementLearningComponent->IsActive() && SpawnAreaManager->IsSpawnAreaValid(PreviousSpawnArea))
		{
			ReinforcementLearningComponent->AddToActiveTargetPairs(PreviousSpawnArea->GetIndex(), CurrentSpawnArea->GetIndex());
		}
		return true;
	}
	return false;
}

void ATargetManager::HandleRuntimeSpawnAndActivation()
{
	int32 NumberToSpawn = GetNumberOfRuntimeTargetsToSpawn();
	int32 NumberToActivate = GetNumberOfTargetsToActivate(NumberToSpawn);

	// If not limit on activation, activate all spawned targets
	if (NumberToActivate == -1)
	{
		NumberToActivate = NumberToSpawn;
	}

	// Only spawn targets that can be activated
	if (!GetBSConfig()->TargetConfig.bAllowSpawnWithoutActivation)
	{
		if (NumberToSpawn > NumberToActivate)
		{
			NumberToSpawn = NumberToActivate;
		}
	}

	if (CurrentSpawnArea->IsCurrentlyManaged())
	{
		FindNextTargetProperties();
	}
	
	for (int i = 0; i < NumberToSpawn; i++)
	{
		if (ATarget* SpawnedTarget = SpawnTarget(CurrentSpawnArea))
		{
			if (NumberToActivate > 0)
			{
				ActivateTarget(SpawnedTarget);
				NumberToActivate--;
			}
		}
		FindNextTargetProperties();
	}
}

void ATargetManager::HandleActivateExistingTargets() 
{
	if (GetManagedTargets().IsEmpty())
	{
		return;
	}
	
	// Persistant Targets are the only type that can always receive continuous activation
	if (GetBSConfig()->TargetConfig.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		HandlePermanentlyActiveTargetActivation();
		return;
	}

	// Check to see if theres any targets available to activate
	if (SpawnAreaManager->GetDeactivatedManagedSpawnAreas().IsEmpty())
	{
		return;
	}

	const int32 NumToActivate = GetNumberOfTargetsToActivate(SpawnAreaManager->GetDeactivatedManagedSpawnAreas().Num());
	
	for (int i = 0; i < NumToActivate; i++)
	{
		if (GetBSConfig()->TargetConfig.TargetSpawningPolicy == ETargetSpawningPolicy::UpfrontOnly)
		{
			if (CurrentSpawnArea)
			{
				if (ATarget* Target = FindManagedTargetByGuid(CurrentSpawnArea->GetTargetGuid()))
				{
					if (ActivateTarget(Target))
					{
						FindNextTargetProperties();
					}
				}
			}
		}
		// TODO: Maybe this should just be moved to GetNextSpawnArea
		else if (const USpawnArea* Point = SpawnAreaManager->FindOldestDeactivatedManagedSpawnArea())
		{
			if (ATarget* Target = FindManagedTargetByGuid(Point->GetTargetGuid()))
			{
				ActivateTarget(Target);
			}
		}
	}
}

void ATargetManager::HandlePermanentlyActiveTargetActivation() const
{
	// Handle initial activation
	TArray<USpawnArea*> SpawnAreas = SpawnAreaManager->GetActivatedSpawnAreas();
	if (SpawnAreas.IsEmpty())
	{
		SpawnAreas = SpawnAreaManager->GetDeactivatedManagedSpawnAreas();
	}
	
	for (const USpawnArea* SpawnArea : SpawnAreas)
	{
		if (ATarget* Target = FindManagedTargetByGuid(SpawnArea->GetTargetGuid()))
		{
			ActivateTarget(Target);
		}
	}
}

int32 ATargetManager::GetNumberOfRuntimeTargetsToSpawn() const
{
	// Depends on: MaxNumTargetsAtOnce, NumRuntimeTargetsToSpawn, ManagedTargets, bUseBatchSpawning

	// Batch spawning waits until there are no more Activated and Deactivated target(s)
	if (GetBSConfig()->TargetConfig.bUseBatchSpawning)
	{
		if (GetManagedTargets().Num() > 0 ||
			SpawnAreaManager->GetActivatedSpawnAreas().Num() > 0 ||
			SpawnAreaManager->GetDeactivatedManagedSpawnAreas().Num() > 0)
		{
			return 0;
		}
	}

	// Set default value to number of runtime targets to spawn to NumRuntimeTargetsToSpawn
	int32 NumAllowedToSpawn = (GetBSConfig()->TargetConfig.NumRuntimeTargetsToSpawn == -1) ? 1 : GetBSConfig()->TargetConfig.NumRuntimeTargetsToSpawn;

	// Return NumRuntimeTargetsToSpawn if no Max
	if (GetBSConfig()->TargetConfig.MaxNumTargetsAtOnce == -1)
	{
		return NumAllowedToSpawn;
	}
	
	NumAllowedToSpawn = GetBSConfig()->TargetConfig.MaxNumTargetsAtOnce - GetManagedTargets().Num();

	// Don't let NumAllowedToSpawn exceed NumRuntimeTargetsToSpawn
	if (NumAllowedToSpawn > GetBSConfig()->TargetConfig.NumRuntimeTargetsToSpawn)
	{
		return GetBSConfig()->TargetConfig.NumRuntimeTargetsToSpawn;
	}
	
	return NumAllowedToSpawn;
}

int32 ATargetManager::GetNumberOfTargetsToActivate(const int32 MaxPossibleToActivate) const
{
	// Depends on: MaxNumActivatedTargetsAtOnce, MinNumTargetsToActivateAtOnce, MaxNumTargetsToActivateAtOnce,
	// DeactivatedManagedSpawnAreas, ActivatedSpawnAreas
	
	int32 Limit = GetBSConfig()->TargetConfig.MaxNumActivatedTargetsAtOnce;
	int32 MinToActivate = GetBSConfig()->TargetConfig.MinNumTargetsToActivateAtOnce;
	int32 MaxToActivate = GetBSConfig()->TargetConfig.MaxNumTargetsToActivateAtOnce;
	
	// No constraints
	if (Limit == -1 && MinToActivate == -1 && MaxToActivate == -1)
	{
		return MaxPossibleToActivate;
	}
	
	// No min constraint, set to zero
	if (MinToActivate == -1)
	{
		MinToActivate = 0;
	}
	
	// No max constraint, set to MaxPossibleToActivate
	if (MaxToActivate == -1)
	{
		MaxToActivate = MaxPossibleToActivate;
	}

	// No constraint on total activated at once
	if (Limit == -1)
	{
		Limit = MaxPossibleToActivate;
	}

	const int32 NumCurrent = SpawnAreaManager->GetActivatedSpawnAreas().Num();
	if (NumCurrent >= Limit)
	{
		return 0;
	}
	
	// Current limit is MaxNumActivatedTargetsAtOnce - Currently activated targets
	const int32 CurrentLimit = Limit - NumCurrent;
	// Don't let Max exceed CurrentLimit
	MaxToActivate = MaxToActivate > CurrentLimit ? CurrentLimit : MaxToActivate;
	// Don't let min exceed max
	MinToActivate = MinToActivate > MaxToActivate ? MaxToActivate : MinToActivate;
	
	return FMath::RandRange(MinToActivate, MaxToActivate);
}

void ATargetManager::SpawnUpfrontOnlyTargets()
{
	if (GetBSConfig()->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid)
	{
		for (int i = 0; i < SpawnAreaManager->GetSpawnAreas().Num(); i++)
		{
			SpawnAreaManager->GetSpawnAreasRef()[i]->SetTargetScale(GetNextTargetScale());
			SpawnTarget(SpawnAreaManager->GetSpawnAreasRef()[i]);
		}
		FindNextTargetProperties();
	}
	else
	{
		for (int i = 0; i < GetBSConfig()->TargetConfig.NumUpfrontTargetsToSpawn; i++)
		{
			FindNextTargetProperties();
			if (CurrentSpawnArea)
			{
				SpawnTarget(CurrentSpawnArea);
			}
		}
	}
}

// Deactivation and Destruction

void ATargetManager::OnTargetHealthChangedOrExpired(const FTargetDamageEvent& TargetDamageEvent)
{
	UpdateConsecutiveTargetsHit(TargetDamageEvent.TimeAlive);
	UpdateDynamicSpawnScale(TargetDamageEvent.TimeAlive);
	HandleTargetExpirationDelegate(GetBSConfig()->TargetConfig.TargetDamageType, TargetDamageEvent);
	HandleManagedTargetRemoval(GetBSConfig()->TargetConfig.TargetDestructionConditions, TargetDamageEvent);

	if (ReinforcementLearningComponent->IsActive())
	{
		if (const USpawnArea* Found = SpawnAreaManager->FindSpawnAreaFromGuid(TargetDamageEvent.Guid))
		{
			ReinforcementLearningComponent->UpdateReinforcementLearningReward(Found->GetIndex(), TargetDamageEvent.TimeAlive != -1);
			ReinforcementLearningComponent->UpdateReinforcementLearningComponent(SpawnAreaManager.Get());
		}
	}
	
	SpawnAreaManager->HandleRecentTargetRemoval(GetBSConfig()->TargetConfig.RecentTargetMemoryPolicy, TargetDamageEvent);
}

void ATargetManager::UpdateConsecutiveTargetsHit(const float TimeAlive)
{
	if (TimeAlive == -1)
	{
		ConsecutiveTargetsHit = 0;
	}
	else
	{
		ConsecutiveTargetsHit++;
	}
}

void ATargetManager::UpdateDynamicSpawnScale(const float TimeAlive)
{
	if (TimeAlive == -1)
	{
		DynamicSpawnScale = FMath::Clamp(DynamicSpawnScale - 5, 0, 100);
	}
	else
	{
		DynamicSpawnScale = FMath::Clamp(DynamicSpawnScale + 1, 0, 100);
	}
}

void ATargetManager::HandleTargetExpirationDelegate(const ETargetDamageType& DamageType, const FTargetDamageEvent& TargetDamageEvent) const
{
	if (DamageType == ETargetDamageType::Tracking)
	{
		OnBeatTrackTargetDamaged.Broadcast(TargetDamageEvent.DamageDelta, TotalPossibleDamage);
	}
	else
	{
		OnTargetDeactivated.Broadcast(TargetDamageEvent.TimeAlive, ConsecutiveTargetsHit, TargetDamageEvent.Transform);
	}
}

void ATargetManager::HandleManagedTargetRemoval(const TArray<ETargetDestructionCondition>& TargetDestructionConditions, const FTargetDamageEvent& TargetDamageEvent)
{
	if (TargetDestructionConditions.Contains(ETargetDestructionCondition::Persistant))
	{
		return;
	}
	
	if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnDeactivation))
	{
		RemoveFromManagedTargets(TargetDamageEvent.Guid);
	}
	if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnExpiration))
	{
		RemoveFromManagedTargets(TargetDamageEvent.Guid);
	}
	if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnHealthReachedZero) &&
		TargetDamageEvent.CurrentHealth <= 0.f)
	{
		RemoveFromManagedTargets(TargetDamageEvent.Guid);
	}
	if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnAnyExternalDamageTaken) &&
		TargetDamageEvent.TimeAlive != -1)
	{
		RemoveFromManagedTargets(TargetDamageEvent.Guid);
	}
	
}

// Finding next spawn location and scale

void ATargetManager::FindNextTargetProperties()
{
	const FVector NewScale = GetNextTargetScale();

	if (CurrentSpawnArea)
	{
		LastTargetSpawnedCenter = CurrentSpawnArea->GetChosenPoint().Equals(GetBoxOrigin());
		// Assign CurrentSpawnArea address to PreviousSpawnArea just before finding CurrentSpawnArea
		PreviousSpawnArea = CurrentSpawnArea;
	}
	else
	{
		LastTargetSpawnedCenter = false;
		PreviousSpawnArea = nullptr;
	}
	
	CurrentSpawnArea = GetNextSpawnArea(GetBSConfig()->TargetConfig.BoundsScalingPolicy, NewScale);
	if (CurrentSpawnArea && SpawnAreaManager->GetSpawnAreas().IsValidIndex(CurrentSpawnArea->GetIndex()))
	{
		/*for (const USpawnArea* SpawnArea : SpawnAreaManager->GetActivatedOrRecentSpawnAreas())
		{
			if (FVector::Distance(SpawnArea->ChosenPoint, CurrentSpawnArea->GetChosenPoint()) < 200.f)
			{
				UE_LOG(LogTemp, Display, TEXT("Distance less than 200: %f"), FVector::Distance(SpawnArea->GetChosenPoint(), CurrentSpawnArea->GetChosenPoint()));
			}
		}*/
		CurrentSpawnArea->SetTargetScale(NewScale);
	}
}

FVector ATargetManager::GetNextTargetScale() const
{
	if (GetBSConfig()->TargetConfig.ConsecutiveTargetScalePolicy == EConsecutiveTargetScalePolicy::SkillBased)
	{
		const float NewFactor = DynamicSpawnCurve->GetFloatValue(DynamicSpawnScale);
		return FVector(UKismetMathLibrary::Lerp(GetBSConfig()->TargetConfig.MinSpawnedTargetScale, GetBSConfig()->TargetConfig.MaxSpawnedTargetScale, NewFactor));
	}
	return FVector(FMath::FRandRange(GetBSConfig()->TargetConfig.MinSpawnedTargetScale, GetBSConfig()->TargetConfig.MaxSpawnedTargetScale));
}

USpawnArea* ATargetManager::GetNextSpawnArea(const EBoundsScalingPolicy BoundsScalingPolicy, const FVector& NewTargetScale) const
{
	// Change the BoxExtent of the SpawnBox if dynamic
	if (IsDynamicBoundsScalingPolicy(BoundsScalingPolicy))
	{
		SetBoxExtents_Dynamic();
	}

	// Can skip GetValidSpawnLocations if forcing every other target in center
	if (GetBSConfig()->TargetConfig.bSpawnEveryOtherTargetInCenter && CurrentSpawnArea && CurrentSpawnArea != SpawnAreaManager->FindSpawnAreaFromLocation(GetBoxOrigin()))
	{
		return SpawnAreaManager->FindSpawnAreaFromLocation(GetBoxOrigin());
	}
	
	TArray<FVector> OpenLocations = SpawnAreaManager->GetValidSpawnLocations(NewTargetScale, GetBoxExtrema(GetBSConfig()->TargetConfig.BoundsScalingPolicy == EBoundsScalingPolicy::Dynamic), CurrentSpawnArea);
	if (OpenLocations.IsEmpty())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("OpenLocations is empty."));
		return nullptr;
	}
	
	if (OpenLocations.Contains(GetBoxOrigin()) && GetBSConfig()->TargetConfig.bSpawnAtOriginWheneverPossible)
	{
		return SpawnAreaManager->FindSpawnAreaFromLocation(GetBoxOrigin());
	}
	
	if (ReinforcementLearningComponent->IsActive())
	{
		if (USpawnArea* NextSpawnArea = TryGetSpawnAreaFromReinforcementLearningComponent(OpenLocations))
		{
			if (GetBSConfig()->TargetConfig.TargetDistributionPolicy != ETargetDistributionPolicy::Grid)
			{
				NextSpawnArea->SetRandomChosenPoint();
			}
			return NextSpawnArea;
		}
		UE_LOG(LogTargetManager, Warning, TEXT("Unable to Spawn Location suggested by RLAgent."));
	}

	const int32 OpenLocationIndex = FMath::RandRange(0, OpenLocations.Num() - 1);
	if (USpawnArea* NextSpawnArea = SpawnAreaManager->FindSpawnAreaFromLocation(OpenLocations[OpenLocationIndex]))
	{
		if (GetBSConfig()->TargetConfig.TargetDistributionPolicy != ETargetDistributionPolicy::Grid)
		{
			NextSpawnArea->SetRandomChosenPoint();
		}
		return NextSpawnArea;
	}
	//UE_LOG(LogTargetManager, Display, TEXT("Found Vertex_BottomLeft %s Found CenterPoint %s Found ChosenPoint %s"), *Found->Vertex_BottomLeft.ToString(), *Found->CenterPoint.ToString(), *Found->ChosenPoint.ToString());
	return nullptr;
}

void ATargetManager::ChangeTargetDirection(ATarget* InTarget, const uint8 InSpawnActivationDeactivation) const
{
	// Alternate directions of spawned targets, while also alternating directions of individual target direction changes
	bool bLastDirectionChangeHorizontal;
	
	if (InSpawnActivationDeactivation == 0)
	{
		bLastDirectionChangeHorizontal = bLastSpawnedTargetDirectionChangeHorizontal;
		bLastSpawnedTargetDirectionChangeHorizontal = !bLastDirectionChangeHorizontal;
	}
	else if (InSpawnActivationDeactivation == 1)
	{
		bLastDirectionChangeHorizontal = bLastActivatedTargetDirectionChangeHorizontal;
		bLastActivatedTargetDirectionChangeHorizontal = !bLastDirectionChangeHorizontal;
	}
	else
	{
		bLastDirectionChangeHorizontal = InTarget->GetLastDirectionChangeHorizontal();
		InTarget->SetLastDirectionChangeHorizontal(!bLastDirectionChangeHorizontal);
	}

	InTarget->SetTargetDirection(GetNewTargetDirection(InTarget->GetActorLocation(), bLastDirectionChangeHorizontal));
}

void ATargetManager::UpdateSpawnVolume() const
{
	const float LocationX = GetBoxOrigin().X - GetBSConfig()->TargetConfig.BoxBounds.X * 0.5f;
	// X Extent should be half move forward distance + max sphere radius
	const float ExtentX = GetBSConfig()->TargetConfig.BoxBounds.X * 0.5f + GetBSConfig()->TargetConfig.MaxSpawnedTargetScale * SphereTargetRadius + 10.f;

	const FVector DynamicExtent = SpawnBox->Bounds.BoxExtent;
		
	SpawnVolume->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y, GetBoxOrigin().Z));
	SpawnVolume->SetBoxExtent(FVector(ExtentX, DynamicExtent.Y, DynamicExtent.Z));
		
	TopBox->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y, GetBoxOrigin().Z + DynamicExtent.Z));
	BottomBox->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y, GetBoxOrigin().Z - DynamicExtent.Z));
	LeftBox->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y - DynamicExtent.Y, GetBoxOrigin().Z));
	RightBox->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y + DynamicExtent.Y, GetBoxOrigin().Z));
	ForwardBox->SetRelativeLocation(FVector(LocationX + ExtentX, GetBoxOrigin().Y, GetBoxOrigin().Z));
	BackwardBox->SetRelativeLocation(FVector(LocationX - ExtentX, GetBoxOrigin().Y, GetBoxOrigin().Z));
		
	TopBox->SetBoxExtent(FVector(ExtentX, DynamicExtent.Y, 0));
	BottomBox->SetBoxExtent(FVector(ExtentX, DynamicExtent.Y, 0));
	LeftBox->SetBoxExtent(FVector(ExtentX, 0, DynamicExtent.Z));
	RightBox->SetBoxExtent(FVector(ExtentX, 0, DynamicExtent.Z));
	ForwardBox->SetBoxExtent(FVector(0, DynamicExtent.Y, DynamicExtent.Z));
	BackwardBox->SetBoxExtent(FVector(0, DynamicExtent.Y, DynamicExtent.Z));
}

void ATargetManager::UpdateTotalPossibleDamage()
{
	TotalPossibleDamage++;
}

bool ATargetManager::TrackingTargetIsDamageable() const
{
	if (GetBSConfig()->TargetConfig.TargetDamageType == ETargetDamageType::Hit || GetManagedTargets().IsEmpty())
	{
		return false;
	}
	if (GetManagedTargets().FindByPredicate([] (const TObjectPtr<ATarget> target)
	{
		return !target->IsTargetImmuneToTracking();
	}))
	{
		return true;
	}
	return false;
}

ATarget* ATargetManager::FindManagedTargetByGuid(const FGuid Guid) const
{
	const TObjectPtr<ATarget>* Found = ManagedTargets.FindByPredicate([&] (const ATarget* target)
	{
		return target->GetGuid() == Guid;
	});
	if (Found && Found->Get())
	{
		return Found->Get();
	}
	return nullptr;
}

FVector ATargetManager::GetNewTargetDirection(const FVector& LocationBeforeChange, const bool bLastDirectionChangeHorizontal) const
{
	switch (GetBSConfig()->TargetConfig.MovingTargetDirectionMode)
	{
	case EMovingTargetDirectionMode::HorizontalOnly:
		{
			if (FMath::RandBool())
			{
				return FVector(0, 1, 0);
			}
			return FVector(0, -1, 0);
		}
	case EMovingTargetDirectionMode::VerticalOnly:
		{
			if (FMath::RandBool())
			{
				return FVector(0, 0, 1);
			}
			return FVector(0, 0, -1);
		}
	case EMovingTargetDirectionMode::AlternateHorizontalVertical:
		{
			if (bLastDirectionChangeHorizontal)
			{
				if (FMath::RandBool())
				{
					return FVector(0, 0, 1);
				}
				return FVector(0, 0, -1);
			}
			if (FMath::RandBool())
			{
				return FVector(0, 1, 0);
			}
			return FVector(0, -1, 0);
		}
	case EMovingTargetDirectionMode::Any:
		{
			const FVector NewExtent = FVector(GetBSConfig()->TargetConfig.BoxBounds.X * 0.5f, GetBoxExtents_Static().Y * 0.5f, GetBoxExtents_Static().Z * 0.5f);
			const FVector OriginOffset = FVector(0, GetBoxExtents_Static().Y * 0.5f, GetBoxExtents_Static().Z * 0.5f);
			const FVector BotLeft = UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->Bounds.Origin + FVector(0, -OriginOffset.Y, -OriginOffset.Z), NewExtent);
			const FVector BotRight = UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->Bounds.Origin + FVector(0, OriginOffset.Y, -OriginOffset.Z), NewExtent);
			const FVector TopLeft = UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->Bounds.Origin + FVector(0, -OriginOffset.Y, OriginOffset.Z), NewExtent);
			const FVector TopRight = UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->Bounds.Origin + FVector(0, OriginOffset.Y, OriginOffset.Z), NewExtent);

			TArray PossibleLocations = {BotLeft, BotRight, TopLeft, TopRight};

			if (LocationBeforeChange.Y < 0)
			{
				if (LocationBeforeChange.Z < GetBoxOrigin().Z)
				{
					PossibleLocations.Remove(BotLeft);
				}
				else
				{
					PossibleLocations.Remove(TopLeft);
				}
			}
			else
			{
				if (LocationBeforeChange.Z < GetBoxOrigin().Z)
				{
					PossibleLocations.Remove(BotRight);
				}
				else
				{
					PossibleLocations.Remove(TopRight);
				}
			}
			const FVector NewLocation = PossibleLocations[UKismetMathLibrary::RandomIntegerInRange(0, 2)];
			return UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeChange, NewLocation);
		}
	case EMovingTargetDirectionMode::ForwardOnly:
		{
			return FVector(-1.f, 0.f, 0.f);
		}
	case EMovingTargetDirectionMode::None:
		break;
	}
	return FVector::ZeroVector;
}

FExtrema ATargetManager::GetBoxExtrema(const bool bDynamic) const
{
	if (bDynamic)
	{
		return FExtrema(SpawnBox->Bounds.GetBoxExtrema(0), SpawnBox->Bounds.GetBoxExtrema(1));
	}
	return StaticExtrema;
}

FExtrema ATargetManager::GenerateBoxExtremaGrid() const
{
	const float MaxTargetDiameter = GetBSConfig()->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
	// This will be SpawnMemoryIncY
	const float HSpacing = GetBSConfig()->GridConfig.GridSpacing.X + MaxTargetDiameter;
	// This will be SpawnMemoryIncZ
	const float VSpacing = GetBSConfig()->GridConfig.GridSpacing.Y + MaxTargetDiameter;
	
	const float HalfWidth = HSpacing * (GetBSConfig()->GridConfig.NumHorizontalGridTargets - 1) * 0.5f;
	const float HalfHeight = VSpacing * (GetBSConfig()->GridConfig.NumVerticalGridTargets - 1) * 0.5f;

	// Add 1 to max so that for loop includes MaxY and MaxZ
	const float MinY = GetBoxOrigin().Y - HalfWidth;
	const float MaxY = GetBoxOrigin().Y + HalfWidth + 1.f;
	const float MinZ = GetBoxOrigin().Z - HalfHeight;
	const float MaxZ = GetBoxOrigin().Z + HalfHeight + 1.f;
	
	return FExtrema(FVector(GetBoxOrigin().X, MinY, MinZ), FVector(GetBoxOrigin().X, MaxY, MaxZ));
}

int32 ATargetManager::AddToManagedTargets(ATarget* SpawnTarget)
{
	SpawnAreaManager->FlagSpawnAreaAsManaged(SpawnTarget->GetGuid());
	TArray<TObjectPtr<ATarget>> Targets = GetManagedTargets();
	const int32 NewIndex = Targets.Add(TObjectPtr<ATarget>(SpawnTarget));
	ManagedTargets = Targets;
	return NewIndex;
}

void ATargetManager::RemoveFromManagedTargets(const FGuid GuidToRemove)
{
	SpawnAreaManager->RemoveManagedFlagFromSpawnArea(GuidToRemove);
	const TArray<TObjectPtr<ATarget>> Targets = GetManagedTargets().FilterByPredicate([&] (const TObjectPtr<ATarget>& OtherTarget)
	{
		if (!OtherTarget)
		{
			return false;
		}
		if (OtherTarget->GetGuid() == GuidToRemove)
		{
			return false;
		}
		return true;
	});
	ManagedTargets = Targets;
}

void ATargetManager::SetBoxExtents_Dynamic() const
{
	const float NewFactor = DynamicSpawnCurve->GetFloatValue(DynamicSpawnScale);
	const float LerpY = UKismetMathLibrary::Lerp(GetBoxExtents_Static().Y, GetBoxExtents_Static().Y * 0.5f, NewFactor);
	const float LerpZ = UKismetMathLibrary::Lerp(GetBoxExtents_Static().Z, GetBoxExtents_Static().Z * 0.5f, NewFactor);
	const float Y = FMath::GridSnap<float>(LerpY, SpawnAreaManager->GetSpawnMemoryIncY());
	const float Z = FMath::GridSnap<float>(LerpZ, SpawnAreaManager->GetSpawnMemoryIncZ());
	SpawnBox->SetBoxExtent(FVector(0, Y, Z));
	UpdateSpawnVolume();
}

void ATargetManager::UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings)
{
	PlayerSettings = InPlayerSettings;
	if (!ManagedTargets.IsEmpty())
	{
		for (ATarget* Target : GetManagedTargets())
		{
			Target->UpdatePlayerSettings(PlayerSettings);
		}
	}
}

USpawnArea* ATargetManager::TryGetSpawnAreaFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const
{
	/* Converting all OpenLocations to indices */
	TArray<int32> Indices;
	for (const FVector Vector : OpenLocations)
	{
		if (const int32 FoundIndex = SpawnAreaManager->FindSpawnAreaIndexFromLocation(Vector); FoundIndex != INDEX_NONE)
		{
			Indices.Add(FoundIndex);
		}
	}
	if (Indices.IsEmpty())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("No targets in OpenLocations or No targets in TargetPairs"));
		return nullptr;
	}
	const int32 ChosenIndex = ReinforcementLearningComponent->ChooseNextActionIndex(Indices);
	if (!SpawnAreaManager->GetSpawnAreas().IsValidIndex(ChosenIndex))
	{
		return nullptr;
	}

	USpawnArea* ChosenPoint = SpawnAreaManager->GetSpawnAreasRef()[ChosenIndex];
	ChosenPoint->SetRandomChosenPoint();
	return ChosenPoint;
}

void ATargetManager::ShowDebug_SpawnBox(const bool bShow)
{
	bShowDebug_SpawnBox = bShow;
	if (bShowDebug_SpawnBox)
	{
		SpawnBox->SetHiddenInGame(false);
		SpawnVolume->SetHiddenInGame(false);
		SpawnBox->SetVisibility(true);
		SpawnVolume->SetVisibility(true);
		SpawnBox->ShapeColor = FColor::Blue;
		SpawnVolume->MarkRenderStateDirty();
		SpawnBox->MarkRenderStateDirty();
		/*TopBox->SetHiddenInGame(false);
		BottomBox->SetHiddenInGame(false);
		LeftBox->SetHiddenInGame(false);
		RightBox->SetHiddenInGame(false);
		ForwardBox->SetHiddenInGame(false);
		BackwardBox->SetHiddenInGame(false);*/
	}
	else
	{
		SpawnVolume->SetHiddenInGame(true);
		SpawnBox->SetHiddenInGame(true);
		SpawnVolume->SetVisibility(false);
		SpawnVolume->SetVisibility(false);
		SpawnVolume->MarkRenderStateDirty();
		SpawnBox->MarkRenderStateDirty();
		FlushPersistentDebugLines(GetWorld());
	}
}

void ATargetManager::ShowDebug_SpawnMemory(const bool bShow)
{
	bShowDebug_SpawnMemory = bShow;
	SpawnAreaManager->bShowDebug_SpawnMemory = bShow;
}

void ATargetManager::ShowDebug_ReinforcementLearningWidget(const bool bShow)
{
	if (!ReinforcementLearningComponent->IsActive())
	{
		return;
	}

	bShowDebug_ReinforcementLearningWidget = bShow;

	ABSPlayerController* Controller = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (bShowDebug_ReinforcementLearningWidget)
	{
		Controller->ShowRLAgentWidget(ReinforcementLearningComponent->OnQTableUpdate, ReinforcementLearningComponent->GetHeight(), ReinforcementLearningComponent->GetWidth(),
		                              ReinforcementLearningComponent->GetAveragedTArrayFromQTable());
		return;
	}
	Controller->HideRLAgentWidget();
}

void ATargetManager::ShowDebug_NumRecentNumActivated() const
{
	int NumRecent=0;
	int NumAct=0;
	for (const USpawnArea* Hey : SpawnAreaManager->GetSpawnAreas())
	{
		if (Hey->IsRecent())
		{
			NumRecent++;
		}
		if (Hey->IsActivated())
		{
			NumAct++;
		}
	}
	UE_LOG(LogTemp, Display, TEXT("NumRecent: %d NumActivated: %d"), NumRecent, NumAct);
}

void ATargetManager::ShowDebug_OverlappingVertices(const bool bShow)
{
	SpawnAreaManager->bShowDebug_OverlappingVertices = bShow;
}

TArray<FAccuracyRow> ATargetManager::GetLocationAccuracy() const
{
	/* Each row in QTable has size equal to ScaledSize, and so does each column */
	TArray<FAccuracyRow> OutArray;
	OutArray.Init(FAccuracyRow(5), 5);
	TArray<FQTableIndex> QTableIndices;

	for (int i = 0; i < 25; i++)
	{
		QTableIndices.Add(FQTableIndex(i));
	}

	TArray<USpawnArea*> Counter = SpawnAreaManager->GetSpawnAreas();

	for (int i = 0; i < SpawnAreaManager->GetSpawnAreas().Num(); i++)
	{
		if (const int32 Found = QTableIndices.Find(SpawnAreaManager->GetOutArrayIndexFromSpawnAreaIndex(i)); Found != INDEX_NONE)
		{
			const int32 RowNum = Found / 5;
			const int32 ColNum = Found % 5;

			if (Counter[i]->GetTotalSpawns() != INDEX_NONE)
			{
				if (OutArray[RowNum].TotalSpawns[ColNum] == INDEX_NONE)
				{
					OutArray[RowNum].TotalSpawns[ColNum] = 0;
				}
				OutArray[RowNum].TotalSpawns[ColNum] += Counter[i]->GetTotalSpawns();
				OutArray[RowNum].TotalHits[ColNum] += Counter[i]->GetTotalHits();
			}
		}
	}

	for (FAccuracyRow& AccuracyRow : OutArray)
	{
		AccuracyRow.UpdateAccuracy();
	}
	return OutArray;
}

FCommonScoreInfo ATargetManager::GetCommonScoreInfo() const
{
	TArray<FQTableIndex> Indices;
	for (int i = 0; i < 25; i++)
	{
		Indices.Add(FQTableIndex(i));
	}

	FCommonScoreInfo CommonScoreInfo = FCommonScoreInfo(25);
	TArray<USpawnArea*> Counter = SpawnAreaManager->GetSpawnAreas();
	for (int i = 0; i < SpawnAreaManager->GetSpawnAreas().Num(); i++)
	{
		if (const int32 Found = Indices.Find(FQTableIndex(SpawnAreaManager->GetOutArrayIndexFromSpawnAreaIndex(i))); Found != INDEX_NONE)
		{
			if (Counter[i]->GetTotalSpawns() != INDEX_NONE)
			{
				if (CommonScoreInfo.TotalSpawns[Found] == INDEX_NONE)
				{
					CommonScoreInfo.TotalSpawns[Found] = 0;
				}
				CommonScoreInfo.TotalSpawns[Found] += Counter[i]->GetTotalSpawns();
				CommonScoreInfo.TotalHits[Found] += Counter[i]->GetTotalHits();
			}
		}
	}
	if (ReinforcementLearningComponent->IsActive())
	{
		CommonScoreInfo.QTable = ReinforcementLearningComponent->GetSaveReadyQTable();
		ReinforcementLearningComponent->PrintRewards();
	}
	return CommonScoreInfo;
}
