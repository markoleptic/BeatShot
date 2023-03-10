// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGamePlayerSettings.h"
#include "WebBrowserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnURLLoaded, bool, bLoadedSuccessfully);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTimerElapsed, const FString&, LastURL);

class UWebBrowser;

UCLASS()
class USERINTERFACE_API UWebBrowserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

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
	void HandleUserLogin(const FLoginPayload LoginPayload);

	/* Allows other widgets to bind to loading of URLs */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnURLLoaded OnURLLoaded;

private:
	/* Fills the login form on the login screen of the website using Javascript */
	bool FillLoginForm(const FLoginPayload LoginPayload) const;

	/* Compares the browser's current URL address against the UserProfileURL */
	UFUNCTION()
	void OnURLChanged();

	FTimerDelegate CheckboxDelegate;

	FTimerDelegate ClickLoginDelegate;

	FTimerDelegate CheckURLDelegate;

	/* Timer to delay clicking the Checkbox on the login page */
	FTimerHandle CheckCheckboxDelay;

	/* Timer to delay clicking the login button on the login page */
	FTimerHandle ClickLoginDelay;

	/* Timer to delay URL address comparisons */
	FTimerHandle CheckURLTimer;

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
	const FString InitialInputEventScript =
		"var nativeInputValueSetter = Object.getOwnPropertyDescriptor(window.HTMLInputElement.prototype, 'value').set; var event = new Event('input', { bubbles: true});";

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
