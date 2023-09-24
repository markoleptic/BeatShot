// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/FeedbackWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/Buttons/BSButton.h"

void UFeedbackWidget::ShowFeedbackWidget()
{
	SetVisibility(ESlateVisibility::Visible);
	PlayFadeIn();
}

void UFeedbackWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_SubmitFeedback->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Back->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Okay->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	
	Button_BugReport->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Feedback->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_BugReport->SetDefaults(0, Button_Feedback);
	Button_Feedback->SetDefaults(1, Button_BugReport);
	
	Value_Title->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_Title);
	Value_Content->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_Content);
	
	OnPostFeedbackResponseDelegate.BindUObject(this, &ThisClass::OnPostFeedbackResponse);

	Button_SubmitFeedback->SetIsEnabled(false);
	Value_Title->SetIsReadOnly(true);
	Value_Content->SetIsReadOnly(true);
}

void UFeedbackWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	if (Button == Button_Feedback)
	{
		TitlePrefix = BugReportTitle;
		Value_Title->SetIsReadOnly(false);
		Value_Content->SetIsReadOnly(false);
	}
	else if (Button == Button_BugReport)
	{
		TitlePrefix = FeedbackTitle;
		Value_Title->SetIsReadOnly(false);
		Value_Content->SetIsReadOnly(false);
	}
	else if (Button == Button_SubmitFeedback)
	{
		const FJsonFeedback Feedback(TitlePrefix.ToString() + Value_Title->GetText().ToString(), Value_Content->GetText().ToString());
		PostFeedback(Feedback, OnPostFeedbackResponseDelegate);
		PlayFadeOut();
	}
	else if (Button == Button_Okay)
	{
		Value_Title->SetText(FText::GetEmpty());
		Value_Content->SetText(FText::GetEmpty());
		Button_SubmitFeedback->SetIsEnabled(false);
		FadeOutResponseDelegate.BindDynamic(this, &ThisClass::SetCollapsedAndUnbindDelegates);
		BindToAnimationFinished(FadeOutResponse, FadeOutResponseDelegate);
		PlayFadeOutResponse();
	}
	else if (Button == Button_Back)
	{
		FadeOutDelegate.BindDynamic(this, &ThisClass::SetCollapsedAndUnbindDelegates);
		BindToAnimationFinished(FadeOut, FadeOutDelegate);
		PlayFadeOut();
	}
}

void UFeedbackWidget::OnPostFeedbackResponse(const bool bSuccess)
{
	if (bSuccess)
	{
		TextBlock_FeedbackResponseTitle->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "FeedbackResponseSuccessTitle"));
		TextBlock_FeedbackResponseInfo->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "FeedbackResponseSuccessInfo"));
	}
	else
	{
		TextBlock_FeedbackResponseTitle->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "FeedbackResponseFailureTitle"));
		TextBlock_FeedbackResponseInfo->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "FeedbackResponseFailureInfo"));
	}
	PlayFadeInResponse();
}

void UFeedbackWidget::OnTextCommitted_Title(const FText& NewTitle, ETextCommit::Type CommitType)
{
}

void UFeedbackWidget::OnTextCommitted_Content(const FText& NewContent, ETextCommit::Type CommitType)
{
	if (!NewContent.IsEmptyOrWhitespace())
	{
		Button_SubmitFeedback->SetIsEnabled(true);
	}
	else
	{
		Button_SubmitFeedback->SetIsEnabled(false);
	}
}

void UFeedbackWidget::SetCollapsedAndUnbindDelegates()
{
	if (FadeOutDelegate.IsBound())
	{
		UnbindFromAnimationFinished(FadeOut, FadeOutDelegate);
		FadeOutDelegate.Unbind();
	}
	if (FadeOutResponseDelegate.IsBound())
	{
		UnbindFromAnimationFinished(FadeOutResponse, FadeOutResponseDelegate);
		FadeOutResponseDelegate.Unbind();
	}
	OnExitAnimationCompletedDelegate.Broadcast();
	SetVisibility(ESlateVisibility::Collapsed);
}
