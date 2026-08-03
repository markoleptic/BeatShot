// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "RangeActors/WallMenu.h"
#include "BSGameInstance.h"
#include "Text3DComponent.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "BeatShot/BSGameplayTags.h"
#include "Components/BoxComponent.h"
#include "Extensions/Text3DDefaultMaterialExtension.h"
#include "Kismet/GameplayStatics.h"
#include "RangeActors/TimeOfDayManager.h"
#include "SaveGames/SaveGamePlayerSettings.h"

namespace
{
auto LeftPillarLocation = FVector(0.0, 175.0, 25.0);
auto RightPillarLocation = FVector(0.0, 1025.0, 25.0);
auto PillarRotation = FRotator(-90.0, 0.0, 0.0);
auto BoardOutlineLocation = FVector(350.0, 1150.00, -12.50);
auto BoardOutlineScale = FVector(1.0, 1.0, 1.5);
}

AWallMenu::AWallMenu()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	Corkboard = CreateDefaultSubobject<UStaticMeshComponent>("Corkboard");
	Corkboard->SetupAttachment(RootComponent);
	Corkboard->SetRelativeLocation(Position_Corkboard);
	Corkboard->SetGenerateOverlapEvents(false);
	Corkboard->SetCollisionProfileName(FName("BlockAll"));

	SM_LeftPillar = CreateDefaultSubobject<UStaticMeshComponent>("SM_LeftPillar");
	SM_LeftPillar->SetupAttachment(RootComponent);
	SM_LeftPillar->SetRelativeLocation(LeftPillarLocation);
	SM_LeftPillar->SetRelativeRotation(PillarRotation);
	SM_LeftPillar->SetGenerateOverlapEvents(false);
	SM_LeftPillar->SetCollisionProfileName(FName("BlockAll"));

	SM_RightPillar = CreateDefaultSubobject<UStaticMeshComponent>("SM_RightPillar");
	SM_RightPillar->SetupAttachment(RootComponent);
	SM_RightPillar->SetRelativeLocation(RightPillarLocation);
	SM_RightPillar->SetRelativeRotation(PillarRotation);
	SM_RightPillar->SetGenerateOverlapEvents(false);
	SM_RightPillar->SetCollisionProfileName(FName("BlockAll"));

	SM_WallMenuBoardOutline = CreateDefaultSubobject<UStaticMeshComponent>("SM_WallMenuBoardOutline");
	SM_WallMenuBoardOutline->SetupAttachment(RootComponent);
	SM_WallMenuBoardOutline->SetRelativeLocation(BoardOutlineLocation);
	SM_WallMenuBoardOutline->SetRelativeScale3D(BoardOutlineScale);
	SM_WallMenuBoardOutline->SetGenerateOverlapEvents(false);
	SM_WallMenuBoardOutline->SetCollisionProfileName(FName("BlockAll"));

	// Create ability system component, and set it to be explicitly replicated
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>("Ability System Component");
		// Minimal Mode means that no GameplayEffects will replicate. They will only live on the Server.
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	}
}

void AWallMenu::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, nullptr);
		GetAbilitySystemComponent()->OnGameplayEffectAppliedDelegateToSelf.AddUObject(
			this, &ThisClass::OnGameplayEffectAppliedToSelf);
	}
}

void AWallMenu::BeginPlay()
{
	Super::BeginPlay();

	// Main Light Visualizer Enable
	MainText_Enable_LightVisualizers = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                               FName("Enable Light Visualizers"));

	SetupMainText(MainText_Enable_LightVisualizers, Corkboard, true, "LightVisualizer");

	ToggleText_LightVisualizers_On = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                             FName("Light Visualizers On"));
	ToggleText_LightVisualizers_Off = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                              FName("Light Visualizers Off"));
	Box_LightVisualizers_On = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(),
	                                                   FName("Box Light Visualizers On"));
	Box_LightVisualizers_Off = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(),
	                                                    FName("Box Light Visualizers Off"));

	SetupToggleText(MainText_Enable_LightVisualizers, ToggleText_LightVisualizers_On, ToggleText_LightVisualizers_Off,
	                Box_LightVisualizers_On, Box_LightVisualizers_Off);

	// Front Beam Visualizer
	MainText_Enable_LVFrontBeam = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                          FName("Enable Front Beam"));
	SetupMainText(MainText_Enable_LVFrontBeam, MainText_Enable_LightVisualizers, false, "LVFrontBeam", Indent);

	ToggleText_LVFrontBeam_On = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                        FName("Front Beam On"));
	ToggleText_LVFrontBeam_Off = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                         FName("Front Beam Off"));
	Box_LVFrontBeam_On = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Front Beam On"));
	Box_LVFrontBeam_Off = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Front Beam Off"));

	SetupToggleText(MainText_Enable_LVFrontBeam, ToggleText_LVFrontBeam_On, ToggleText_LVFrontBeam_Off,
	                Box_LVFrontBeam_On, Box_LVFrontBeam_Off, -Indent);

	// Top Beam Visualizer
	MainText_EnableLV_TopBeam = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                        FName("Enable Top Beam"));
	SetupMainText(MainText_EnableLV_TopBeam, MainText_Enable_LightVisualizers, false, "LVTopBeam",
	              Offset_MainText + Indent);

	ToggleText_LV_TopBeam_On = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(), FName("Top Beam On"));
	ToggleText_LV_TopBeam_Off = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                        FName("Top Beam Off"));
	Box_LV_TopBeam_On = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Top Beam On"));
	Box_LV_TopBeam_Off = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Top Beam Off"));

	SetupToggleText(MainText_EnableLV_TopBeam, ToggleText_LV_TopBeam_On, ToggleText_LV_TopBeam_Off, Box_LV_TopBeam_On,
	                Box_LV_TopBeam_Off, -Indent);

	// Left Beam Visualizer
	MainText_Enable_LVLeftBeam = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                         FName("Enable Left Beam"));
	SetupMainText(MainText_Enable_LVLeftBeam, MainText_Enable_LightVisualizers, false, "LVLeftBeam",
	              Offset_MainText * 2 + Indent);

	ToggleText_LVLeftBeam_On = NewObject<
		UText3DComponent>(this, UText3DComponent::StaticClass(), FName("Left Beam On"));
	ToggleText_LVLeftBeam_Off = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                        FName("Left Beam Off"));
	Box_LVLeftBeam_On = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Left Beam On"));
	Box_LVLeftBeam_Off = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Left Beam Off"));

	SetupToggleText(MainText_Enable_LVLeftBeam, ToggleText_LVLeftBeam_On, ToggleText_LVLeftBeam_Off, Box_LVLeftBeam_On,
	                Box_LVLeftBeam_Off, -Indent);

	// Right Beam Visualizer
	MainText_Enable_LVRightBeam = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                          FName("Enable Right Beam"));
	SetupMainText(MainText_Enable_LVRightBeam, MainText_Enable_LightVisualizers, false, "LVRightBeam",
	              Offset_MainText * 3 + Indent);

	ToggleText_LVRightBeam_On = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                        FName("Right Beam On"));
	ToggleText_LVRightBeam_Off = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                         FName("Right Beam Off"));
	Box_LVRightBeam_On = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Right Beam On"));
	Box_LVRightBeam_Off = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Right Beam Off"));

	SetupToggleText(MainText_Enable_LVRightBeam, ToggleText_LVRightBeam_On, ToggleText_LVRightBeam_Off,
	                Box_LVRightBeam_On, Box_LVRightBeam_Off, -Indent);

	// Left Cube Visualizer
	MainText_EnableLV_LeftCube = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                         FName("Enable Left Cube"));
	SetupMainText(MainText_EnableLV_LeftCube, MainText_Enable_LightVisualizers, false, "LVLeftCube",
	              Offset_MainText * 4 + Indent);

	ToggleText_LV_LeftCube_On = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                        FName("Left Cube On"));
	ToggleText_LV_LeftCube_Off = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                         FName("Left Cube Off"));
	Box_LV_LeftCube_On = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Left Cube On"));
	Box_LV_LeftCube_Off = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Left Cube Off"));

	SetupToggleText(MainText_EnableLV_LeftCube, ToggleText_LV_LeftCube_On, ToggleText_LV_LeftCube_Off,
	                Box_LV_LeftCube_On, Box_LV_LeftCube_Off, -Indent);

	// Right Cube Visualizer
	MainText_EnableLV_RightCube = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                          FName("Enable Right Cube"));
	SetupMainText(MainText_EnableLV_RightCube, MainText_Enable_LightVisualizers, false, "LVRightCube",
	              Offset_MainText * 5 + Indent);

	ToggleText_LV_RightCube_On = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                         FName("Right Cube On"));
	ToggleText_LV_RightCube_Off = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                          FName("Right Cube Off"));
	Box_LV_RightCube_On = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Right Cube On"));
	Box_LV_RightCube_Off = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Right Cube Off"));

	SetupToggleText(MainText_EnableLV_RightCube, ToggleText_LV_RightCube_On, ToggleText_LV_RightCube_Off,
	                Box_LV_RightCube_On, Box_LV_RightCube_Off, -Indent);


	// Night Mode Enable
	MainText_Enable_NightMode = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                        FName("Enable Night Mode"));
	SetupMainText(MainText_Enable_NightMode, MainText_Enable_LightVisualizers, false, "NightMode", Offset_MainText * 6);

	ToggleText_NightMode_On = NewObject<
		UText3DComponent>(this, UText3DComponent::StaticClass(), FName("Night Mode On"));
	ToggleText_NightMode_Off = NewObject<UText3DComponent>(this, UText3DComponent::StaticClass(),
	                                                       FName("Night Mode Off"));
	Box_NightMode_On = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Night Mode On"));
	Box_NightMode_Off = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Box Night Mode Off"));

	SetupToggleText(MainText_Enable_NightMode, ToggleText_NightMode_On, ToggleText_NightMode_Off, Box_NightMode_On,
	                Box_NightMode_Off);

	if (UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		GI->RegisterPlayerSettingsUpdaters(OnPlayerSettingsChangedDelegate_Game);
		GI->RegisterPlayerSettingsSubscriber<
			AWallMenu, FPlayerSettings_Game>(this, &AWallMenu::OnPlayerSettingsChanged);
		GI->RegisterPlayerSettingsSubscriber<
			AWallMenu, FPlayerSettings_User>(this, &AWallMenu::OnPlayerSettingsChanged);
	}

	if (TimeOfDayManager)
	{
		TimeOfDayManager->OnTimeOfDayTransitionCompleted.BindUObject(this, &ThisClass::OnTimeOfDayChangeCompleted);
	}

	BoxToTextMap.Add(Box_LightVisualizers_On, FText3DToggle(MainText_Enable_LightVisualizers, Box_LightVisualizers_On,
	                                                        ToggleText_LightVisualizers_On,
	                                                        ToggleText_LightVisualizers_Off, true,
	                                                        "bShowLightVisualizers"));
	BoxToTextMap.Add(Box_LightVisualizers_Off, FText3DToggle(MainText_Enable_LightVisualizers, Box_LightVisualizers_Off,
	                                                         ToggleText_LightVisualizers_On,
	                                                         ToggleText_LightVisualizers_Off, false,
	                                                         "bShowLightVisualizers"));

	BoxToTextMap.Add(Box_LVFrontBeam_On, FText3DToggle(MainText_Enable_LVFrontBeam, Box_LVFrontBeam_On,
	                                                   ToggleText_LVFrontBeam_On, ToggleText_LVFrontBeam_Off, true,
	                                                   "bShow_LVFrontBeam"));
	BoxToTextMap.Add(Box_LVFrontBeam_Off, FText3DToggle(MainText_Enable_LVFrontBeam, Box_LVFrontBeam_Off,
	                                                    ToggleText_LVFrontBeam_On, ToggleText_LVFrontBeam_Off, false,
	                                                    "bShow_LVFrontBeam"));

	BoxToTextMap.Add(Box_LVLeftBeam_On, FText3DToggle(MainText_Enable_LVLeftBeam, Box_LVLeftBeam_On,
	                                                  ToggleText_LVLeftBeam_On, ToggleText_LVLeftBeam_Off, true,
	                                                  "bShow_LVLeftBeam"));
	BoxToTextMap.Add(Box_LVLeftBeam_Off, FText3DToggle(MainText_Enable_LVLeftBeam, Box_LVLeftBeam_Off,
	                                                   ToggleText_LVLeftBeam_On, ToggleText_LVLeftBeam_Off, false,
	                                                   "bShow_LVLeftBeam"));

	BoxToTextMap.Add(Box_LVRightBeam_On, FText3DToggle(MainText_Enable_LVRightBeam, Box_LVRightBeam_On,
	                                                   ToggleText_LVRightBeam_On, ToggleText_LVRightBeam_Off, true,
	                                                   "bShow_LVRightBeam"));
	BoxToTextMap.Add(Box_LVRightBeam_Off, FText3DToggle(MainText_Enable_LVRightBeam, Box_LVRightBeam_Off,
	                                                    ToggleText_LVRightBeam_On, ToggleText_LVRightBeam_Off, false,
	                                                    "bShow_LVRightBeam"));

	BoxToTextMap.Add(Box_LV_TopBeam_On, FText3DToggle(MainText_EnableLV_TopBeam, Box_LV_TopBeam_On,
	                                                  ToggleText_LV_TopBeam_On, ToggleText_LV_TopBeam_Off, true,
	                                                  "bShow_LVTopBeam"));
	BoxToTextMap.Add(Box_LV_TopBeam_Off, FText3DToggle(MainText_EnableLV_TopBeam, Box_LV_TopBeam_Off,
	                                                   ToggleText_LV_TopBeam_On, ToggleText_LV_TopBeam_Off, false,
	                                                   "bShow_LVTopBeam"));

	BoxToTextMap.Add(Box_LV_LeftCube_On, FText3DToggle(MainText_EnableLV_LeftCube, Box_LV_LeftCube_On,
	                                                   ToggleText_LV_LeftCube_On, ToggleText_LV_LeftCube_Off, true,
	                                                   "bShow_LVLeftCube"));
	BoxToTextMap.Add(Box_LV_LeftCube_Off, FText3DToggle(MainText_EnableLV_LeftCube, Box_LV_LeftCube_Off,
	                                                    ToggleText_LV_LeftCube_On, ToggleText_LV_LeftCube_Off, false,
	                                                    "bShow_LVLeftCube"));

	BoxToTextMap.Add(Box_LV_RightCube_On, FText3DToggle(MainText_EnableLV_RightCube, Box_LV_RightCube_On,
	                                                    ToggleText_LV_RightCube_On, ToggleText_LV_RightCube_Off, true,
	                                                    "bShow_LVRightCube"));
	BoxToTextMap.Add(Box_LV_RightCube_Off, FText3DToggle(MainText_EnableLV_RightCube, Box_LV_RightCube_Off,
	                                                     ToggleText_LV_RightCube_On, ToggleText_LV_RightCube_Off, false,
	                                                     "bShow_LVRightCube"));

	BoxToTextMap.Add(Box_NightMode_On, FText3DToggle(MainText_Enable_NightMode, Box_NightMode_On,
	                                                 ToggleText_NightMode_On, ToggleText_NightMode_Off, true,
	                                                 "bNightModeSelected"));
	BoxToTextMap.Add(Box_NightMode_Off, FText3DToggle(MainText_Enable_NightMode, Box_NightMode_Off,
	                                                  ToggleText_NightMode_On, ToggleText_NightMode_Off, false,
	                                                  "bNightModeSelected"));

	for (TPair<TObjectPtr<UBoxComponent>, FText3DToggle>& Pair : BoxToTextMap)
	{
		if (Pair.Value.Parent)
		{
			if (UText3DComponent* CastedParent = Cast<UText3DComponent>(Pair.Value.Parent))
			{
				const auto MaterialExtension = CastedParent->GetCastedMaterialExtension<
					UText3DDefaultMaterialExtension>();
				MaterialExtension->SetStyle(EText3DMaterialStyle::Custom);
				if (Material_Main_Front_Text3D)
				{
					MaterialExtension->SetFrontMaterial(Material_Main_Front_Text3D);
				}
				if (Material_Bevel)
				{
					MaterialExtension->SetBevelMaterial(Material_Bevel);
				}
				if (Material_Extrude)
				{
					MaterialExtension->SetExtrudeMaterial(Material_Extrude);
				}
				if (Font_Text3D)
				{
					CastedParent->SetFont(Font_Text3D);
				}
			}
		}
		if (!Pair.Value.OnText || !Pair.Value.OffText)
		{
			continue;
		}

		const auto OnTextMaterialExtension = Pair.Value.OnText->GetCastedMaterialExtension<
			UText3DDefaultMaterialExtension>();
		OnTextMaterialExtension->SetStyle(EText3DMaterialStyle::Custom);
		const auto OffTextMaterialExtension = Pair.Value.OffText->GetCastedMaterialExtension<
			UText3DDefaultMaterialExtension>();
		OffTextMaterialExtension->SetStyle(EText3DMaterialStyle::Custom);

		if (Material_Bevel)
		{
			OnTextMaterialExtension->SetBevelMaterial(Material_Bevel);
			OffTextMaterialExtension->SetBevelMaterial(Material_Bevel);
		}
		if (Material_Extrude)
		{
			OnTextMaterialExtension->SetExtrudeMaterial(Material_Extrude);
			OffTextMaterialExtension->SetExtrudeMaterial(Material_Extrude);
		}
		if (Font_Text3D)
		{
			Pair.Value.OnText->SetFont(Font_Text3D);
			Pair.Value.OffText->SetFont(Font_Text3D);
		}

		if (!Material_Toggle)
		{
			continue;
		}

		UMaterialInstanceDynamic* DynamicMatFrontOn = UMaterialInstanceDynamic::Create(Material_Toggle,
			Pair.Value.OnText.Get());
		UMaterialInstanceDynamic* DynamicMatFrontOff = UMaterialInstanceDynamic::Create(Material_Toggle,
			Pair.Value.OffText.Get());

		if (!DynamicMatFrontOn || !DynamicMatFrontOff)
		{
			continue;
		}
		OnTextMaterialExtension->SetFrontMaterial(DynamicMatFrontOn);
		OffTextMaterialExtension->SetFrontMaterial(DynamicMatFrontOff);
	}

	const FPlayerSettings PlayerSettings = LoadPlayerSettings();
	bNightModeUnlockedAtInit = PlayerSettings.User.bNightModeUnlocked;

	SetNightModeTextVisibility(bNightModeUnlockedAtInit);
	ToggleAllText(PlayerSettings.Game, bNightModeUnlockedAtInit);

	SetDynamicMaterialPulseValue(ToggleText_NightMode_On, 0.0f);
	SetDynamicMaterialPulseValue(ToggleText_NightMode_Off, 0.0f);
	bIsWaitingOnTimeOfDayTransition = false;
}

UAbilitySystemComponent* AWallMenu::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AWallMenu::OnGameplayEffectAppliedToSelf(UAbilitySystemComponent* /*ABS*/,
                                              const FGameplayEffectSpec& EffectSpec,
                                              FActiveGameplayEffectHandle /*EffectHandle*/)
{
	FGameplayTagContainer Container;
	EffectSpec.GetAllAssetTags(Container);
	const FHitResult* Hit = EffectSpec.GetEffectContext().GetHitResult();

	if (!Hit || !Container.HasTagExact(BSGameplayTags::Ability_Fire))
	{
		return;
	}

	if (UPrimitiveComponent* HitComponent = Hit->GetComponent())
	{
		UBoxComponent* Box = Cast<UBoxComponent>(HitComponent);
		if (const FText3DToggle* Found = BoxToTextMap.Find(Box))
		{
			bool bRequiresSave = false;
			FPlayerSettings PlayerSettings = LoadPlayerSettings();
			if (Found->SettingType == "bNightModeSelected" && PlayerSettings.User.bNightModeUnlocked)
			{
				if (!bIsWaitingOnTimeOfDayTransition)
				{
					bRequiresSave = PlayerSettings.Game.bNightModeSelected != Found->bIsOnText || bRequiresSave;
					PlayerSettings.Game.bNightModeSelected = Found->bIsOnText;
				}
			}
			else if (Found->SettingType == "bShowLightVisualizers")
			{
				// Don't do anything if its already on
				if (Found->bIsOnText && PlayerSettings.Game.bShowLightVisualizers)
				{
					return;
				}

				bRequiresSave = true;
				PlayerSettings.Game.bShowLightVisualizers = Found->bIsOnText;

				// By default, only turn on 3 lights when initially toggling on
				if (Found->bIsOnText)
				{
					ToggleText(true, ToggleText_LV_TopBeam_On, ToggleText_LV_TopBeam_Off);
					ToggleText(true, ToggleText_LV_LeftCube_On, ToggleText_LV_LeftCube_Off);
					ToggleText(true, ToggleText_LV_RightCube_On, ToggleText_LV_RightCube_Off);
					ToggleText(false, ToggleText_LVFrontBeam_On, ToggleText_LVFrontBeam_Off);
					ToggleText(false, ToggleText_LVLeftBeam_On, ToggleText_LVLeftBeam_Off);
					ToggleText(false, ToggleText_LVRightBeam_On, ToggleText_LVRightBeam_Off);

					PlayerSettings.Game.bShow_LVTopBeam = true;
					PlayerSettings.Game.bShow_LVLeftCube = true;
					PlayerSettings.Game.bShow_LVRightCube = true;
					PlayerSettings.Game.bShow_LVFrontBeam = false;
					PlayerSettings.Game.bShow_LVLeftBeam = false;
					PlayerSettings.Game.bShow_LVRightBeam = false;
				}
				// Toggle all off if disabled
				else if (!Found->bIsOnText)
				{
					ToggleText(false, ToggleText_LV_TopBeam_On, ToggleText_LV_TopBeam_Off);
					ToggleText(false, ToggleText_LV_LeftCube_On, ToggleText_LV_LeftCube_Off);
					ToggleText(false, ToggleText_LV_RightCube_On, ToggleText_LV_RightCube_Off);
					ToggleText(false, ToggleText_LVFrontBeam_On, ToggleText_LVFrontBeam_Off);
					ToggleText(false, ToggleText_LVLeftBeam_On, ToggleText_LVLeftBeam_Off);
					ToggleText(false, ToggleText_LVRightBeam_On, ToggleText_LVRightBeam_Off);

					PlayerSettings.Game.bShow_LVTopBeam = false;
					PlayerSettings.Game.bShow_LVLeftCube = false;
					PlayerSettings.Game.bShow_LVRightCube = false;
					PlayerSettings.Game.bShow_LVFrontBeam = false;
					PlayerSettings.Game.bShow_LVLeftBeam = false;
					PlayerSettings.Game.bShow_LVRightBeam = false;
				}
			}
			else if (Found->SettingType == "bShow_LVFrontBeam")
			{
				bRequiresSave = PlayerSettings.Game.bShow_LVFrontBeam != Found->bIsOnText || bRequiresSave;
				PlayerSettings.Game.bShow_LVFrontBeam = Found->bIsOnText;
			}
			else if (Found->SettingType == "bShow_LVLeftBeam")
			{
				bRequiresSave = PlayerSettings.Game.bShow_LVLeftBeam != Found->bIsOnText || bRequiresSave;
				PlayerSettings.Game.bShow_LVLeftBeam = Found->bIsOnText;
			}
			else if (Found->SettingType == "bShow_LVRightBeam")
			{
				bRequiresSave = PlayerSettings.Game.bShow_LVRightBeam != Found->bIsOnText || bRequiresSave;
				PlayerSettings.Game.bShow_LVRightBeam = Found->bIsOnText;
			}
			else if (Found->SettingType == "bShow_LVTopBeam")
			{
				bRequiresSave = PlayerSettings.Game.bShow_LVTopBeam != Found->bIsOnText || bRequiresSave;
				PlayerSettings.Game.bShow_LVTopBeam = Found->bIsOnText;
			}
			else if (Found->SettingType == "bShow_LVLeftCube")
			{
				bRequiresSave = PlayerSettings.Game.bShow_LVLeftCube != Found->bIsOnText || bRequiresSave;
				PlayerSettings.Game.bShow_LVLeftCube = Found->bIsOnText;
			}
			else if (Found->SettingType == "bShow_LVRightCube")
			{
				bRequiresSave = PlayerSettings.Game.bShow_LVRightCube != Found->bIsOnText || bRequiresSave;
				PlayerSettings.Game.bShow_LVRightCube = Found->bIsOnText;
			}

			if (bRequiresSave)
			{
				SavePlayerSettings(PlayerSettings.Game);
				ToggleText(Found->bIsOnText, Found->OnText.Get(), Found->OffText.Get());
			}
		}
	}
}

void AWallMenu::OnTimeOfDayChangeCompleted(const ETimeOfDay NewTimeOfDay)
{
	SetDynamicMaterialPulseValue(ToggleText_NightMode_On, 0.0f);
	SetDynamicMaterialPulseValue(ToggleText_NightMode_Off, 0.0f);
	bIsWaitingOnTimeOfDayTransition = false;
}

void AWallMenu::ToggleAllText(const FPlayerSettings_Game& GameSettings, const bool bNightModeUnlocked) const
{
	ToggleText(GameSettings.bShowLightVisualizers, ToggleText_LightVisualizers_On, ToggleText_LightVisualizers_Off);
	ToggleNightModeText(GameSettings.bNightModeSelected && bNightModeUnlocked);
	ToggleText(GameSettings.bShow_LVTopBeam, ToggleText_LV_TopBeam_On, ToggleText_LV_TopBeam_Off);
	ToggleText(GameSettings.bShow_LVLeftCube, ToggleText_LV_LeftCube_On, ToggleText_LV_LeftCube_Off);
	ToggleText(GameSettings.bShow_LVRightCube, ToggleText_LV_RightCube_On, ToggleText_LV_RightCube_Off);
	ToggleText(GameSettings.bShow_LVFrontBeam, ToggleText_LVFrontBeam_On, ToggleText_LVFrontBeam_Off);
	ToggleText(GameSettings.bShow_LVLeftBeam, ToggleText_LVLeftBeam_On, ToggleText_LVLeftBeam_Off);
	ToggleText(GameSettings.bShow_LVRightBeam, ToggleText_LVRightBeam_On, ToggleText_LVRightBeam_Off);
}

void AWallMenu::OnPlayerSettingsChanged(const FPlayerSettings_Game& GameSettings)
{
	ToggleAllText(GameSettings, LoadPlayerSettings().User.bNightModeUnlocked);
}

void AWallMenu::OnPlayerSettingsChanged(const FPlayerSettings_User& UserSettings)
{
	SetNightModeTextVisibility(UserSettings.bNightModeUnlocked);

	if (!bNightModeUnlockedAtInit && UserSettings.bNightModeUnlocked)
	{
		bNightModeUnlockedAtInit = true;
		FPlayerSettings_Game NewGameSettings = LoadPlayerSettings().Game;
		NewGameSettings.bNightModeSelected = true;
		SavePlayerSettings(NewGameSettings);
	}
}

void AWallMenu::SetupMainText(UText3DComponent* InComponent,
                              USceneComponent* InParent,
                              const bool bFirstText,
                              const FString& Key,
                              const FVector& AdditionalOffset) const
{
	InComponent->AttachToComponent(InParent, FAttachmentTransformRules::KeepRelativeTransform);
	InComponent->RegisterComponent();
	InComponent->SetRelativeLocation(bFirstText ? Position_FirstMainText : Offset_MainText + AdditionalOffset);
	if (bFirstText)
	{
		InComponent->SetRelativeRotation(Rotation_FirstMainText);
	}
	InComponent->SetScaleProportionally(true);
	InComponent->SetHasMaxHeight(true);
	InComponent->SetMaxHeight(AdditionalOffset == FVector::ZeroVector ? MaxHeightMainText : MaxHeightIndentedText);
	InComponent->SetText(FText::FromStringTable("/Game/StringTables/ST_WallMenu.ST_WallMenu", Key));
	InComponent->SetExtrude(5.f);
	InComponent->SetBevel(2.f);
	InComponent->SetCastShadow(false);
	InComponent->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Left);
}

void AWallMenu::SetupToggleText(USceneComponent* InParent,
                                UText3DComponent* InToggleTextOn,
                                UText3DComponent* InToggleTextOff,
                                UBoxComponent* InBoxOn,
                                UBoxComponent* InBoxOff,
                                const FVector& AdditionalOffset)
{
	InToggleTextOn->AttachToComponent(InParent, FAttachmentTransformRules::KeepRelativeTransform);
	InToggleTextOn->RegisterComponent();
	InToggleTextOn->SetRelativeLocation(Offset_OnText + AdditionalOffset);
	InToggleTextOn->SetExtrude(5.f);
	InToggleTextOn->SetBevel(2.f);
	InToggleTextOn->SetText(FText::FromStringTable("/Game/StringTables/ST_WallMenu.ST_WallMenu", FString("On")));
	InToggleTextOn->SetCastShadow(false);
	InToggleTextOn->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Center);

	InToggleTextOff->AttachToComponent(InParent, FAttachmentTransformRules::KeepRelativeTransform);
	InToggleTextOff->RegisterComponent();
	InToggleTextOff->SetRelativeLocation(Offset_OffText + AdditionalOffset);
	InToggleTextOff->SetExtrude(5.f);
	InToggleTextOff->SetBevel(2.f);
	InToggleTextOff->SetText(FText::FromStringTable("/Game/StringTables/ST_WallMenu.ST_WallMenu", FString("Off")));
	InToggleTextOff->SetCastShadow(false);
	InToggleTextOff->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Center);

	InToggleTextOn->SetScaleProportionally(true);
	InToggleTextOff->SetScaleProportionally(true);
	InToggleTextOn->SetHasMaxHeight(true);
	InToggleTextOff->SetHasMaxHeight(true);

	if (AdditionalOffset == FVector::ZeroVector)
	{
		InToggleTextOn->SetMaxHeight(MaxHeightMainText);
		InToggleTextOff->SetMaxHeight(MaxHeightMainText);
	}
	else
	{
		InToggleTextOn->SetMaxHeight(MaxHeightIndentedText);
		InToggleTextOff->SetMaxHeight(MaxHeightIndentedText);
	}

	InBoxOn->AttachToComponent(InToggleTextOn, FAttachmentTransformRules::KeepRelativeTransform);
	InBoxOn->RegisterComponent();
	InBoxOn->SetRelativeLocation(Position_BoxCollision);
	InBoxOn->SetRelativeScale3D(Scale_BoxCollision_On);
	InBoxOn->SetBoxExtent(Extents_BoxCollision);
	InBoxOn->SetGenerateOverlapEvents(false);
	InBoxOn->SetCollisionProfileName(FName("BlockAll"));

	InBoxOff->AttachToComponent(InToggleTextOff, FAttachmentTransformRules::KeepRelativeTransform);
	InBoxOff->RegisterComponent();
	InBoxOff->SetRelativeLocation(Position_BoxCollision);
	InBoxOff->SetRelativeScale3D(Scale_BoxCollision_Off);
	InBoxOff->SetBoxExtent(Extents_BoxCollision);
	InBoxOff->SetGenerateOverlapEvents(false);
	InBoxOff->SetCollisionProfileName(FName("BlockAll"));
}

void AWallMenu::ToggleText(const bool bIsOn,
                           const UText3DComponent* InToggleTextOn,
                           const UText3DComponent* InToggleTextOff) const
{
	if (InToggleTextOn == ToggleText_NightMode_On && InToggleTextOff == ToggleText_NightMode_Off)
	{
		ToggleNightModeText(bIsOn);
		return;
	}

	SetDynamicMaterialEmissiveColor(InToggleTextOn, bIsOn ? FLinearColor::Green : FLinearColor::Red);
	SetDynamicMaterialEmissiveColor(InToggleTextOff, bIsOn ? FLinearColor::Red : FLinearColor::Green);
}

void AWallMenu::ToggleNightModeText(const bool bIsOn) const
{
	if (bIsWaitingOnTimeOfDayTransition)
	{
		return;
	}

	SetDynamicMaterialEmissiveColor(ToggleText_NightMode_On, bIsOn ? FLinearColor::Green : FLinearColor::Red);
	SetDynamicMaterialEmissiveColor(ToggleText_NightMode_Off, bIsOn ? FLinearColor::Red : FLinearColor::Green);
	SetDynamicMaterialPulseValue(ToggleText_NightMode_On, bIsOn ? 1.0f : 0.0f);
	SetDynamicMaterialPulseValue(ToggleText_NightMode_Off, bIsOn ? 0.0f : 1.0f);

	bIsWaitingOnTimeOfDayTransition = true;
}

void AWallMenu::SetNightModeTextVisibility(const bool bVisible) const
{
	MainText_Enable_NightMode->SetVisibility(bVisible);
	ToggleText_NightMode_On->SetVisibility(bVisible);
	ToggleText_NightMode_Off->SetVisibility(bVisible);
}

void AWallMenu::SetDynamicMaterialPulseValue(const UText3DComponent* InText3D, const float Value)
{
	if (InText3D)
	{
		if (UMaterialInstanceDynamic* Dynamic = Cast<UMaterialInstanceDynamic>(InText3D->GetFrontMaterial()))
		{
			Dynamic->SetScalarParameterValue(FName("bEnablePulse"), Value);
		}
	}
}

void AWallMenu::SetDynamicMaterialEmissiveColor(const UText3DComponent* InText3D, const FLinearColor& Color)
{
	if (InText3D)
	{
		if (UMaterialInstanceDynamic* Dynamic = Cast<UMaterialInstanceDynamic>(InText3D->GetFrontMaterial()))
		{
			Dynamic->SetVectorParameterValue(FName("EmissiveColor"), Color);
		}
	}
}
