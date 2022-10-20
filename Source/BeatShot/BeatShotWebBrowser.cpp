// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatShotWebBrowser.h"
#include "WebBrowser.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "WebBrowserModule.h"
#include "Components/PanelWidget.h"
#include "ContentBrowserMenuContexts.h"
#include "SWebBrowser.h"


void UBeatShotWebBrowser::SynchronizeProperties()
{
}

void UBeatShotWebBrowser::SetCookie()
{
	IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
	TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager();
	WebBrowserSingleton->SetDevToolsShortcutEnabled(true);
	if (CookieManager.IsValid())
	{
		IWebBrowserCookieManager::FCookie CookieVariable;
		CookieVariable.Name = "jwt";
		CookieVariable.Value = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Im1hcmsiLCJpYXQiOjE2NjYyMDU4NDUsImV4cCI6MTY2ODc5Nzg0NX0.YxBhMgjcTzwEdpSTOaQ7JbmlCpkjKRLbc6ImFhURMZY";
		CookieVariable.bHasExpires = true;
		CookieVariable.bHttpOnly = true;
		CookieVariable.bSecure = true;
		CookieVariable.Domain = "localhost";
		CookieVariable.Path = "/";
		CookieManager->SetCookie("http://localhost:3000", CookieVariable);
	}

	FBrowserContextSettings Settings = FBrowserContextSettings("hi");
	Settings.bPersistSessionCookies = true;
	WebBrowserSingleton->RegisterContext(Settings);
	
}
