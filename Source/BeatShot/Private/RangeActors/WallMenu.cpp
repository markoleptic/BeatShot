// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "RangeActors/WallMenu.h"
#include "Character/BSCharacter.h"
#include "BSGameInstance.h"
#include "../../../../../../Epic Games/UE_5.2_Source_Installed/Engine/Plugins/Experimental/Text3D/Source/Text3D/Public/Text3DComponent.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "BeatShot/BSGameplayTags.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AWallMenu::AWallMenu()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	Corkboard = CreateDefaultSubobject<UStaticMeshComponent>("Corkboard");
	Corkboard->SetupAttachment(RootComponent);
	Corkboard->SetRelativeLocation(Position_Corkboard);
	Corkboard->SetGenerateOverlapEvents(false);
	Corkboard->SetCollisionProfileName(FName("BlockAll"));

	// Create ability system component, and set it to be explicitly replicated
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>("Ability System Component");
		// Minimal Mode means that no GameplayEffects will replicate. They will only live on the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	}
	
	MainText_Enable_LightVisualizers = CreateDefaultSubobject<UText3DComponent>(FName("Enable Light Visualizers"));
	SetupMainText(MainText_Enable_LightVisualizers, Corkboard, true, "WallMenu_LightVisualizer");

	ToggleText_LightVisualizers_On = CreateDefaultSubobject<UText3DComponent>(FName("Light Visualizers On"));
	ToggleText_LightVisualizers_Off = CreateDefaultSubobject<UText3DComponent>(FName("Light Visualizers Off"));
	Box_LightVisualizers_On = CreateDefaultSubobject<UBoxComponent>(FName("Box Light Visualizers On"));
	Box_LightVisualizers_Off = CreateDefaultSubobject<UBoxComponent>(FName("Box Light Visualizers Off"));
	
	SetupToggleText(MainText_Enable_LightVisualizers, ToggleText_LightVisualizers_On, ToggleText_LightVisualizers_Off,
		Box_LightVisualizers_On, Box_LightVisualizers_Off);
	
	MainText_Enable_NightMode = CreateDefaultSubobject<UText3DComponent>(FName("Enable Night Mode"));
	SetupMainText(MainText_Enable_NightMode, MainText_Enable_LightVisualizers, false, "WallMenu_NightMode");

	ToggleText_NightMode_On = CreateDefaultSubobject<UText3DComponent>(FName("Night Mode On"));
	ToggleText_NightMode_Off = CreateDefaultSubobject<UText3DComponent>(FName("Night Mode Off"));
	Box_NightMode_On = CreateDefaultSubobject<UBoxComponent>(FName("Box Night Mode On"));
	Box_NightMode_Off = CreateDefaultSubobject<UBoxComponent>(FName("Box Night Mode Off"));
	
	SetupToggleText(MainText_Enable_NightMode, ToggleText_NightMode_On, ToggleText_NightMode_Off,
	Box_NightMode_On, Box_NightMode_Off);
}

void AWallMenu::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_Game);
		GI->GetPublicGameSettingsChangedDelegate().AddUniqueDynamic(this, &AWallMenu::OnPlayerSettingsChanged_Game);
		GI->GetPublicUserSettingsChangedDelegate().AddUniqueDynamic(this, &AWallMenu::OnPlayerSettingsChanged_User);
	}

	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, nullptr);
		GetAbilitySystemComponent()->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::OnGameplayEffectAppliedToSelf);
		Init(LoadPlayerSettings().Game, LoadPlayerSettings().User);
	}
}

UAbilitySystemComponent* AWallMenu::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AWallMenu::OnGameplayEffectAppliedToSelf(UAbilitySystemComponent* ABS, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle)
{
	FGameplayTagContainer Container;
	EffectSpec.GetAllAssetTags(Container);
	const FHitResult* Hit = EffectSpec.GetEffectContext().GetHitResult();

	if (!Hit || !Container.HasTagExact(FBSGameplayTags::Get().Ability_Fire))
	{
		return;
	}
	
	if (UPrimitiveComponent* HitComponent = Hit->GetComponent())
	{
		UBoxComponent* Box = Cast<UBoxComponent>(HitComponent);
		const FText3DToggle* Found = BoxToTextMap.Find(Box);
		if (Found)
		{
			FPlayerSettings_Game PlayerSettings_Game = LoadPlayerSettings().Game;
			if (Found->SettingType == "bNightModeSelected" && LoadPlayerSettings().User.bNightModeUnlocked)
			{
				PlayerSettings_Game.bNightModeSelected = Found->bIsOnText;
			}
			else if (Found->SettingType == "bShowLightVisualizers")
			{
				PlayerSettings_Game.bShowLightVisualizers = Found->bIsOnText;
			}
			SavePlayerSettings(PlayerSettings_Game);
			ToggleText(Found->bIsOnText, Found->OnText.Get(), Found->OffText.Get());
		}
	}
}

void AWallMenu::Init(const FPlayerSettings_Game& GameSettings, const FPlayerSettings_User& UserSettings)
{
	ApplyMainTextMaterials();
	ToggleText(GameSettings.bShowLightVisualizers, ToggleText_LightVisualizers_On, ToggleText_LightVisualizers_Off);
	ToggleText(GameSettings.bNightModeSelected, ToggleText_NightMode_On, ToggleText_NightMode_Off);
	
	MainText_Enable_NightMode->SetVisibility(UserSettings.bNightModeUnlocked);
	ToggleText_NightMode_On->SetVisibility(UserSettings.bNightModeUnlocked);
	ToggleText_NightMode_Off->SetVisibility(UserSettings.bNightModeUnlocked, !UserSettings.bNightModeUnlocked);
	
	if (BoxToTextMap.IsEmpty())
	{
		BoxToTextMap.Add(Box_LightVisualizers_On, FText3DToggle(nullptr, Box_LightVisualizers_On, ToggleText_LightVisualizers_On, ToggleText_LightVisualizers_Off,
			true, "bShowLightVisualizers"));
		BoxToTextMap.Add(Box_LightVisualizers_Off, FText3DToggle(nullptr, Box_LightVisualizers_Off, ToggleText_LightVisualizers_On, ToggleText_LightVisualizers_Off,
	false, "bShowLightVisualizers"));
		BoxToTextMap.Add(Box_NightMode_On, FText3DToggle(nullptr, Box_NightMode_On, ToggleText_NightMode_On, ToggleText_NightMode_Off,
	true, "bNightModeSelected"));
		BoxToTextMap.Add(Box_NightMode_Off, FText3DToggle(nullptr, Box_NightMode_Off, ToggleText_NightMode_On, ToggleText_NightMode_Off,
	true, "bNightModeSelected"));
	}
}

void AWallMenu::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	Init(GameSettings, LoadPlayerSettings().User);
}

void AWallMenu::OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings)
{
	Init(LoadPlayerSettings().Game, UserSettings);
}

void AWallMenu::SetupMainText(UText3DComponent* InComponent, USceneComponent* InParent, const bool bFirstText, const FString& Key) const
{
	InComponent->SetupAttachment(InParent);
	
	if (bFirstText)
	{
		InComponent->SetRelativeLocation(Position_FirstMainText);
		InComponent->SetRelativeRotation(Rotation_FirstMainText);
	}
	else
	{
		InComponent->SetRelativeLocation(Offset_MainText);
	}
	
	InComponent->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", Key));
	InComponent->SetExtrude(5.f);
	InComponent->SetBevel(2.f);
	InComponent->SetCastShadow(false);
	InComponent->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Left);
	
	if (Material_Main_Front_Text3D)
	{
		InComponent->SetFrontMaterial(Material_Main_Front_Text3D);
	}
	if (Material_Main_Extrude_Text3D)
	{
		InComponent->SetExtrudeMaterial(Material_Main_Extrude_Text3D);
		InComponent->SetBevelMaterial(Material_Main_Extrude_Text3D);
	}
	if (Font_Text3D)
	{
		InComponent->SetFont(Font_Text3D);
	}
}

void AWallMenu::SetupToggleText(USceneComponent* InParent, UText3DComponent* InToggleTextOn, UText3DComponent* InToggleTextOff, UBoxComponent* InBoxOn, UBoxComponent* InBoxOff) const
{
	InToggleTextOn->SetupAttachment(InParent);
	InToggleTextOn->SetRelativeLocation(Offset_OnText);
	InToggleTextOn->SetExtrude(5.f);
	InToggleTextOn->SetBevel(2.f);
	InToggleTextOn->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "WallMenu_On"));
	InToggleTextOn->SetCastShadow(false);
	InToggleTextOn->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Center);
	
	InToggleTextOff->SetupAttachment(InParent);
	InToggleTextOff->SetRelativeLocation(Offset_OffText);
	InToggleTextOff->SetExtrude(5.f);
	InToggleTextOff->SetBevel(2.f);
	InToggleTextOff->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "WallMenu_Off"));
	InToggleTextOff->SetCastShadow(false);
	InToggleTextOff->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Center);

	if (Material_ToggleActive_Front_Text3D)
	{
		InToggleTextOn->SetFrontMaterial(Material_ToggleActive_Front_Text3D);
	}
	
	if (Material_ToggleInactive_Front_Text3D)
	{
		InToggleTextOff->SetBevelMaterial(Material_Toggle_Extrude_Text3D);
	}
	
	if (Font_Text3D)
	{
		InToggleTextOn->SetFont(Font_Text3D);
		InToggleTextOff->SetFont(Font_Text3D);
	}
	
	InBoxOn->SetupAttachment(InToggleTextOn);
	InBoxOn->SetRelativeLocation(Position_BoxCollision);
	InBoxOn->SetRelativeScale3D(Scale_BoxCollision_On);
	InBoxOn->SetBoxExtent(Extents_BoxCollision);
	InBoxOn->SetGenerateOverlapEvents(false);
	InBoxOn->SetCollisionProfileName(FName("BlockAll"));
	
	InBoxOff->SetupAttachment(InToggleTextOff);
	InBoxOff->SetRelativeLocation(Position_BoxCollision);
	InBoxOff->SetRelativeScale3D(Scale_BoxCollision_Off);
	InBoxOff->SetBoxExtent(Extents_BoxCollision);
	InBoxOff->SetGenerateOverlapEvents(false);
	InBoxOff->SetCollisionProfileName(FName("BlockAll"));
}

void AWallMenu::ToggleText(const bool bIsOn, UText3DComponent* InToggleTextOn, UText3DComponent* InToggleTextOff) const
{
	if (!Material_ToggleActive_Front_Text3D || !Material_ToggleInactive_Front_Text3D)
	{
		return;
	}
	if (bIsOn)
	{
		InToggleTextOn->SetFrontMaterial(Material_ToggleActive_Front_Text3D);
		InToggleTextOff->SetFrontMaterial(Material_ToggleInactive_Front_Text3D);
	}
	else
	{
		InToggleTextOn->SetFrontMaterial(Material_ToggleInactive_Front_Text3D);
		InToggleTextOff->SetFrontMaterial(Material_ToggleActive_Front_Text3D);
	}
}

void AWallMenu::ApplyMainTextMaterials() const
{
	if (Material_Main_Front_Text3D)
	{
		MainText_Enable_LightVisualizers->SetFrontMaterial(Material_Main_Front_Text3D);
		MainText_Enable_NightMode->SetFrontMaterial(Material_Main_Front_Text3D);
	}

	if (Material_Main_Extrude_Text3D)
	{
		MainText_Enable_LightVisualizers->SetExtrudeMaterial(Material_Main_Extrude_Text3D);
		MainText_Enable_LightVisualizers->SetBevelMaterial(Material_Main_Extrude_Text3D);
		MainText_Enable_NightMode->SetExtrudeMaterial(Material_Main_Extrude_Text3D);
		MainText_Enable_NightMode->SetBevelMaterial(Material_Main_Extrude_Text3D);
	}

	if (Font_Text3D)
	{
		MainText_Enable_LightVisualizers->SetFont(Font_Text3D);
		MainText_Enable_NightMode->SetFont(Font_Text3D);
	}
}
