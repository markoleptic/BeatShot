// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Overlays/AudioSelectWidget.h"
#include <shobjidl_core.h>
#include "AudioAnalyzerManager.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/VerticalBox.h"
#include "Microsoft/COMPointer.h"
#include "Overlays/PopupMessageWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "SaveGames/SaveGamePlayerSettings.h"
#include "Utilities/SongMetadataWidget.h"
#include "Utilities/Buttons/BSButton.h"
#include "Utilities/ComboBox/BSComboBoxString.h"

namespace
{
#define MAX_FILETYPES_STR 4096
#define MAX_FILENAME_STR 65536 // This buffer has to be big enough to contain the names of all the selected files as well as the null characters between them and the null character at the end



bool TryGetMetadata(const TObjectPtr<UAudioAnalyzerManager>& Manager,
                    const FString& SongFilePath,
                    FBS_AudioConfig& InOutAudioConfig,
                    const bool OverrideMetadataFields)
{
	if (!Manager->InitPlayerAudio(SongFilePath))
	{
		InOutAudioConfig.SongMetadata = {};
		InOutAudioConfig.SongFilePath.Empty();
		return false;
	}

	// File path should always be updated
	InOutAudioConfig.SongFilePath = SongFilePath;

	if (OverrideMetadataFields)
	{
		FString FileName, _;
		auto& [Title, Artist, Album, Year, Genre, Length] = InOutAudioConfig.SongMetadata;
		Manager->GetMetadata(FileName, _, _, Title, Artist, Album, Year, Genre);
		if (Title.IsEmpty())
		{
			Title = FileName;
		}
		Length = Manager->GetTotalDuration();
	}

	return true;
}

FString MakeSongOptionLabel(const FSongMetadata& SongMetadata, const FString& SongFilePath)
{
	// Full paths are unique, so labels are unique, and the box may key on them.
	FString Label = SongMetadata.Title.IsEmpty() ? FPaths::GetBaseFilename(SongFilePath) : SongMetadata.Title;
	if (!SongMetadata.Artist.IsEmpty())
	{
		Label += TEXT(" — ") + SongMetadata.Artist;
	}
	return Label + TEXT(" (") + SongFilePath + TEXT(")");
}

FString FindSongPathByOptionLabel(const TMap<FString, FSongMetadata>& FilePathsAndSongMetadata,
                                  const FString& OptionLabel)
{
	for (const TPair<FString, FSongMetadata>& Current : FilePathsAndSongMetadata)
	{
		if (MakeSongOptionLabel(Current.Value, Current.Key) == OptionLabel)
		{
			return Current.Key;
		}
	}
	return FString();
}

constexpr int32 MaxSongMetadataFields = 5;

FString MakeDeviceSongOptionLabel(const FSongMetadata& SongMetadata,
                                  const int32 MaxFieldCount,
                                  const bool bUseFieldPrefixes)
{
	FString Label = SongMetadata.Title;
	if (MaxFieldCount >= 2 && !SongMetadata.Artist.IsEmpty())
	{
		Label += TEXT(" — ") + SongMetadata.Artist;
	}
	TArray<FString> ParentheticalParts;
	if (MaxFieldCount >= 3 && !SongMetadata.Album.IsEmpty())
	{
		if (bUseFieldPrefixes)
		{
			static const FString AlbumText = IBSWidgetInterface::GetWidgetTextFromKey("AF_SongAlbum").ToString();
			ParentheticalParts.Add(AlbumText + TEXT(" ") + SongMetadata.Album);
		}
		else
		{
			ParentheticalParts.Add(SongMetadata.Album);
		}
	}
	if (MaxFieldCount >= 4 && !SongMetadata.Year.IsEmpty())
	{
		if (bUseFieldPrefixes)
		{
			static const FString YearText = IBSWidgetInterface::GetWidgetTextFromKey("AF_SongYear").ToString();
			ParentheticalParts.Add(YearText + TEXT(" ") + SongMetadata.Year);
		}
		else
		{
			ParentheticalParts.Add(SongMetadata.Year);
		}
	}
	if (MaxFieldCount >= 5 && !SongMetadata.Genre.IsEmpty())
	{
		if (bUseFieldPrefixes)
		{
			static const FString GenreText = IBSWidgetInterface::GetWidgetTextFromKey("AF_SongGenre").ToString();
			ParentheticalParts.Add(GenreText + TEXT(" ") + SongMetadata.Genre);
		}
		else
		{
			ParentheticalParts.Add(SongMetadata.Genre);
		}
	}
	if (ParentheticalParts.Num() > 0)
	{
		Label += TEXT(" (") + FString::Join(ParentheticalParts, TEXT(", ")) + TEXT(")");
	}
	return Label;
}

struct FDeviceSongLabelSpec
{
	int32 FieldCount = 1;
	bool bUseFieldPrefixes = false;

	FDeviceSongLabelSpec(const int32 InFieldCount, const bool bInUseFieldPrefixes)
		: FieldCount(InFieldCount), bUseFieldPrefixes(bInUseFieldPrefixes)
	{
	}
};

/** Smallest-distinguishing label config for one shared-title group; the clean format is preferred, and field prefixes
 * (Album:, Year:, Genre:) are used only when it cannot disambiguate. */
FDeviceSongLabelSpec ComputeLabelSpec(const TArray<FSongMetadata>& TitleGroup)
{
	for (const bool bUseFieldPrefixes : {false, true})
	{
		for (int32 FieldCount = 1; FieldCount <= MaxSongMetadataFields; FieldCount++)
		{
			TSet<FString> DistinctLabels;
			for (const FSongMetadata& Current : TitleGroup)
			{
				DistinctLabels.Add(MakeDeviceSongOptionLabel(Current, FieldCount, bUseFieldPrefixes));
			}
			if (DistinctLabels.Num() == TitleGroup.Num())
			{
				return FDeviceSongLabelSpec(FieldCount, bUseFieldPrefixes);
			}
		}
	}
	// Unreachable: max fields with prefixes is an injective function of the unique 5-tuple.
	return FDeviceSongLabelSpec(MaxSongMetadataFields, true);
}

/** One option label per entry: bare Title, or extended per shared-title group as needed, with the field-prefix
 * fallback from ComputeLabelSpec when clean parts cannot disambiguate. */
TArray<FString> MakeDeviceSongOptionLabels(const TArray<FSongMetadata>& Entries)
{
	TArray<FString> OptionLabels;
	OptionLabels.SetNum(Entries.Num());
	TMap<FString, TArray<int32>> TitleToIndices;
	for (int32 Index = 0; Index < Entries.Num(); Index++)
	{
		TitleToIndices.FindOrAdd(Entries[Index].Title).Add(Index);
	}
	for (const TPair<FString, TArray<int32>>& TitlePair : TitleToIndices)
	{
		TArray<FSongMetadata> TitleGroup;
		for (const int32 Index : TitlePair.Value)
		{
			TitleGroup.Add(Entries[Index]);
		}
		const FDeviceSongLabelSpec LabelSpec = ComputeLabelSpec(TitleGroup);
		for (const int32 Index : TitlePair.Value)
		{
			OptionLabels[Index] = MakeDeviceSongOptionLabel(Entries[Index], LabelSpec.FieldCount,
			                                                LabelSpec.bUseFieldPrefixes);
		}
	}
	return OptionLabels;
}

const FSongMetadata* FindDeviceSongByOptionLabel(const TArray<FSongMetadata>& Entries, const FString& OptionLabel)
{
	const TArray<FString> OptionLabels = MakeDeviceSongOptionLabels(Entries);
	for (int32 Index = 0; Index < Entries.Num(); Index++)
	{
		if (OptionLabels[Index] == OptionLabel)
		{
			return &Entries[Index];
		}
	}
	return nullptr;
}
}

void UAudioSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Manager = NewObject<UAudioAnalyzerManager>(this);

	NumberFormattingOptions.MinimumIntegralDigits = 2;
	NumberFormattingOptions.MaximumIntegralDigits = 2;
	Checkbox_PlaybackAudio->SetIsChecked(true);

	Button_CaptureAudio->SetDefaults(static_cast<uint8>(EAudioFormat::Capture), Button_AudioFromFile);
	Button_AudioFromFile->SetDefaults(static_cast<uint8>(EAudioFormat::File), Button_CaptureAudio);

	Button_Start->SetIsEnabled(false);
	Box_SongMetadata->SetVisibility(ESlateVisibility::Collapsed);

	Button_Back->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleBackButtonClicked);
	Button_LoadFile->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleLoadFileButtonClicked);
	Button_Start->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleStartButtonClicked);
	Button_AudioFromFile->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleAudioFromFileButtonClicked);
	Button_CaptureAudio->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleCaptureAudioButtonClicked);
	Button_AddSong->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleAddSongButtonClicked);

	SongMetadataWidget->OnRemoveButtonClicked.BindUObject(this, &ThisClass::HandleRemoveButtonClicked);
	SongMetadataWidget->OnEditButtonClicked.BindUObject(this, &ThisClass::HandleEditButtonClicked);
	SongMetadataWidget->OnAddButtonClicked.BindUObject(this, &ThisClass::HandleAddButtonClicked);
	SongMetadataWidget->OnCommitButtonClicked.BindUObject(this, &ThisClass::HandleCommitButtonClicked);
	SongMetadataWidget->OnCancelButtonClicked.BindUObject(this, &ThisClass::HandleCancelButtonClicked);

	ComboBox_InAudioDevices->OnSelectionChanged.AddUniqueDynamic(
		this, &UAudioSelectWidget::OnSelectionChanged_InAudioDevice);
	ComboBox_AudioDeviceSongs->OnSelectionChanged.AddUniqueDynamic(
		this, &UAudioSelectWidget::OnSelectionChanged_AudioDeviceSongs);
	ComboBox_AudioFromFileSongs->OnSelectionChanged.AddUniqueDynamic(
		this, &UAudioSelectWidget::OnSelectionChanged_AudioFromFileSongs);
	Checkbox_PlaybackAudio->OnCheckStateChanged.AddUniqueDynamic(
		this, &UAudioSelectWidget::OnCheckStateChanged_PlaybackAudio);

	SetupTooltip(QMark_PlaybackAudio, GetTooltipTextFromKey("PlaybackAudio"));
	SetupTooltip(QMark_Input, GetTooltipTextFromKey("AF_InputAudioDevice"));
	SetupTooltip(QMark_AudioDeviceSong, GetTooltipTextFromKey("AF_AudioDeviceSong"));

	AudioFromFileAudioConfig.AudioFormat = EAudioFormat::File;
	AudioDeviceAudioConfig.AudioFormat = EAudioFormat::Capture;

	TArray<FString> InAudioDeviceList;
	Manager->GetInputAudioDevices(InAudioDeviceList);
	ComboBox_InAudioDevices->AddOptions(InAudioDeviceList);

	const auto PlayerSettings = LoadPlayerSettings().User;
	FilePathsAndSongMetadata = PlayerSettings.FilePathsAndSongMetadata;
	RebuildFileSongOptions(AudioFromFileAudioConfig.SongFilePath);

	RebuildDeviceSongOptions(AudioDeviceAudioConfig.SongMetadata);

	if (PlayerSettings.LastUsedAudioFormat == EAudioFormat::File)
	{
		Button_AudioFromFile->SetActive();
		HandleAudioFromFileButtonClicked(Button_AudioFromFile);
	}
	else if (PlayerSettings.LastUsedAudioFormat == EAudioFormat::Capture)
	{
		Button_CaptureAudio->SetActive();
		HandleCaptureAudioButtonClicked(Button_CaptureAudio);
	}
}

void UAudioSelectWidget::FadeIn()
{
	PlayAnimationForward(FadeInAnim);
}

void UAudioSelectWidget::FadeOut()
{
	FadeOutDelegate.BindDynamic(this, &UAudioSelectWidget::OnFadeOutFinish);
	PlayAnimationForward(FadeOutAnim);
	OnExitAudioSelect.ExecuteIfBound();
}

void UAudioSelectWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	SaveGamePlayerScore = InSaveGamePlayerScore;
}

void UAudioSelectWidget::OnFadeOutFinish()
{
	RemoveFromParent();
}

void UAudioSelectWidget::HandleLoadFileButtonClicked(const UBSButton*)
{
	TArray<FString> FileNames;
	const bool bSuccess = OpenFileDialog(FileNames);

	if (!bSuccess || FileNames.IsEmpty() || FileNames[0].IsEmpty())
	{
		return;
	}

	const FString SongFilePath = FileNames[0];

	if (TryGetMetadata(Manager, SongFilePath, AudioFromFileAudioConfig, true))
	{
		auto PlayerSettings = LoadPlayerSettings().User;
		PlayerSettings.AddSongMetadata(SongFilePath, AudioFromFileAudioConfig.SongMetadata);
		FilePathsAndSongMetadata = PlayerSettings.FilePathsAndSongMetadata;
		SavePlayerSettings(PlayerSettings);

		if (SaveGamePlayerScore->MaybeUpdateSongMetadata(AudioFromFileAudioConfig.SongMetadata))
		{
			SaveGamePlayerScore->SaveToSlot();
		}

		const auto OptionLabel = MakeSongOptionLabel(AudioFromFileAudioConfig.SongMetadata, SongFilePath);
		if (!ComboBox_AudioFromFileSongs->HasOption(OptionLabel))
		{
			ComboBox_AudioFromFileSongs->AddOption(OptionLabel);
		}
		ComboBox_AudioFromFileSongs->Sort();
		ComboBox_AudioFromFileSongs->SetSelectedOption(OptionLabel);
	}
	else
	{
		ShowSongPathErrorMessage();
	}

	UpdateSongInfoWidgets(EAudioFormat::File);
}

void UAudioSelectWidget::HandleAudioFromFileButtonClicked(const UBSButton*)
{
	if (ComboBox_AudioFromFileSongs->GetSelectedIndex() == -1)
	{
		const auto PlayerSettings = LoadPlayerSettings().User;
		if (FilePathsAndSongMetadata.Contains(PlayerSettings.LastUsedSongFilePath))
		{
			if (TryGetMetadata(Manager, PlayerSettings.LastUsedSongFilePath, AudioFromFileAudioConfig, false))
			{
				AudioFromFileAudioConfig.SongMetadata = FilePathsAndSongMetadata[PlayerSettings.LastUsedSongFilePath];
				ComboBox_AudioFromFileSongs->SetSelectedOption(MakeSongOptionLabel(
					AudioFromFileAudioConfig.SongMetadata, PlayerSettings.LastUsedSongFilePath));
			}
		}
	}

	Box_AudioFromFile->SetVisibility(ESlateVisibility::Visible);
	Box_AudioDevice->SetVisibility(ESlateVisibility::Collapsed);
	UpdateSongInfoWidgets(EAudioFormat::File);
}

void UAudioSelectWidget::HandleCaptureAudioButtonClicked(const UBSButton*)
{
	Button_Start->SetIsEnabled(false);

	if (ComboBox_InAudioDevices->GetSelectedIndex() == -1)
	{
		const auto PlayerSettings = LoadPlayerSettings().User;
		if (!PlayerSettings.LastUsedInputAudioDevice.IsEmpty() && ComboBox_InAudioDevices->HasOption(
			    PlayerSettings.LastUsedInputAudioDevice))
		{
			ComboBox_InAudioDevices->SetSelectedOption(PlayerSettings.LastUsedInputAudioDevice);
			AudioDeviceAudioConfig.InAudioDevice = PlayerSettings.LastUsedInputAudioDevice;
		}
		AudioDeviceAudioConfig.bPlaybackAudio = PlayerSettings.bPlaybackAudioDuringCapture;
		Checkbox_PlaybackAudio->SetIsChecked(AudioDeviceAudioConfig.bPlaybackAudio);
		if (PlayerSettings.LastUsedAudioDeviceSongMetadata.IsValid())
		{
			AudioDeviceAudioConfig.SongMetadata = PlayerSettings.LastUsedAudioDeviceSongMetadata;
		}
	}

	Box_AudioFromFile->SetVisibility(ESlateVisibility::Collapsed);
	Box_AudioDevice->SetVisibility(ESlateVisibility::Visible);
	UpdateSongInfoWidgets(EAudioFormat::Capture);
}

void UAudioSelectWidget::HandleAddSongButtonClicked(const UBSButton*)
{
	Box_SongMetadata->SetVisibility(ESlateVisibility::Visible);
	// Populate with blank metadata
	SongMetadataWidget->Populate(FSongMetadata{}, EAudioFormat::Capture, USongMetadataWidget::EMode::Add);
	// Indicate that the next steps need to be completed in the SongMetadata widget
	Button_AddSong->SetIsEnabled(false);
}

void UAudioSelectWidget::HandleBackButtonClicked(const UBSButton*)
{
	FadeOut();
}

void UAudioSelectWidget::HandleStartButtonClicked(const UBSButton*)
{
	auto PlayerSettings = LoadPlayerSettings().User;

	// Starting the session is what updates recency and the last-used fields.
	if (Button_AudioFromFile->IsActive())
	{
		PlayerSettings.MarkFileSongUsed(AudioFromFileAudioConfig);
	}
	else
	{
		PlayerSettings.MarkCaptureSongUsed(AudioDeviceAudioConfig);
	}
	SavePlayerSettings(PlayerSettings);

	if (!OnStartButtonClickedDelegate.ExecuteIfBound(Button_AudioFromFile->IsActive()
	                                                 ? AudioFromFileAudioConfig
	                                                 : AudioDeviceAudioConfig))
	{
		UE_LOG(LogTemp, Display, TEXT("OnStartButtonClickedDelegate not bound."));
	}
}

void UAudioSelectWidget::OnSelectionChanged_InAudioDevice(const TArray<FString>& ActiveSelections,
                                                          const ESelectInfo::Type SelectionType)
{
	if (ActiveSelections.IsEmpty() || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	const FString SelectedInAudioDevice = ActiveSelections[0];
	AudioDeviceAudioConfig.InAudioDevice = SelectedInAudioDevice;

	UpdateSongInfoWidgets(EAudioFormat::Capture);
}

void UAudioSelectWidget::OnSelectionChanged_AudioFromFileSongs(const TArray<FString>& ActiveSelections,
                                                               const ESelectInfo::Type SelectionType)
{
	if (ActiveSelections.IsEmpty() || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}

	const FString OptionLabel = ActiveSelections[0];
	const FString NewSongFilePath = FindSongPathByOptionLabel(FilePathsAndSongMetadata, OptionLabel);
	if (!ensureMsgf(!NewSongFilePath.IsEmpty(), TEXT("File option has no backing path: %s"), *OptionLabel))
	{
		return;
	}

	if (TryGetMetadata(Manager, NewSongFilePath, AudioFromFileAudioConfig, false))
	{
		AudioFromFileAudioConfig.SongMetadata = FilePathsAndSongMetadata[NewSongFilePath];
	}
	else
	{
		ShowSongPathErrorMessage();
	}

	UpdateSongInfoWidgets(EAudioFormat::File);
}

void UAudioSelectWidget::OnSelectionChanged_AudioDeviceSongs(const TArray<FString>& ActiveSelections,
                                                             const ESelectInfo::Type SelectionType)
{
	if (ActiveSelections.IsEmpty() || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}

	const FString OptionLabel = ActiveSelections[0];

	if (!OptionLabel.TrimStartAndEnd().IsEmpty())
	{
		const auto PlayerSettings = LoadPlayerSettings().User;
		const FSongMetadata* MatchingEntry = FindDeviceSongByOptionLabel(PlayerSettings.RecentAudioDeviceSongMetadata,
		                                                                 OptionLabel);
		if (ensureMsgf(MatchingEntry, TEXT("Song option has no backing entry: %s"), *OptionLabel))
		{
			AudioDeviceAudioConfig.SongMetadata = *MatchingEntry;
		}
	}

	UpdateSongInfoWidgets(EAudioFormat::Capture);
}

void UAudioSelectWidget::OnCheckStateChanged_PlaybackAudio(const bool bIsChecked)
{
	AudioDeviceAudioConfig.bPlaybackAudio = bIsChecked;
}

void UAudioSelectWidget::HandleCancelButtonClicked()
{
	if (Button_CaptureAudio->IsActive())
	{
		UpdateSongInfoWidgets(EAudioFormat::Capture);
	}
	else
	{
		UpdateSongInfoWidgets(EAudioFormat::File);
	}
}

void UAudioSelectWidget::HandleEditButtonClicked()
{
	// Edits must be canceled or commited before starting game mode.
	Button_Start->SetIsEnabled(false);
}

void UAudioSelectWidget::HandleRemoveButtonClicked()
{
	auto PlayerSettings = LoadPlayerSettings().User;
	if (Button_CaptureAudio->IsActive())
	{
		PlayerSettings.RemoveAudioDeviceSongMetadata(AudioDeviceAudioConfig.SongMetadata);
		SavePlayerSettings(PlayerSettings);

		if (PlayerSettings.LastUsedAudioDeviceSongMetadata.IsValid())
		{
			AudioDeviceAudioConfig.SongMetadata = PlayerSettings.LastUsedAudioDeviceSongMetadata;
		}
		else
		{
			AudioDeviceAudioConfig.SongMetadata = {};
		}

		UpdateSongInfoWidgets(EAudioFormat::Capture);
	}
	else
	{
		PlayerSettings.RemoveSongMetadata(AudioFromFileAudioConfig.SongFilePath);
		FilePathsAndSongMetadata = PlayerSettings.FilePathsAndSongMetadata;
		SavePlayerSettings(PlayerSettings);

		const auto RemovedOptionLabel = MakeSongOptionLabel(AudioFromFileAudioConfig.SongMetadata,
		                                                    AudioFromFileAudioConfig.SongFilePath);
		ComboBox_AudioFromFileSongs->RemoveOption(RemovedOptionLabel);

		if (FilePathsAndSongMetadata.Contains(PlayerSettings.LastUsedSongFilePath))
		{
			AudioFromFileAudioConfig.SongMetadata = FilePathsAndSongMetadata[PlayerSettings.LastUsedSongFilePath];
			if (TryGetMetadata(Manager, PlayerSettings.LastUsedSongFilePath, AudioFromFileAudioConfig, false))
			{
				ComboBox_AudioFromFileSongs->SetSelectedOption(MakeSongOptionLabel(
					AudioFromFileAudioConfig.SongMetadata, PlayerSettings.LastUsedSongFilePath));
			}
		}
		else
		{
			AudioFromFileAudioConfig.SongMetadata = {};
			AudioFromFileAudioConfig.SongFilePath.Empty();
		}
		UpdateSongInfoWidgets(EAudioFormat::File);
	}
}

void UAudioSelectWidget::HandleAddButtonClicked()
{
	auto PlayerSettings = LoadPlayerSettings().User;
	if (Button_CaptureAudio->IsActive())
	{
		const auto SongMetadata = SongMetadataWidget->GetSongMetadata();
		// Use the typed metadata, not the stale config selection.
		if (PlayerSettings.AddAudioDeviceSongMetadata(SongMetadata))
		{
			AudioDeviceAudioConfig.SongMetadata = SongMetadata;
			SavePlayerSettings(PlayerSettings);

			UpdateSongInfoWidgets(EAudioFormat::Capture);
		}
		else
		{
			SongMetadataWidget->SetErrorMessage(IBSWidgetInterface::GetWidgetTextFromKey("AF_ExistingMetadataError"));
		}
	}
}

void UAudioSelectWidget::HandleCommitButtonClicked()
{
	auto PlayerSettings = LoadPlayerSettings().User;
	const auto SongMetadata = SongMetadataWidget->GetSongMetadata();
	if (Button_CaptureAudio->IsActive())
	{
		if (PlayerSettings.EditAudioDeviceSongMetadata(AudioDeviceAudioConfig.SongMetadata, SongMetadata))
		{
			AudioDeviceAudioConfig.SongMetadata = SongMetadata;
			SavePlayerSettings(PlayerSettings);

			UpdateSongInfoWidgets(EAudioFormat::Capture);
		}
		else
		{
			SongMetadataWidget->SetErrorMessage(IBSWidgetInterface::GetWidgetTextFromKey("AF_ExistingMetadataError"));
		}
	}
	else
	{
		PlayerSettings.EditSongMetadata(AudioFromFileAudioConfig.SongFilePath, SongMetadata);

		AudioFromFileAudioConfig.SongMetadata = SongMetadata;
		FilePathsAndSongMetadata = PlayerSettings.FilePathsAndSongMetadata;
		SavePlayerSettings(PlayerSettings);

		UpdateSongInfoWidgets(EAudioFormat::File);
	}
}

void UAudioSelectWidget::RebuildFileSongOptions(const FString& SelectedSongFilePath)
{
	const auto PlayerSettings = LoadPlayerSettings().User;
	ComboBox_AudioFromFileSongs->ClearOptions();
	for (const TPair<FString, FSongMetadata>& FilePathToMetadata : PlayerSettings.FilePathsAndSongMetadata)
	{
		ComboBox_AudioFromFileSongs->AddOption(MakeSongOptionLabel(FilePathToMetadata.Value, FilePathToMetadata.Key));
	}
	ComboBox_AudioFromFileSongs->Sort();

	const FSongMetadata* FoundMetadata = PlayerSettings.FilePathsAndSongMetadata.Find(SelectedSongFilePath);
	if (FoundMetadata)
	{
		ComboBox_AudioFromFileSongs->SetSelectedOption(MakeSongOptionLabel(*FoundMetadata, SelectedSongFilePath));
	}
}

void UAudioSelectWidget::RebuildDeviceSongOptions(const FSongMetadata& SelectedSongMetadata)
{
	const auto PlayerSettings = LoadPlayerSettings().User;
	const auto& Entries = PlayerSettings.RecentAudioDeviceSongMetadata;
	const TArray<FString> OptionLabels = MakeDeviceSongOptionLabels(Entries);
	ComboBox_AudioDeviceSongs->ClearOptions();
	for (const FString& OptionLabel : OptionLabels)
	{
		ComboBox_AudioDeviceSongs->AddOption(OptionLabel);
	}
	ComboBox_AudioDeviceSongs->Sort();
	const int32 SelectedIndex = Entries.IndexOfByPredicate([&SelectedSongMetadata](const FSongMetadata& Current)
	{
		return Current.IsEqualExcludingLength(SelectedSongMetadata);
	});
	if (SelectedIndex != INDEX_NONE)
	{
		ComboBox_AudioDeviceSongs->SetSelectedOption(OptionLabels[SelectedIndex]);
	}
}

void UAudioSelectWidget::UpdateSongInfoWidgets(const EAudioFormat AudioFormat)
{
	bool ShowSongMetadata = false;
	bool EnableStartButton = false;
	FBS_AudioConfig* AudioConfig = nullptr;

	if (AudioFormat == EAudioFormat::File)
	{
		AudioConfig = &AudioFromFileAudioConfig;
		ShowSongMetadata = !AudioFromFileAudioConfig.SongFilePath.IsEmpty();
		EnableStartButton = ShowSongMetadata && AudioFromFileAudioConfig.SongMetadata.IsValid();
	}
	else if (AudioFormat == EAudioFormat::Capture)
	{
		AudioConfig = &AudioDeviceAudioConfig;
		ShowSongMetadata = !AudioDeviceAudioConfig.InAudioDevice.IsEmpty() && AudioDeviceAudioConfig.SongMetadata.
		                   IsValid();
		EnableStartButton = ShowSongMetadata;
	}

	if (AudioConfig)
	{
		if (AudioFormat == EAudioFormat::File)
		{
			RebuildFileSongOptions(AudioConfig->SongFilePath);
			SongMetadataWidget->Populate(AudioConfig->SongMetadata, AudioFormat, USongMetadataWidget::EMode::Edit);
			Button_AddSong->SetIsEnabled(true);
		}
		else if (AudioFormat == EAudioFormat::Capture)
		{
			RebuildDeviceSongOptions(AudioConfig->SongMetadata);
			if (ShowSongMetadata)
			{
				SongMetadataWidget->Populate(AudioConfig->SongMetadata, AudioFormat, USongMetadataWidget::EMode::Edit);
			}
		}
	}

	Box_SongMetadata->SetVisibility(ShowSongMetadata ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	Button_Start->SetIsEnabled(EnableStartButton);
}

bool UAudioSelectWidget::OpenFileDialog(TArray<FString>& OutFileNames)
{
	const FString DialogTitle = "Choose a Song in .mp3 or .ogg Format";
	const FString DefaultPath;
	const FString DefaultFile;
	const FString FileTypes = ".mp3,.ogg";
	int OutFilterIndex = 0;
	return FileDialogShared(false, GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle(),
	                        DialogTitle, DefaultPath, DefaultFile, FileTypes, 0, OutFileNames, OutFilterIndex);
}

bool UAudioSelectWidget::FileDialogShared(const bool bSave,
                                          const void* ParentWindowHandle,
                                          const FString& DialogTitle,
                                          const FString& DefaultPath,
                                          const FString& DefaultFile,
                                          const FString& FileTypes,
                                          uint32 /*Flags*/,
                                          TArray<FString>& OutFilenames,
                                          int32& OutFilterIndex)
{
	bool bSuccess;

#if PLATFORM_WINDOWS
	WCHAR Filename[MAX_FILENAME_STR];
	FCString::Strncpy(Filename, *DefaultFile.Replace(TEXT("/"), TEXT("\\")), MAX_FILENAME_STR);

	// Convert the forward slashes in the path name to backslashes, otherwise it'll be ignored as invalid and use whatever is cached in the registry
	WCHAR Pathname[MAX_FILENAME_STR];
	FCString::Strncpy(Pathname, *FPaths::ConvertRelativePathToFull(DefaultPath).Replace(TEXT("/"), TEXT("\\")),
	                  MAX_FILENAME_STR);

	// Convert the "|" delimited list of filetypes to NULL delimited then add a second NULL character to indicate the end of the list
	WCHAR FileTypeStr[MAX_FILETYPES_STR];
	WCHAR* FileTypesPtr = nullptr;
	const int32 FileTypesLen = FileTypes.Len();

	// Nicely formatted file types for lookup later and suitable to append to filenames without extensions
	TArray<FString> CleanExtensionList;

	// The strings must be in pairs for windows.
	// It is formatted as follows: Pair1String1|Pair1String2|Pair2String1|Pair2String2
	// where the second string in the pair is the extension.  To get the clean extensions we only care about the second string in the pair
	TArray<FString> UnformattedExtensions;
	FileTypes.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);
	for (int32 ExtensionIndex = 1; ExtensionIndex < UnformattedExtensions.Num(); ExtensionIndex += 2)
	{
		const FString& Extension = UnformattedExtensions[ExtensionIndex];
		// Assume the user typed in an extension or doesnt want one when using the *.* extension. We can't determine what extension they wan't in that case
		if (Extension != TEXT("*.*"))
		{
			// Add to the clean extension list, first removing the * wildcard from the extension
			const int32 WildCardIndex = Extension.Find(TEXT("*"));
			CleanExtensionList.Add(WildCardIndex != INDEX_NONE ? Extension.RightChop(WildCardIndex + 1) : Extension);
		}
	}

	if (FileTypesLen > 0 && FileTypesLen - 1 < MAX_FILETYPES_STR)
	{
		FileTypesPtr = FileTypeStr;
		FCString::Strncpy(FileTypeStr, *FileTypes, MAX_FILETYPES_STR);

		TCHAR* Pos = FileTypeStr;
		while (Pos[0] != 0)
		{
			if (Pos[0] == '|')
			{
				Pos[0] = 0;
			}

			Pos++;
		}

		// Add two trailing NULL characters to indicate the end of the list
		FileTypeStr[FileTypesLen] = 0;
		FileTypeStr[FileTypesLen + 1] = 0;
	}

	OPENFILENAME Ofn;
	FMemory::Memzero(&Ofn, sizeof(OPENFILENAME));

	Ofn.lStructSize = sizeof(OPENFILENAME);
	Ofn.hwndOwner = (HWND)ParentWindowHandle;
	Ofn.lpstrFilter = FileTypesPtr;
	Ofn.nFilterIndex = 1;
	Ofn.lpstrFile = Filename;
	Ofn.nMaxFile = MAX_FILENAME_STR;
	Ofn.lpstrInitialDir = Pathname;
	Ofn.lpstrTitle = *DialogTitle;
	if (FileTypesLen > 0)
	{
		Ofn.lpstrDefExt = &FileTypeStr[0];
	}

	Ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER;

	if (bSave)
	{
		Ofn.Flags |= OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_NOVALIDATE;
	}
	else
	{
		Ofn.Flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	}

	if (bSave)
	{
		bSuccess = !!::GetSaveFileName(&Ofn);
	}
	else
	{
		bSuccess = !!::GetOpenFileName(&Ofn);
	}

	if (bSuccess)
	{
		// GetOpenFileName/GetSaveFileName changes the CWD on success. Change it back immediately.
		//FPlatformProcess::SetCurrentWorkingDirectoryToBaseDir();
		UE_LOG(LogTemp, Display, TEXT("%s"), Filename);

		new(OutFilenames) FString(Filename);

		// The index of the filter in OPENFILENAME starts at 1.
		OutFilterIndex = Ofn.nFilterIndex - 1;

		// Get the extension to add to the filename (if one doesnt already exist)
		FString Extension = CleanExtensionList.IsValidIndex(OutFilterIndex)
		                    ? CleanExtensionList[OutFilterIndex]
		                    : TEXT("");

		// Make sure all filenames gathered have their paths normalized and proper extensions added
		for (auto OutFilenameIt = OutFilenames.CreateIterator(); OutFilenameIt; ++OutFilenameIt)
		{
			FString& OutFilename = *OutFilenameIt;

			OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);

			if (FPaths::GetExtension(OutFilename).IsEmpty() && !Extension.IsEmpty())
			{
				// filename does not have an extension. Add an extension based on the filter that the user chose in the dialog
				OutFilename += Extension;
			}

			FPaths::NormalizeFilename(OutFilename);
		}
	}
	else
	{
		const uint32 Error = ::CommDlgExtendedError();
		if (Error != ERROR_SUCCESS)
		{
			//UE_LOG(LogDesktopPlatform, Warning, TEXT("Error reading results of file dialog. Error: 0x%04X"), Error);
		}
	}
#endif

	return bSuccess;
}

void UAudioSelectWidget::ShowSongPathErrorMessage()
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(GetWorld(), PopupMessageClass);
	TArray<UBSButton*> Buttons = PopupMessageWidget->InitPopup(GetWidgetTextFromKey("G_Error"),
	                                                           GetWidgetTextFromKey("ASW_SongPathErrorMessage"), 1);
	if (!Buttons.IsEmpty())
	{
		Buttons[0]->SetButtonText(GetWidgetTextFromKey("G_Okay"));
		Buttons[0]->OnBSButtonPressed.AddLambda([this](const UBSButton* /*Button*/)
		{
			if (PopupMessageWidget)
			{
				PopupMessageWidget->FadeOut();
				FadeOut();
			}
		});
	}
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}
