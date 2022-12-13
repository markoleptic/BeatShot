// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WebBrowser.h"
#include "Delegates/DelegateCombinations.h"
#include "WebBrowserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnURLLoaded, bool, bLoadedSuccessfully);

/**
 * 
 */
UCLASS()
class BEATSHOT_API UWebBrowserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/* The WebBrowser widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UWebBrowser* Browser;
	
	/* Load Custom GameModes for the user */
	UFUNCTION(BlueprintCallable)
	void LoadCustomGameModesURL(const FString& Username);
	
	/* Load Default GameModes for the user */
	UFUNCTION(BlueprintCallable)
	void LoadDefaultGameModesURL(const FString& Username);
	
	/* Load Patch Notes for any user */
	UFUNCTION(BlueprintCallable)
	void LoadPatchNotesURL() const;

	/* Profile for a user */
	UFUNCTION(BlueprintCallable)
	void LoadProfileURL(const FString& Username);

	/* Executes Javascript to login a user */
	UFUNCTION(BlueprintCallable)
	void HandleUserLogin(const FString& Username,const FString& Email,const FString& Password);

	/* Allows other widgets to bind to loading of URLs */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnURLLoaded OnURLLoaded;

private:

	/* Fills the login form on the login screen of the website using Javascript */
	bool FillLoginForm(const FString& Username,const FString& Email,const FString& Password) const;

	/* Check the persistant checkbox on the login screen of the website using Javascript */
	void CheckPersistCheckbox();

	/* Check the login button on the login screen of the website using Javascript */
	void ClickLogin();

	/* Compares the browser's current URL address against the UserProfileURL */
	void OnURLChanged(const FString& LastURL);

	/* Callback for After the delay inside OnURLChanged has completed, which recursively calls OnURLChanged */
	UFUNCTION()
	void OnURLChangedCallback();

	/* Timer to recursively call OnURLChanged at constant intervals */
	FTimerHandle URLCheckDelay;

	/* Timer to recursively call OnURLChanged at constant intervals */
	FTimerHandle CheckCheckboxDelay;

	/* Timer to recursively call OnURLChanged at constant intervals */
	FTimerHandle ClickLoginDelay;

	/* The user's profile URL to check the current browser URL against */
	FString UserProfileURL;

	/* The number of OnURLChanged function calls, or number of profile URL checks */
	int32 URLCheckAttempts = 0;

#pragma region URLs
	
	/* Patch Notes URL */
	const FString PatchNotesURL = "https://beatshot.gg/patchnotes";
	
	/* Base profile URL */
	const FString ProfileURL = "https://beatshot.gg/profile/";

	/* Default GameMode end URL */
	const FString DefaultModesString = "/stats/defaultmodes";

	/* Custom GameMode end URL */
	const FString CustomModesString = "/stats/custommodes";

#pragma endregion

#pragma region SCRIPTS
	
	/* Sets up the nativeInputValueSetter so the browser recognizes the changes */
	const FString InitialInputEventScript = "var nativeInputValueSetter = Object.getOwnPropertyDescriptor(window.HTMLInputElement.prototype, 'value').set; var event = new Event('input', { bubbles: true});";

	/* Javascript to check the persist checkbox */
	const FString CheckPersistScript = "document.getElementById('persist').checked=true; document.getElementById('persist').click();";
	
	/* Javascript to click the login button */
	const FString ClickLoginScript = "document.getElementById('login-button').click()";

	/* Sets the username form element text */
	const FString SetElementUsernameScript = "nativeInputValueSetter.call(username, '";

	/* Sets the email form element text */
	const FString SetElementEmailScript = "nativeInputValueSetter.call(email, '";

	/* Sets the password form element text */
	const FString SetElementPasswordScript = "nativeInputValueSetter.call(password, '";

	/* Dispatches the username so the browser recognizes the change */
	const FString DispatchUsernameChangeEventScript = "username.dispatchEvent(event);";

	/* Dispatches the email so the browser recognizes the change */
	const FString DispatchEmailChangeEventScript = "email.dispatchEvent(event);";

	/* Dispatches the password so the browser recognizes the change */
	const FString DispatchPasswordChangeEventScript = "password.dispatchEvent(event);";

#pragma endregion
};
