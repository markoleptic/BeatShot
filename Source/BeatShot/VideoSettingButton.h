#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Delegates/DelegateCombinations.h"

#include "VideoSettingButton.generated.h"

UENUM()
enum class ESettingType : uint8
{
	AntiAliasing UMETA(DisplayName, "AntiAliasing"),
	GlobalIllumination UMETA(DisplayName, "GlobalIllumination"),
	PostProcessing UMETA(DisplayName, "PostProcessing"),
	Reflection UMETA(DisplayName, "Reflection"),
	Shadow UMETA(DisplayName, "Shadow"),
	Shading UMETA(DisplayName, "Shading"),
	Texture UMETA(DisplayName, "Texture"),
	ViewDistance UMETA(DisplayName, "ViewDistance"),
	VisualEffect UMETA(DisplayName, "VisualEffect")
};

ENUM_RANGE_BY_FIRST_AND_LAST(ESettingType, ESettingType::AntiAliasing, ESettingType::VisualEffect);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonClicked, UVideoSettingButton*, VideoSettingButton);

UCLASS()
class BEATSHOT_API UVideoSettingButton : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnButtonClickedCallback();
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button;
	UPROPERTY(EditDefaultsOnly)
	int32 Quality;
	UPROPERTY(EditDefaultsOnly)
	ESettingType SettingType;
	UPROPERTY(EditDefaultsOnly)
	UVideoSettingButton* Next;

	FOnButtonClicked OnButtonClicked;
};
