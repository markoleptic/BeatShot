// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to Project Borealis for almost all of this code

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BSCharacterMovementComponent.generated.h"

#define LADDER_MOUNT_TIMEOUT 0.2f

/** Crouch Timings (in seconds) */
#define MOVEMENT_DEFAULT_CROUCHTIME 0.4f
#define MOVEMENT_DEFAULT_CROUCHJUMPTIME 0.0f
#define MOVEMENT_DEFAULT_UNCROUCHTIME 0.2f
#define MOVEMENT_DEFAULT_UNCROUCHJUMPTIME 0.8f

class ABSCharacter;
/** Information about the ground under the character.  It only gets updated as needed. */
USTRUCT(BlueprintType)
struct FCharacterGroundInfo
{
	GENERATED_BODY()

	FCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};


/** Base CharacterMovementComponent for this game */
UCLASS()
class BEATSHOT_API UBSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UBSCharacterMovementComponent();

	void SetSprintSpeedMultiplier(float NewSpringSpeedMultiplier);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float GroundTraceDistance;
	
	/** Returns the current ground info.  Calling this will update the ground info if it's out of date. */
	UFUNCTION(BlueprintCallable, Category = "CharacterMovement")
	const FCharacterGroundInfo& GetGroundInfo();

	/** Gets the friction from cached ground info */
	float GetFrictionFromHit(const FHitResult& Hit) const;
	
	TObjectPtr<ABSCharacter> BSCharacter;

protected:

	/** Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo() */
	FCharacterGroundInfo CachedGroundInfo;
	
	/** If the player is using a ladder */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bOnLadder;

	/** Milliseconds between step sounds */
	float MoveSoundTime;

	/** If we are stepping left, else, right */
	bool StepSide;

	/** The multiplier for acceleration when on ground. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float GroundAccelerationMultiplier;

	/** The multiplier for acceleration when in air. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float AirAccelerationMultiplier;
	
	/* The vector differential magnitude cap when in air. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Jumping / Falling")
	float AirSpeedCap;

	/** Time to crouch on ground in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float CrouchTime;

	/** Time to UnCrouch on ground in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float UnCrouchTime;

	/** Time to crouch in air in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float CrouchJumpTime;

	/** Time to UnCrouch in air in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float UnCrouchJumpTime;

	/** the minimum step height from moving fast */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite)
	float MinStepHeight;

	/** the minimum step height from moving fast */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite)
	float GroundBrakingDeceleration = 15.f;
	
	/** Time (in millis) the player has to re-jump without applying friction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Jumping / Falling", meta=(DisplayName="Rejump Window", ForceUnits="ms"))
	float BrakingWindow;

	/* Progress checked against the Braking Window, incremented in millis. */
	float BrakingWindowTimeElapsed;

	/** If the player has been on the ground past the Braking Window, start braking. */
	bool bBrakingWindowElapsed;

	/** Wait a frame before crouch speed. */
	bool bCrouchFrameTolerated = false;

	/** If in the crouching transition */
	bool bIsInCrouchTransition = false;

	/** If in the crouching transition */
	bool bInCrouch;

	/** The target ground speed when running. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float RunSpeed;

	/** The target ground speed when sprinting. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SprintSpeed;

	/** The target ground speed when walking slowly. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float WalkSpeed;

	/** Speed on a ladder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Ladder")
	float LadderSpeed;

	/** The minimum speed to scale up from for slope movement  */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SpeedMultMin;

	/** The maximum speed to scale up to for slope movement */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SpeedMultMax;

	/** The maximum angle we can roll for camera adjust */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float RollAngle = 0.0f;

	/** Speed of rolling the camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float RollSpeed = 0.0f;

	/** Speed of rolling the camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float BounceMultiplier = 0.0f;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float AxisSpeedLimit = 6667.5f;

	/** Threshold relating to speed ratio and friction which causes us to catch air */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideLimit = 0.5f;

	/** Fraction of UnCrouch half-height to check for before doing starting an UnCrouch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float GroundUnCrouchCheckFactor = 0.75f;

	bool bShouldPlayMoveSounds = false;

public:
	/** Print pos and vel (Source: cl_showpos) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	uint32 bShowPos : 1;

	virtual void InitializeComponent() override;
	virtual void OnRegister() override;

	// Overrides for Source-like movement
	virtual float GetMaxSpeed() const override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;
	virtual void PhysFalling(float deltaTime, int32 Iterations) override;
	virtual bool ShouldLimitAirControl(float DeltaTime, const FVector& FallAcceleration) const override;
	virtual FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	void UpdateSurfaceFriction(bool bIsSliding = false);
	void UpdateCrouching(float DeltaTime, bool bOnlyUnCrouch = false);

	// Overrides for crouch transitions
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;
	virtual void DoCrouchResize(float TargetTime, float DeltaTime, bool bClientSimulation = false);
	virtual void DoUnCrouchResize(float TargetTime, float DeltaTime, bool bClientSimulation = false);

	virtual bool MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit = nullptr, ETeleportType Teleport = ETeleportType::None) override;

	// Jump overrides
	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bClientSimulation) override;
	virtual FVector HandleSlopeBoosting(const FVector& SlideResult, const FVector& Delta, const float Time, const FVector& Normal, const FHitResult& Hit) const override;
	virtual bool ShouldCatchAir(const FFindFloorResult& OldFloor, const FFindFloorResult& NewFloor) override;
	virtual bool IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const override;
	virtual bool ShouldCheckForValidLandingSpot(float DeltaTime, const FVector& Delta, const FHitResult& Hit) const override;

	// Acceleration
	FORCEINLINE FVector GetAcceleration() const { return Acceleration; }

	/** Is this player on a ladder? */
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	bool IsOnLadder() const { return bOnLadder; }

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	/** Do camera roll effect based on velocity */
	float GetCameraRoll();

	/** Set no clip */
	void SetNoClip(bool bNoClip);

	/** Toggle no clip */
	void ToggleNoClip();

	bool IsBrakingWindowTolerated() const { return bBrakingWindowElapsed; }

	bool IsInCrouch() const { return bInCrouch; }

private:
	/** Plays sound effect according to movement and surface */
	void PlayMoveSound(float DeltaTime);
	class UBSMoveStepSound* GetMoveStepSoundBySurface(EPhysicalSurface SurfaceType) const;
	virtual void PlayJumpSound(const FHitResult& Hit, bool bJumped);
	
	float DefaultStepHeight;
	float DefaultWalkableFloorZ;
	float LocalSurfaceFriction;

	/** The time that the player can remount on the ladder */
	float OffLadderTicks = -1.0f;

	bool bHasDeferredMovementMode;
	EMovementMode DeferredMovementMode;
};

