// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/BSComboBoxEntry.h"
#include "WidgetComponents/SBSComboBox.h"
#include "Widgets/SNullWidget.h"
#include "UObject/EditorObjectVersion.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Styling/UMGCoreStyle.h"

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// UComboBoxString

static FComboBoxStyle* DefaultComboBoxStyle = nullptr;
static FTableRowStyle* DefaultComboBoxRowStyle = nullptr;

#if WITH_EDITOR
static FComboBoxStyle* EditorComboBoxStyle = nullptr;
static FTableRowStyle* EditorComboBoxRowStyle = nullptr;
#endif 


UBSComboBoxString::UBSComboBoxString()
{
	if (DefaultComboBoxStyle == nullptr)
	{
		DefaultComboBoxStyle = new FComboBoxStyle(FUMGCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox"));

		// Unlink UMG default colors.
		DefaultComboBoxStyle->UnlinkColors();
	}

	if (DefaultComboBoxRowStyle == nullptr)
	{
		DefaultComboBoxRowStyle = new FTableRowStyle(FUMGCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));

		// Unlink UMG default colors.
		DefaultComboBoxRowStyle->UnlinkColors();
	}

	WidgetStyle = *DefaultComboBoxStyle;
	ItemStyle = *DefaultComboBoxRowStyle;

#if WITH_EDITOR 
	if (EditorComboBoxStyle == nullptr)
	{
		EditorComboBoxStyle = new FComboBoxStyle(FCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("EditorUtilityComboBox"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorComboBoxStyle->UnlinkColors();
	}

	if (EditorComboBoxRowStyle == nullptr)
	{
		EditorComboBoxRowStyle = new FTableRowStyle(FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorComboBoxRowStyle->UnlinkColors();
	}

	if (IsEditorWidget())
	{
		WidgetStyle = *EditorComboBoxStyle;
		ItemStyle = *EditorComboBoxRowStyle;

		// The CDO isn't an editor widget and thus won't use the editor style, call post edit change to mark difference from CDO
		PostEditChange();
	}
#endif // WITH_EDITOR

	WidgetStyle.UnlinkColors();
	ItemStyle.UnlinkColors();

	ForegroundColor = ItemStyle.TextColor;
	bIsFocusable = true;

	ContentPadding = FMargin(4.0, 2.0);
	MaxListHeight = 450.0f;
	HasDownArrow = true;
	EnableGamepadNavigationMode = true;
	// We don't want to try and load fonts on the server.
	if ( !IsRunningDedicatedServer() )
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
		Font = FSlateFontInfo(RobotoFontObj.Object, 16, FName("Bold"));
	}
}

TSharedRef<SWidget> UBSComboBoxString::RebuildWidget()
{
	int32 InitialIndex = FindOptionIndex(GetSelectedOption());
	
	TSharedPtr<FString> CurrentOptionPtr;
	
	if ( InitialIndex != -1 )
	{
		CurrentOptionPtr = Options[InitialIndex];
	}
	
	MyComboBox =
		SNew(SBSComboBox<TSharedPtr<FString>>)
		.ComboBoxStyle(&WidgetStyle)
		.ItemStyle(&ItemStyle)
		.ForegroundColor(ForegroundColor)
		.OptionsSource(&Options)
		.InitiallySelectedItems(CurrentlySelectedOptionPointers)
		.ContentPadding(ContentPadding)
		.MaxListHeight(MaxListHeight)
		.HasDownArrow(HasDownArrow)
		.EnableGamepadNavigationMode(EnableGamepadNavigationMode)
		.OnGenerateWidget(BIND_UOBJECT_DELEGATE(SBSComboBox<TSharedPtr<FString>>::FOnGenerateWidget, HandleGenerateWidget))
		.OnMultiSelectionChanged(BIND_UOBJECT_DELEGATE(FOnMultiSelectionChanged, HandleSelectionChanged))
		.OnComboBoxOpening(BIND_UOBJECT_DELEGATE(FOnComboBoxOpening, HandleOpening))
		.IsFocusable(bIsFocusable)
		.CloseComboBoxOnSelectionChanged(bCloseComboBoxOnSelectionChanged)
		.SelectionMode(GetSelectionModeType(SelectionMode))
		.MaxNumSelectedItems(MaxNumberOfSelections)
		.CanSelectNone(bCanSelectNone)
		[
			SAssignNew(ComboBoxContent, SBox)
		];

	if ( InitialIndex != -1 )
	{
		// Generate the widget for the initially selected widget if needed
		UpdateOrGenerateWidget(CurrentOptionPtr);
	}

	return MyComboBox.ToSharedRef();
}

void UBSComboBoxString::HandleSelectionChanged(const TArray<TSharedPtr<FString>>& Items, const ESelectInfo::Type SelectionType)
{
	CurrentlySelectedOptionPointers = Items;

	// When the selection changes we always generate another widget to represent the content area of the combobox.
	if (ComboBoxContent && MyComboBox)
	{
		ComboBoxContent->SetContent(HandleMultiSelectionChangedGenerateWidget(CurrentlySelectedOptionPointers));
	}
	
	if (!IsDesignTime())
	{
		TArray<FString> Temp;
		for (const TSharedPtr<FString>& Item : Items)
		{
			if (Item != nullptr)
			{
				Temp.Add(*Item);
				// UE_LOG(LogTemp, Display, TEXT("Item: %s"), **Item.Get());
			}
		}
		OnSelectionChanged.Broadcast(Temp, SelectionType);
	}
	// UE_LOG(LogTemp, Display, TEXT("GetOptionCount %d"), GetOptionCount());
	// UE_LOG(LogTemp, Display, TEXT("GetSelectedOptionCount %d"), GetSelectedOptionCount());
}

void UBSComboBoxString::AddOption(const FString& Option)
{
	Options.Add(MakeShareable(new FString(Option)));
	RefreshOptions();
}

bool UBSComboBoxString::RemoveOption(const FString& Option)
{
	const int32 OptionIndex = FindOptionIndex(Option);
	if ( OptionIndex != -1 )
	{
		if ( CurrentlySelectedOptionPointers.Contains(Options[OptionIndex]))
		{
			ClearSelection();
		}
		Options.RemoveAt(OptionIndex);
		RefreshOptions();
		return true;
	}
	return false;
}

int32 UBSComboBoxString::FindOptionIndex(const FString& Option) const
{
	for ( int32 OptionIndex = 0; OptionIndex < Options.Num(); OptionIndex++ )
	{
		const TSharedPtr<FString>& OptionAtIndex = Options[OptionIndex];

		if ( ( *OptionAtIndex ) == Option )
		{
			return OptionIndex;
		}
	}
	
	return -1;
}

FString UBSComboBoxString::GetOptionAtIndex(int32 Index) const
{
	if (Index >= 0 && Index < Options.Num())
	{
		return *(Options[Index]);
	}
	return FString();
}

void UBSComboBoxString::ClearOptions()
{
	ClearSelection();
	Options.Empty();
	if ( MyComboBox.IsValid() )
	{
		MyComboBox->RefreshOptions();
	}
}

void UBSComboBoxString::ClearSelection()
{
	for (TSharedPtr<FString> String : CurrentlySelectedOptionPointers)
	{
		String.Reset();
	}
	if ( MyComboBox.IsValid() )
	{
		MyComboBox->ClearSelection();
	}

	if ( ComboBoxContent.IsValid() )
	{
		ComboBoxContent->SetContent(SNullWidget::NullWidget);
	}
}

void UBSComboBoxString::RefreshOptions()
{
	if ( MyComboBox.IsValid() )
	{
		MyComboBox->RefreshOptions();
	}
}

FString UBSComboBoxString::GetSelectedOption() const
{
	if (GetSelectedOptionCount() > 0 && CurrentlySelectedOptionPointers[0].IsValid())
	{
		return *CurrentlySelectedOptionPointers[0];
	}
	return FString();
}

int32 UBSComboBoxString::GetSelectedIndex() const
{
	if (GetSelectedOptionCount() > 0)
	{
		for (int32 OptionIndex = 0; OptionIndex < Options.Num(); ++OptionIndex)
		{
			if (CurrentlySelectedOptionPointers.IsValidIndex(OptionIndex))
			{
				return OptionIndex;
			}
		}
	}

	return -1;
}

int32 UBSComboBoxString::GetOptionCount() const
{
	return Options.Num();
}

int32 UBSComboBoxString::GetSelectedOptionCount() const
{
	return CurrentlySelectedOptionPointers.Num();
}

bool UBSComboBoxString::IsOpen() const
{
	return MyComboBox.IsValid() && MyComboBox->IsOpen();
}

void UBSComboBoxString::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyComboBox.Reset();
	ComboBoxContent.Reset();
}

void UBSComboBoxString::PostInitProperties()
{
	Super::PostInitProperties();

	// Initialize the set of options from the default set only once.
	for (const FString& DefaultOption : DefaultOptions)
	{
		AddOption(DefaultOption);
	}
}

void UBSComboBoxString::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FEditorObjectVersion::GUID);
}

void UBSComboBoxString::PostLoad()
{
	Super::PostLoad();

	// Initialize the set of options from the default set only once.
	for (const FString& DefaultOption : DefaultOptions)
	{
		AddOption(DefaultOption);
	}

	if (GetLinkerCustomVersion(FEditorObjectVersion::GUID) < FEditorObjectVersion::ComboBoxControllerSupportUpdate)
	{
		EnableGamepadNavigationMode = false;
	}
}

const FText UBSComboBoxString::GetPaletteCategory()
{
	return Super::GetPaletteCategory();
}

void UBSComboBoxString::UpdateOrGenerateWidget(TSharedPtr<FString> Item)
{
	ComboBoxContent->SetContent(HandleGenerateWidget(Item));
}

void UBSComboBoxString::HandleOpening()
{
	OnOpening.Broadcast();
}

TSharedRef<SWidget> UBSComboBoxString::HandleGenerateWidget(TSharedPtr<FString> Item) const
{
	const FString StringItem = Item.IsValid() ? *Item : FString();

	// Call the user's delegate to see if they want to generate a custom widget bound to the data source.
	if (!IsDesignTime() && OnGenerateWidgetEventDelegate.IsBound())
	{
		if (UWidget* Widget = OnGenerateWidgetEventDelegate.Execute(this, StringItem))
		{
			if (const UBSComboBoxEntry* Entry = Cast<UBSComboBoxEntry>(Widget))
			{
				Entry->ToggleTooltipImageVisibility(true);
			}
			return Widget->TakeWidget();
		}
	}

	// If a row wasn't generated just create the default one, a simple text block of the item's name.
	return SNew(STextBlock)
		.Text(FText::FromString(StringItem))
		.Font(Font);
}

TSharedRef<SWidget> UBSComboBoxString::HandleMultiSelectionChangedGenerateWidget(TConstArrayView<SBSComboBox<TSharedPtr<FString>>::NullableOptionType> Items) const
{
	// Call the user's delegate to see if they want to generate a custom widget bound to the data source.
	if (!IsDesignTime() && OnSelectionChanged_GenerateWidgetForMultiSelection.IsBound())
	{
		TArray<FString> Copy;
		for (const TSharedPtr<FString>& Item : Items)
		{
			if (Item.Get())
			{
				Copy.Add(*Item);
			}
		}
		if (UWidget* Widget = OnSelectionChanged_GenerateWidgetForMultiSelection.Execute(this, Copy))
		{
			if (const UBSComboBoxEntry* Entry = Cast<UBSComboBoxEntry>(Widget))
			{
				Entry->ToggleTooltipImageVisibility(false);
			}
			return Widget->TakeWidget();
		}
	}
	FString DefaultListOfItems;
	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i].Get())
		{
			DefaultListOfItems.Append(*Items[i]);
			if (i < Items.Num() - 1)
			{
				DefaultListOfItems.Append(", ");
			}
		}
	}
	// If a row wasn't generated just create the default one, a simple text block of the item's name.
	return SNew(STextBlock)
		.Text(FText::FromString(DefaultListOfItems))
		.Font(Font);
}

void UBSComboBoxString::SetSelectedOptions(TArray<FString> InOptions)
{
	if (ComboBoxContent.IsValid())
	{
		CurrentlySelectedOptionPointers.Empty();
		for (const FString& String : InOptions)
		{
			const int32 InIndex = FindOptionIndex(String);
			if (!Options.IsValidIndex(InIndex))
			{
				continue;
			}
			if (!CurrentlySelectedOptionPointers.Contains(Options[InIndex]))
			{
				CurrentlySelectedOptionPointers.Add(Options[InIndex]);
			}
		}
		ComboBoxContent->SetContent(HandleMultiSelectionChangedGenerateWidget(CurrentlySelectedOptionPointers));
		if (MyComboBox)
		{
			MyComboBox->SetItemSelection(CurrentlySelectedOptionPointers, true);
		}
	}
	else
	{
		HandleSelectionChanged(Options, ESelectInfo::Direct);
	}
}

void UBSComboBoxString::SetSelectedIndices(const TArray<int32> InIndices)
{
	for (const int32& Index : InIndices)
	{
		SetSelectedIndex(Index);
	}
}

TArray<FString> UBSComboBoxString::GetSelectedOptions() const
{
	TArray<FString> ReturnArray = TArray<FString>();
	for (const TSharedPtr<FString>& OptionPointer : CurrentlySelectedOptionPointers)
	{
		if (OptionPointer.Get())
		{
			ReturnArray.AddUnique(*OptionPointer);
		}
	}
	return ReturnArray;
}

TArray<int32> UBSComboBoxString::GetSelectedIndices() const
{
	TArray<int32> ReturnArray = TArray<int32>();
	for (const TSharedPtr<FString>& OptionPointer : CurrentlySelectedOptionPointers)
	{
		if (OptionPointer.Get())
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

void UBSComboBoxString::SetSelectedIndex(const int32 InIndex)
{
	if (Options.IsValidIndex(InIndex))
	{
		if (ComboBoxContent.IsValid())
		{

			if (!CurrentlySelectedOptionPointers.Contains(Options[InIndex]))
			{
				CurrentlySelectedOptionPointers.Add(Options[InIndex]);
			}
			ComboBoxContent->SetContent(HandleMultiSelectionChangedGenerateWidget(CurrentlySelectedOptionPointers));
			if (MyComboBox)
			{
				MyComboBox->SetItemSelection(Options[InIndex], true);
			}
		}
		else
		{
			HandleSelectionChanged(Options, ESelectInfo::Direct);
		}
	}
}

void UBSComboBoxString::SetSelectedOption(const FString InOption)
{
	SetSelectedIndex(FindOptionIndex(InOption));
}

#undef LOCTEXT_NAMESPACE