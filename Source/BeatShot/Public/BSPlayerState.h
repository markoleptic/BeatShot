// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "BSPlayerState.generated.h"

class UBSAbilitySystemComponent;

/** Base player state class for this game */
UCLASS()
class BEATSHOT_API ABSPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABSPlayerState();

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "BeatShot|PlayerState")
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const { return AbilitySystemComponent; }

	virtual void PostInitializeComponents() override;

	class UBSAttributeSetBase* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "BeatShot|Attributes")
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "BeatShot|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "BeatShot|Attributes")
	float GetMaxHealth() const;

protected:
	UPROPERTY()
	UBSAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UBSAttributeSetBase* AttributeSetBase;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle MoveSpeedChangedDelegateHandle;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void MoveSpeedChanged(const FOnAttributeChangeData& Data);
	virtual void TagChange_State_Sprint(const FGameplayTag CallbackTag, int32 NewCount);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
