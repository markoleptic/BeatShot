// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Utilities/HttpRequestHelpers.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
#include "SaveGames/SaveGamePlayerScore.h"

namespace HttpRequestHelpers
{
	void PostFeedback(const FJsonFeedback InFeedback, TSharedPtr<FBSHttpResponse, ESPMode::ThreadSafe> FeedbackResponse)
	{
		FString ContentString;
		const TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		FJsonObjectConverter::UStructToJsonObject(FJsonFeedback::StaticStruct(), &InFeedback, JsonObject);
		const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&ContentString);
		FJsonSerializer::Serialize(JsonObject, JsonWriter);

		const FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetURL(Constants::Endpoint_SendFeedback);
		HttpRequest->SetTimeout(5.f);
		HttpRequest->SetVerb("POST");
		HttpRequest->SetHeader("Content-Type", "application/json");
		HttpRequest->SetContentAsString(ContentString);
		HttpRequest->OnProcessRequestComplete().BindLambda(
			[FeedbackResponse](FHttpRequestPtr, const FHttpResponsePtr Response, const bool bConnectedSuccessfully)
			{
				check(FeedbackResponse.IsValid());
				FeedbackResponse->bConnectedSuccessfully = bConnectedSuccessfully;
				if (!bConnectedSuccessfully || !Response.IsValid())
				{
					FeedbackResponse->HttpStatus = 502;
					UE_LOG(LogTemp, Warning, TEXT("Failed to connect to send feedback."));
				}
				else
				{
					FeedbackResponse->HttpStatus = Response->GetResponseCode();
					if (FeedbackResponse->HttpStatus >= 200 && FeedbackResponse->HttpStatus <= 300)
					{
						FeedbackResponse->OK = true;
						UE_LOG(LogTemp, Warning, TEXT("Successfully sent feedback."));
					}
					else
					{
						UE_LOG(LogTemp,
						       Warning,
						       TEXT("Failed to send feedback Http Status: %d"),
						       FeedbackResponse->HttpStatus);
					}
				}
				if (FeedbackResponse->OnHttpResponseReceived.IsBound())
				{
					FeedbackResponse->OnHttpResponseReceived.Execute();
				}
			});
		HttpRequest->ProcessRequest();
	}
}
