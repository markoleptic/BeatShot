// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "LoginWidget.generated.h"

class UOverlay;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExitLogin);
UCLASS()
class BEATSHOT_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UHorizontalBox* DefaultSignInText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UHorizontalBox* HasSignedInBeforeText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UOverlay* RegisterOverlay;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UOverlay* ContinueWithoutOverlay;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UOverlay* LoginOverlay;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UOverlay* LoggedInOverlay;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FExitLogin ExitLogin;
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* FadeInAll;
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* FadeOutAll;
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* FadeInAllInitial;
private:
	FWidgetAnimationDynamicEvent EndDelegate;
	UFUNCTION()
		void FadeOutAnimFinished();
};
