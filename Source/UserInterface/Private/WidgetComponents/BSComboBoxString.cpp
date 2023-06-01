// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/BSComboBoxEntry.h"

void UBSComboBoxString::HandleSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectionType)
{
	CurrentOptionPtr = Item;
	SetSelectedOption(CurrentOptionPtr.IsValid() ? CurrentOptionPtr.ToSharedRef().Get() : FString());

	// When the selection changes we always generate another widget to represent the content area of the combobox.
	if ( ComboBoxContent.IsValid() )
	{
		DefaultComboBoxContent.Reset();
		ComboBoxContent->SetContent(HandleSelectionChangedGenerateWidget(Item));
	}

	if ( !IsDesignTime() )
	{
		OnSelectionChanged.Broadcast(Item.IsValid() ? *Item : FString(), SelectionType);
	}
}

TSharedRef<SWidget> UBSComboBoxString::HandleGenerateWidget(TSharedPtr<FString> Item) const
{
	FString StringItem = Item.IsValid() ? *Item : FString();

	// Call the user's delegate to see if they want to generate a custom widget bound to the data source.
	if (!IsDesignTime() && OnGenerateWidgetEventDelegate.IsBound())
	{
		UWidget* Widget = OnGenerateWidgetEventDelegate.Execute(this, StringItem);
		if ( Widget != NULL )
		{
			if(UBSComboBoxEntry* Entry = Cast<UBSComboBoxEntry>(Widget))
			{
				if (bHideTooltipImage)
				{
					Entry->ToggleTooltipImageVisibility(false);
				}
			}
			return Widget->TakeWidget();
		}
	}

	// If a row wasn't generated just create the default one, a simple text block of the item's name.
	return SNew(STextBlock)
		.Text(FText::FromString(StringItem))
		.Font(Font);
}

TSharedRef<SWidget> UBSComboBoxString::HandleSelectionChangedGenerateWidget(TSharedPtr<FString> Item) const
{
	FString StringItem = Item.IsValid() ? *Item : FString();
	// Call the user's delegate to see if they want to generate a custom widget bound to the data source.
	if (!IsDesignTime() && OnSelectionChangedGenerateWidgetEventDelegate.IsBound())
	{
		UWidget* Widget = OnSelectionChangedGenerateWidgetEventDelegate.Execute(this, StringItem);
		if ( Widget != NULL )
		{
			if(UBSComboBoxEntry* Entry = Cast<UBSComboBoxEntry>(Widget))
			{
				bHideTooltipImage = true;
				Entry->ToggleTooltipImageVisibility(false);
			}
			return Widget->TakeWidget();
		}
	}

	// If a row wasn't generated just create the default one, a simple text block of the item's name.
	return SNew(STextBlock)
		.Text(FText::FromString(StringItem))
		.Font(Font);
}

void UBSComboBoxString::SetSelectedOptions(TArray<FString> InOptions)
{
	for (const FString& String : InOptions)
	{
		AddSelectedIndex(FindOptionIndex(String));
	}
}

void UBSComboBoxString::SetSelectedIndices(const TArray<int32> InIndices)
{
	for (const int32& Index : InIndices)
	{
		if (Options.IsValidIndex(Index))
		{
			FString TempSelectedOption = *Options[Index];
			CurrentOptionPointers.AddUnique(Options[Index]);
			if (!SelectedOptions.Contains(TempSelectedOption))
			{
				SelectedOptions.AddUnique(*Options[Index]);

				if (ComboBoxContent.IsValid())
				{
					MyComboBox->SetSelectedItem(CurrentOptionPtr);
					UpdateOrGenerateWidget(CurrentOptionPtr);
				}		
				else
				{
					HandleSelectionChanged(CurrentOptionPtr, ESelectInfo::Direct);
				}
			}
		}
	}
}

TArray<FString> UBSComboBoxString::GetSelectedOptions() const
{
	TArray<FString> ReturnArray = TArray<FString>();
	for (const TSharedPtr<FString>& OptionPointer : CurrentOptionPointers)
	{
		if (OptionPointer.IsValid())
		{
			ReturnArray.AddUnique(*OptionPointer);
		}
	}
	return ReturnArray;
}

TArray<int32> UBSComboBoxString::GetSelectedIndices() const
{
	TArray<int32> ReturnArray = TArray<int32>();
	for (const TSharedPtr<FString>& OptionPointer : CurrentOptionPointers)
	{
		if (OptionPointer.IsValid())
		{
			for (int32 OptionIndex = 0; OptionIndex < Options.Num(); ++OptionIndex)
			{
				if (Options[OptionIndex] == OptionPointer)
				{
					ReturnArray.AddUnique(OptionIndex);
				}
			}
		}
	}
	return ReturnArray;
}

void UBSComboBoxString::AddSelectedIndex(const int32 InIndex)
{
	if (Options.IsValidIndex(InIndex))
	{
		CurrentOptionPtr = Options[InIndex];
		// Don't select item if its already selected
		if (!SelectedOptions.Contains(*CurrentOptionPtr))
		{
			SelectedOptions.AddUnique(*CurrentOptionPtr);

			if (ComboBoxContent.IsValid())
			{
				MyComboBox->SetSelectedItem(CurrentOptionPtr);
				UpdateOrGenerateWidget(CurrentOptionPtr);
			}		
			else
			{
				HandleSelectionChanged(CurrentOptionPtr, ESelectInfo::Direct);
			}
		}
	}
}

void UBSComboBoxString::AddSelectedOption(const FString InOption)
{
	AddSelectedIndex(FindOptionIndex(InOption));
}