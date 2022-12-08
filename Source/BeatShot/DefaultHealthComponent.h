// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DefaultHealthComponent.generated.h"


class UDefaultGameInstance;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATSHOT_API UDefaultHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/* Sets default values for this component's properties */
	UDefaultHealthComponent();

protected:
	/* Called when the game starts */
	virtual void BeginPlay() override;

public:	
	/* Called every frame */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Sets the max health of the component */
	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float NewMaxHealth);

	/* The maximum amount of damage that can be dealt to a target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalPossibleDamage;

	/* Whether or not to update TotalPossibleDamage */
	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
	bool ShouldUpdateTotalPossibleDamage;

private:
	/* Default health value for a target */
	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.f;

	/* Current health value for a target */
	UPROPERTY(VisibleAnywhere)
	float Health = 0.f;

	/* Called when a target takes damage */
	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator,
		AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere)
	UDefaultGameInstance* GI;
};
