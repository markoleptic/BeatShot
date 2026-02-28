// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequestHelpers.generated.h"

/** Executed when a response is received from a request. */
DECLARE_DELEGATE(FOnHttpResponseReceived);

/** Base struct for Http requests in this class. */
USTRUCT()
struct FBSHttpResponse
{
	GENERATED_BODY()

	int32 HttpStatus;
	bool bConnectedSuccessfully;
	bool OK;
	FOnHttpResponseReceived OnHttpResponseReceived;

	FBSHttpResponse() : HttpStatus(0), bConnectedSuccessfully(false), OK(false)
	{
	}
};

/** Used to create a feedback Json object. */
USTRUCT()
struct FJsonFeedback
{
	GENERATED_BODY()

	UPROPERTY()
	FString Title;

	UPROPERTY()
	FString Content;

	FJsonFeedback()
	{
		Title = FString();
		Content = FString();
	}

	FJsonFeedback(const FString& InTitle, const FString& InContent)
	{
		Title = InTitle;
		Content = InContent;
	}
};

namespace HttpRequestHelpers
{
/** Makes a POST request to BeatShot website which emails the feedback. Executes OnHttpResponseReceived supplied by
 *  FeedbackResponse .
 *
 *  @param InFeedback struct to send with the request
 *  @param FeedbackResponse struct containing callback delegate and response info
 */
void BEATSHOTGLOBAL_API PostFeedback(const FJsonFeedback InFeedback,
                                     TSharedPtr<FBSHttpResponse, ESPMode::ThreadSafe> FeedbackResponse);
}
