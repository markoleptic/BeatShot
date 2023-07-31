// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "SaveLoadInterface.h"
#include "WallMenu.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UBSAbilitySystemComponent;
class UBoxComponent;
class UText3DComponent;

USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FText3DToggle
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USceneComponent> Parent;
	TObjectPtr<UBoxComponent> CollisionReceiver;
	TObjectPtr<UText3DComponent> OnText;
	TObjectPtr<UText3DComponent> OffText;
	FGuid Guid;
	bool bIsOnText;
	FString SettingType;

	FText3DToggle()
	{
		bIsOnText = false;
		Parent = nullptr;
		CollisionReceiver = nullptr;
		OnText = nullptr;
		OffText = nullptr;
		Guid = FGuid::NewGuid();
		SettingType = FString();
	}

	FText3DToggle(USceneComponent* InParent, UBoxComponent* Collision, UText3DComponent* InOnText, UText3DComponent* InOffText, const bool bInIsOnText, const FString InSettingType)
	{
		bIsOnText = bInIsOnText;
		Parent = InParent;
		CollisionReceiver = Collision;
		OnText = InOnText;
		OffText = InOffText;
		Guid = FGuid::NewGuid();
		SettingType = InSettingType;
	}
	~FText3DToggle()
	{
		Parent = nullptr;
		CollisionReceiver = nullptr;
		OnText = nullptr;
		OffText = nullptr;
		Guid.Invalidate();
		SettingType.Empty();
	}

	FORCEINLINE bool operator==(const FText3DToggle& Other) const
	{
		if (!Guid.IsValid() || !Other.Guid.IsValid())
		{
			return false;
		}
		return Guid == Other.Guid;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FText3DToggle& Text3DToggle)
	{
		return GetTypeHash(Text3DToggle.Guid);
	}
};


/** Displays some settings on a wall that can be changed by shooting them */
UCLASS()
class BEATSHOT_API AWallMenu : public AActor, public ISaveLoadInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

	AWallMenu();
	
	virtual void PostInitializeComponents() override;

	/* ~Begin IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/* ~End IAbilitySystemInterface */

	void OnGameplayEffectAppliedToSelf(UAbilitySystemComponent* ABS, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle);

protected:
	
	UPROPERTY()
	UBSAbilitySystemComponent* AbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Corkboard;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UText3DComponent* MainText_Enable_LightVisualizers;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UText3DComponent* ToggleText_LightVisualizers_On;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UText3DComponent* ToggleText_LightVisualizers_Off;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBoxComponent* Box_LightVisualizers_On;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBoxComponent* Box_LightVisualizers_Off;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UText3DComponent* MainText_Enable_NightMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UText3DComponent* ToggleText_NightMode_On;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UText3DComponent* ToggleText_NightMode_Off;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBoxComponent* Box_NightMode_On;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBoxComponent* Box_NightMode_Off;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Text3D|Main")
	UMaterialInterface* Material_Main_Front_Text3D;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Text3D|Main")
	UMaterialInterface* Material_Main_Extrude_Text3D;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Text3D|Toggle")
	UMaterialInterface* Material_ToggleActive_Front_Text3D;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Text3D|Toggle")
	UMaterialInterface* Material_ToggleInactive_Front_Text3D;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Text3D|Toggle")
	UMaterialInterface* Material_Toggle_Extrude_Text3D;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Text3D")
	UFont* Font_Text3D;
	
	/** Displays which settings are on/off etc by lighting the correct words */
	void Init(const FPlayerSettings_Game& GameSettings, const FPlayerSettings_User& UserSettings);
	
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	virtual void OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings) override;

	void SetupMainText(UText3DComponent* InComponent, USceneComponent* InParent, const bool bFirstText, const FString& Key) const;
	void SetupToggleText(USceneComponent* InParent, UText3DComponent* InToggleTextOn, UText3DComponent* InToggleTextOff, UBoxComponent* InBoxOn, UBoxComponent* InBoxOff) const;
	void ToggleText(const bool bIsOn, UText3DComponent* InToggleTextOn, UText3DComponent* InToggleTextOff) const;
	void ApplyMainTextMaterials() const;

	FVector Position_Corkboard = { 300.f, 50.f, 0.f };
	
	/** Offset from main text to to the on text */
	FVector Offset_OnText = { 0, 747.f, 0};
	/** Offset from main text to to the off text */
	FVector Offset_OffText = { 0, 944.f, 0};

	/** Offset from Corkboard to first main text */
	FVector Position_FirstMainText = { 310.f, 40.f, 55.f};
	/** Rotation first main text */
	FRotator Rotation_FirstMainText = { -90.f, 0.f, 0.f};
	/** Offset from previous main text */
	FVector Offset_MainText = { 0.f, 0.f, -65.f};


	/** Offset from Toggle text to box component */
	FVector Position_BoxCollision = { 2.f, 0, 22.5f };
	/** Box component extents */
	FVector Extents_BoxCollision = { 1.f, 32.f, 32.f };
	/** Box component scale for on */
	FVector Scale_BoxCollision_On = { 3.f, 1.75f, 0.95f };
	/** Box component scale for off */
	FVector Scale_BoxCollision_Off = { 3.f, 2.25f, 0.95f };

	TMap<TObjectPtr<UBoxComponent>, FText3DToggle> BoxToTextMap;
};
