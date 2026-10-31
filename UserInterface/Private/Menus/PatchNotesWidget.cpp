// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "Menus/PatchNotesWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "Styles/PatchNotesStyle.h"

DEFINE_LOG_CATEGORY(LogPatchNotes);


void UPatchNotesWidget::InitializeNativeClassData()
{
	Super::InitializeNativeClassData();

	if (WidgetTree == nullptr)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"), RF_Transient);
	}

	const FString KeyPrefix = PatchKey + TEXT("_");
	FText DateText;
	TArray<FPatchNotesSection> Sections;
	const UStringTable* StringTable = LoadObject<UStringTable>(nullptr, *StringTablePath);
	if (StringTable == nullptr)
	{
		UE_LOG(LogPatchNotes, Warning,
		       TEXT("PatchNotesWidget: could not load string table at '%s'; the widget will be blank."),
		       *StringTablePath);
		return;
	}

	CollectPatchNotes(StringTable, StringTablePath, KeyPrefix, DateText, Sections);
	if (Sections.Num() == 0)
	{
		UE_LOG(LogPatchNotes, Warning,
		       TEXT("PatchNotesWidget: no entries found with key prefix '%s' in '%s'; the widget will be blank."),
		       *KeyPrefix, *StringTablePath);
		return;
	}

	UWidgetTree* Tree = WidgetTree;
	const FPatchNotesStyle Style = StyleAsset ? StyleAsset->Values : FPatchNotesStyle();
	UVerticalBox* Root = Tree->ConstructWidget<UVerticalBox>();
	AddHeader(Tree, Root, DateText, Style);
	for (const FPatchNotesSection& Section : Sections)
	{
		AddSection(Tree, Root, Section, Style);
	}
	WidgetTree->RootWidget = Root;
}

namespace
{
/** Parses a non-negative decimal key index; every character must be a digit. */
bool ParseKeyIndex(const FString& Part, int32& OutValue)
{
	if (Part.IsEmpty())
	{
		return false;
	}
	for (const TCHAR Character : Part)
	{
		if (Character < TEXT('0') || Character > TEXT('9'))
		{
			return false;
		}
	}
	OutValue = FCString::Atoi(*Part);
	return true;
}
}

void UPatchNotesWidget::CollectPatchNotes(const UStringTable* StringTable,
                                          const FString& TableId,
                                          const FString& KeyPrefix,
                                          FText& OutDateText,
                                          TArray<FPatchNotesSection>& OutSections)
{
	const FName TableIdent = FName(*TableId);
	TMap<int32, FString> SectionTitles;
	TMap<int32, TMap<int32, FString>> SectionBullets;
	TMap<int32, TMap<int32, TMap<int32, FString>>> SectionSubBullets;

	StringTable->GetStringTable()->EnumerateKeysAndSourceStrings([&](const FTextKey& Key, const FString& SourceString)
	{
		const FString KeyString = Key.ToString();
		if (!KeyString.StartsWith(KeyPrefix))
		{
			return true;
		}

		if (KeyString == KeyPrefix + TEXT("Date"))
		{
			OutDateText = FText::FromStringTable(TableIdent, FTextKey(KeyString));
			return true;
		}

		// <Prefix>_<Section>[_<Bullet>[_<Sub>]]
		TArray<FString> KeyParts;
		const FString RemainingParts = KeyString.Mid(KeyPrefix.Len());
		if (!RemainingParts.ParseIntoArray(KeyParts, TEXT("_"), true) || KeyParts.Num() > 3)
		{
			return true;
		}

		TArray<int32> Values;
		for (const FString& Part : KeyParts)
		{
			int32 Value;
			if (!ParseKeyIndex(Part, Value))
			{
				Values.Reset();
				break;
			}
			Values.Add(Value);
		}

		switch (Values.Num())
		{
		case 1:
			SectionTitles.Add(Values[0], SourceString);
			break;
		case 2:
			SectionBullets.FindOrAdd(Values[0]).Add(Values[1], SourceString);
			break;
		case 3:
			SectionSubBullets.FindOrAdd(Values[0]).FindOrAdd(Values[1]).Add(Values[2], SourceString);
			break;
		default:
			break;
		}
		return true;
	});

	TArray<int32> SectionIndices;
	SectionTitles.GetKeys(SectionIndices);
	SectionIndices.Sort();
	for (const int32 SectionIndex : SectionIndices)
	{
		FPatchNotesSection Section;
		Section.Title = FText::FromStringTable(TableIdent, FTextKey(KeyPrefix + FString::FromInt(SectionIndex)));

		TArray<int32> BulletIndices;
		if (const TMap<int32, FString>* Bullets = SectionBullets.Find(SectionIndex))
		{
			Bullets->GetKeys(BulletIndices);
		}
		BulletIndices.Sort();
		for (const int32 BulletIndex : BulletIndices)
		{
			FPatchNotesBullet Bullet;
			Bullet.Text = FText::FromStringTable(
				TableIdent, FTextKey(KeyPrefix + FString::FromInt(SectionIndex) + "_" + FString::FromInt(BulletIndex)));

			TArray<int32> SubIndices;
			if (const TMap<int32, FString>* SubBullets = (SectionSubBullets.Find(SectionIndex)
			                                              ? SectionSubBullets[SectionIndex].Find(BulletIndex)
			                                              : nullptr))
			{
				SubBullets->GetKeys(SubIndices);
			}
			SubIndices.Sort();
			for (const int32 SubIndex : SubIndices)
			{
				Bullet.SubBulletTexts.Add(FText::FromStringTable(
					TableIdent, FTextKey(
						KeyPrefix + FString::FromInt(SectionIndex) + "_" + FString::FromInt(BulletIndex) + "_" +
						FString::FromInt(SubIndex))));
			}
			Section.Bullets.Add(MoveTemp(Bullet));
		}
		OutSections.Add(MoveTemp(Section));
	}
}

void UPatchNotesWidget::AddHeader(UWidgetTree* Tree,
                                  UVerticalBox* Root,
                                  const FText& DateText,
                                  const FPatchNotesStyle& Style)
{
	UHorizontalBox* HeaderRow = Tree->ConstructWidget<UHorizontalBox>();

	UTextBlock* VersionText = Tree->ConstructWidget<UTextBlock>();
	VersionText->SetText(FText::FromString(PatchVersionText));
	VersionText->SetFont(Style.Font_Version);
	HeaderRow->AddChildToHorizontalBox(VersionText)->SetVerticalAlignment(VAlign_Center);

	if (!DateText.IsEmpty())
	{
		UTextBlock* DateBlock = Tree->ConstructWidget<UTextBlock>();
		DateBlock->SetText(DateText);
		DateBlock->SetFont(Style.Font_Date);
		DateBlock->SetJustification(ETextJustify::Right);
		UHorizontalBoxSlot* DateSlot = HeaderRow->AddChildToHorizontalBox(DateBlock);
		DateSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		DateSlot->SetHorizontalAlignment(HAlign_Right);
		DateSlot->SetVerticalAlignment(VAlign_Center);
	}

	UVerticalBoxSlot* HeaderSlot = Root->AddChildToVerticalBox(HeaderRow);
	HeaderSlot->SetPadding(FMargin(0.f, 0.f, Style.RightEdgePadding, Style.SectionPadding));
}

void UPatchNotesWidget::AddSection(UWidgetTree* Tree,
                                   UVerticalBox* Root,
                                   const FPatchNotesSection& Section,
                                   const FPatchNotesStyle& Style)
{
	UVerticalBox* SectionBox = Tree->ConstructWidget<UVerticalBox>();

	UHorizontalBox* TitleRow = Tree->ConstructWidget<UHorizontalBox>();
	if (Style.SectionTitleIcon.GetResourceObject() != nullptr)
	{
		UImage* Icon = Tree->ConstructWidget<UImage>();
		Icon->SetBrush(Style.SectionTitleIcon);
		TitleRow->AddChildToHorizontalBox(Icon)->SetVerticalAlignment(VAlign_Center);
	}
	UTextBlock* Title = Tree->ConstructWidget<UTextBlock>();
	Title->SetText(Section.Title);
	Title->SetFont(Style.Font_SectionTitle);
	UHorizontalBoxSlot* TitleSlot = TitleRow->AddChildToHorizontalBox(Title);
	TitleSlot->SetVerticalAlignment(VAlign_Center);
	TitleSlot->SetPadding(FMargin(Style.IconTextSpacing, 0.f, 0.f, 0.f));
	SectionBox->AddChildToVerticalBox(TitleRow);

	for (const FPatchNotesBullet& Bullet : Section.Bullets)
	{
		AddBulletRow(Tree, SectionBox, Bullet.Text, 1, Style);
		for (const FText& SubText : Bullet.SubBulletTexts)
		{
			AddBulletRow(Tree, SectionBox, SubText, 2, Style);
		}
	}

	UVerticalBoxSlot* SectionSlot = Root->AddChildToVerticalBox(SectionBox);
	SectionSlot->SetPadding(FMargin(0.f, Style.SectionPadding, Style.RightEdgePadding, Style.SectionPadding));
}

void UPatchNotesWidget::AddBulletRow(UWidgetTree* Tree,
                                     UVerticalBox* SectionBox,
                                     const FText& Text,
                                     int32 Depth,
                                     const FPatchNotesStyle& Style)
{
	UHorizontalBox* Row = Tree->ConstructWidget<UHorizontalBox>();
	if (Style.BulletIcon.GetResourceObject() != nullptr)
	{
		UImage* Icon = Tree->ConstructWidget<UImage>();
		Icon->SetBrush(Style.BulletIcon);
		Row->AddChildToHorizontalBox(Icon)->SetVerticalAlignment(VAlign_Center);
	}

	UTextBlock* BulletText = Tree->ConstructWidget<UTextBlock>();
	BulletText->SetText(Text);
	BulletText->SetFont(Style.Font_Bullet);
	BulletText->SetAutoWrapText(true);
	UHorizontalBoxSlot* TextSlot = Row->AddChildToHorizontalBox(BulletText);
	TextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	TextSlot->SetVerticalAlignment(VAlign_Top);
	TextSlot->SetPadding(FMargin(Style.IconTextSpacing, 0.f, 0.f, 0.f));

	UVerticalBoxSlot* RowSlot = SectionBox->AddChildToVerticalBox(Row);
	RowSlot->SetPadding(FMargin(Style.IndentPerLevel * Depth, Style.BulletPaddingTop, 0.f, 0.f));
}
