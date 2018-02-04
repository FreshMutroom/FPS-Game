// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "MyGameInstance.h"
#include "GameStartGameMode.h"
#include "FPS_ProjectCharacter.h"
#include "MenuWidget.h"
#include "Runtime/UMG/Public/Components/ComboBoxString.h"
#include "MyStatics.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"
#include "MenuCamera.h"
#include "MyPlayerController.h"
#include "Runtime/MediaAssets/Public/MediaPlayer.h"
#include "BotStartLocation.h"
#include "Runtime/UMG/Public/Components/WidgetSwitcher.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "ActorSpawnPoint.h"
#include "MySaveGame.h"
#include "Sniper_Bot.h"
#include "MyAmbientSound.h"


UMyGameInstance::UMyGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//MoviePlayer = NewObject<UMediaPlayer>(UMediaPlayer::StaticClass());

}


void UMyGameInstance::InitWidgetArray(int32 Size)
{
	MenuWidgets.Init(nullptr, Size);

	for (int i = 0; i < BP_MenuWidgets.Num(); ++i)
	{
		UMenuWidget * Widget = CreateWidget<UMenuWidget>(this, BP_MenuWidgets[i]);
		if (Widget)
		{
			MenuWidgets[i] = Widget;
		}
	}

}

void UMyGameInstance::MakeAllWidgetsInvisible()
{
	UWorld * World = GetWorld();
	if (World)
	{
		//UWidgetLayoutLibrary::RemoveAllWidgets(World);
		for (UMenuWidget * Widget : MenuWidgets)
		{
			Widget->RemoveFromViewport();
		}

	}

	//RestoreHUD();

}

void UMyGameInstance::MakeWidgetActive(EWidgetTypeEnum WidgetType)
{

	UWorld * World = GetWorld();
	if (World)
	{
		//UWidgetLayoutLibrary::RemoveAllWidgets(World);
		for (UMenuWidget * Widget : MenuWidgets)
		{
			Widget->RemoveFromViewport();
		}

		uint8 Index = (uint8)WidgetType;
		UMenuWidget * Widget = MenuWidgets[Index];

		if (Widget && !Widget->IsInViewport())
		{
			if (Widget->OnVisibleAnimation)
			{
				
				if (WidgetType == EWidgetTypeEnum::Loading)
				{
					
					Widget->PlayAnimation(Widget->OnVisibleAnimation, 0.f, 0);
				}
				else
				{
					Widget->PlayAnimation(Widget->OnVisibleAnimation, 0.f, 1);
				}
				
			}
			
			Widget->AddToViewport(1);
			PreviousWidget = CurrentWidget;
			CurrentWidget = WidgetType;
		}

	}

	//RestoreHUD();

}

bool UMyGameInstance::InitWidget(EWidgetTypeEnum WidgetType)
{
	
	uint8 Index = (uint8)WidgetType;

	if (MenuWidgets[Index] == nullptr)
	{
		UWorld * World = GetWorld();
		if (World)
		{
			APlayerController * Controller = UGameplayStatics::GetPlayerController(World, 0);
			if (Controller)
			{
				
				if (BP_MenuWidgets.IsValidIndex(Index) && BP_MenuWidgets[Index])
				{
					UUserWidget * Widget = CreateWidget<UUserWidget>(Controller, BP_MenuWidgets[Index]);
					if (Widget)
					{
						UMenuWidget * AsMenuWidget = Cast<UMenuWidget>(Widget);
						if (AsMenuWidget)
						{
							/* Make sure all widget blueprints parent MenuWidget */
							
							MenuWidgets.Insert(AsMenuWidget, Index);

							return true;

						}
					}
				}
			}
		}
	}

	return false;

}

void UMyGameInstance::Main_OnStartGameButtonPressed()
{
	PlayUISound(EButtonType::NewGame);
	
	MakeAllWidgetsInvisible();

	StartNewGame();

}

void UMyGameInstance::Main_OnLoadGameButtonPressed()
{
	/* Check to see if a save game exists */
	if (UGameplayStatics::DoesSaveGameExist(TEXT("AutoSave"), 0))
	{
		PlayUISound(EButtonType::Select);
		
		MakeWidgetActive(EWidgetTypeEnum::ConfirmLoadSave);
		
	}
	else
	{
		/* No save game exists */


	}
	

}

void UMyGameInstance::Main_OnGameSettingsButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	MakeWidgetActive(EWidgetTypeEnum::GameSettings);

}

void UMyGameInstance::Main_OnVideoSettingsButtonPressed()
{
	
	PopulateVideoSettingsApplied();

	/* Make video menu open to show video settings and not audio/mouse settings */
	uint8 Index = (uint8)EWidgetTypeEnum::VideoSettings;
	UMenuWidget * Widget = MenuWidgets[Index];
	if (Widget && Widget->WidgetSwitcher)
	{
		Widget->WidgetSwitcher->SetActiveWidgetIndex(0);
	}

	PlayUISound(EButtonType::Confirm);

	MakeWidgetActive(EWidgetTypeEnum::VideoSettings);

	InitResolutionComboBox();

}

void UMyGameInstance::Main_OnExitButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	MakeWidgetActive(EWidgetTypeEnum::ConfirmExitToWindows);

}

void UMyGameInstance::Game_OnDifficultyButtonPressed(int32 Index)
{
	if (DifficultyMultipliers.IsValidIndex(Index))
	{
		PlayUISound(EButtonType::Select);
		
		DamageMultiplier = DifficultyMultipliers[Index];
	}

}

void UMyGameInstance::Game_OnReturnButtonPressed()
{
	PlayUISound(EButtonType::Confirm);
	
	MakeWidgetActive(PreviousWidget);

}

void UMyGameInstance::Pause_OnResumeButtonPressed()
{
	PlayUISound(EButtonType::ResumeGame);
	
	MakeAllWidgetsInvisible();

	UWorld * World = GetWorld();
	if (World)
	{
		UGameplayStatics::SetGamePaused(World, false);

		AController * Controller = UGameplayStatics::GetPlayerController(World, 0);
		if (Controller)
		{
			APlayerController * PlayerController = Cast<APlayerController>(Controller);
			if (PlayerController)
			{
				PlayerController->SetInputMode(FInputModeGameOnly());
				PlayerController->bShowMouseCursor = false;
			}
		}
	}

}

void UMyGameInstance::Pause_OnControlsButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	MakeWidgetActive(EWidgetTypeEnum::Controls);

}

void UMyGameInstance::Pause_OnSettingsButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	PopulateVideoSettingsApplied();

	/* Make video menu open to show video settings and not audio/mouse settings */
	uint8 Index = (uint8)EWidgetTypeEnum::VideoSettings;
	UMenuWidget * Widget = MenuWidgets[Index];
	if (Widget && Widget->WidgetSwitcher)
	{
		Widget->WidgetSwitcher->SetActiveWidgetIndex(0);
	}

	MakeWidgetActive(EWidgetTypeEnum::VideoSettings);

	InitResolutionComboBox();

}

void UMyGameInstance::Pause_OnLoadButtonPressed()
{
	/* Check to see if a save game exists */
	if (UGameplayStatics::DoesSaveGameExist(TEXT("AutoSave"), 0))
	{
		PlayUISound(EButtonType::Select);
		
		MakeWidgetActive(EWidgetTypeEnum::ConfirmLoadSave);
	}
	else
	{
		/* No save game exists */


	}

}

void UMyGameInstance::Pause_OnReturnToMainButtonPressed()
{
	PlayUISound(EButtonType::Cancel);
	
	MakeWidgetActive(EWidgetTypeEnum::ConfirmExitToMain);

}

void UMyGameInstance::Pause_OnExitButtonPressed()
{
	MakeWidgetActive(EWidgetTypeEnum::ConfirmExitToWindows);

}

void UMyGameInstance::Controls_OnReturnButtonPressed()
{
	PlayUISound(EButtonType::Select);

	MakeWidgetActive(EWidgetTypeEnum::Pause);

}

void UMyGameInstance::Video_OnWindowedModeButtonPressed(int32 Index)
{
	if (VideoSettings.IsValidIndex(0))
	{
		PlayUISound(EButtonType::Select);
		
		VideoSettings[0] = Index;
	}

}

void UMyGameInstance::Video_OnResolutionBoxChanged(FString NewResolution)
{
	PlayUISound(EButtonType::Select);
	
	ResolutionDirty = NewResolution;

}

void UMyGameInstance::Video_OnFramerateSliderChanged(float NewFramerate)
{
	if (NewFramerate >= 0.f)
	{
		
		FramerateLimitDirty = NewFramerate;
	}

}

void UMyGameInstance::Video_OnVSyncBoxChanged(bool NewValue)
{
	if (VideoSettings.IsValidIndex(1))
	{
		PlayUISound(EButtonType::Select);
		
		if (NewValue)
		{ 
			VideoSettings[1] = 1;
		}
		else
		{
			VideoSettings[1] = 0;
		}

	}

}

void UMyGameInstance::Video_OnPresetsButtonPressed(int32 Index)
{
	PlayUISound(EButtonType::Select);
	
	if (Index == 4)
	{
		VideoSettings[2] = 4;
	}
	else
	{
		SetVideoPresets(Index);

		FadeAAQualityButtons();

	}
	
}

void UMyGameInstance::Video_OnTexturesButtonPressed(int32 Index)
{
	if (VideoSettings.IsValidIndex(3))
	{
		PlayUISound(EButtonType::Select);
		
		if (Index != VideoSettings[3])
		{
			/* Change preset to custom*/
			VideoSettings[2] = 4;
		}
		
		VideoSettings[3] = Index;

	}

}

void UMyGameInstance::Video_OnDetailModeButtonPressed(int32 Index)
{
	if (VideoSettings.IsValidIndex(4))
	{
		PlayUISound(EButtonType::Select);
		
		if (Index != VideoSettings[4])
		{
			/* Change preset to custom*/
			VideoSettings[2] = 4;
		}
		
		VideoSettings[4] = Index;

	}

}

void UMyGameInstance::Video_OnShadowQualityButtonPressed(int32 Index)
{
	if (VideoSettings.IsValidIndex(5))
	{
		PlayUISound(EButtonType::Select);
		
		if (Index != VideoSettings[5])
		{
			/* Change preset to custom*/
			VideoSettings[2] = 4;
		}
		
		VideoSettings[5] = Index;

	}

}

void UMyGameInstance::Video_OnViewDistanceButtonPressed(int32 Index)
{
	if (VideoSettings.IsValidIndex(6))
	{
		PlayUISound(EButtonType::Select);
		
		if (Index != VideoSettings[6])
		{
			/* Change preset to custom*/
			VideoSettings[2] = 4;
		}
		
		VideoSettings[6] = Index;

		
	}

}

void UMyGameInstance::Video_OnAAModeButtonPressed(int32 Index)
{
	if (VideoSettings.IsValidIndex(7))
	{
		PlayUISound(EButtonType::Select);
		
		if (Index != VideoSettings[7])
		{
			/* Change preset to custom*/
			VideoSettings[2] = 4;
		}
		
		VideoSettings[7] = Index;

		/* Check if need to adjust AA quality Index */
		if (Index == 1)
		{
			if (VideoSettings[8] > 1)
			{
				VideoSettings[8] = 1;
			}
			
		}

		FadeAAQualityButtons();

	}

}

void UMyGameInstance::Video_OnAAQualityButtonPressed(int32 Index)
{
	if (VideoSettings.IsValidIndex(8))
	{
		PlayUISound(EButtonType::Select);
		
		/* Check if AA mode allows this option */
		if (VideoSettings[7] == 0)
		{
			return;
		}
		else if (VideoSettings[7] == 1)
		{
			if (Index == 2 || Index == 3)
			{
				return;
			}
		}

		if (Index != VideoSettings[8])
		{
			/* Change preset to custom*/
			VideoSettings[2] = 4;
		}

		VideoSettings[8] = Index;

	}

}

void UMyGameInstance::Video_OnEffectsQualityButtonPressed(int32 Index)
{
	if (VideoSettings.IsValidIndex(9))
	{
		PlayUISound(EButtonType::Select);
		
		if (Index != VideoSettings[9])
		{
			/* Change preset to custom*/
			VideoSettings[2] = 4;
		}
		
		VideoSettings[9] = Index;

	}

}

void UMyGameInstance::Video_OnFoliageQualityButtonPressed(int32 Index)
{
	if (VideoSettings.IsValidIndex(10))
	{
		PlayUISound(EButtonType::Select);
		
		if (Index != VideoSettings[10])
		{
			/* Change preset to custom*/
			VideoSettings[2] = 4;
		}
		
		VideoSettings[10] = Index;

	}

}

void UMyGameInstance::Video_OnPostProcessingButtonPressed(int32 Index)
{
	if (VideoSettings.IsValidIndex(11))
	{
		PlayUISound(EButtonType::Select);
		
		if (Index != VideoSettings[11])
		{
			/* Change preset to custom*/
			VideoSettings[2] = 4;
		}
		
		VideoSettings[11] = Index;

	}

}

void UMyGameInstance::Video_OnConfirmButtonPressed()
{
	if (CheckGameSettings())
	{
		ApplyVideoSettings(VideoSettings, FramerateLimitDirty, ResolutionDirty, AudioVolumeDirty, MouseSensitivityDirty);

	}

	PlayUISound(EButtonType::Confirm);

	MakeWidgetActive(PreviousWidget);

}

void UMyGameInstance::Video_OnCancelButtonPressed()
{
	PlayUISound(EButtonType::Cancel);
	
	MakeWidgetActive(PreviousWidget);

	AudioVolumeDirty = AudioVolume;

	UMyStatics::SetMenuVolume(AudioVolumeDirty);

	UpdateMenuMusicVolume();

}

void UMyGameInstance::Video_OnVolumeDownButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	AdjustAudioVolumeDirty(-0.05f);

	UpdateMenuMusicVolume();

}

void UMyGameInstance::Video_OnVolumeUpButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	AdjustAudioVolumeDirty(0.05f);

	UpdateMenuMusicVolume();

}

void UMyGameInstance::Video_OnMouseSensDownButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	MouseSensitivityDirty -= 0.05f;

	if (MouseSensitivityDirty < 0.05f)
	{
		MouseSensitivityDirty = 0.05f;
	}

}

void UMyGameInstance::Video_OnMouseSensUpButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	MouseSensitivityDirty += 0.05f;

	if (MouseSensitivityDirty > 2.f)
	{
		MouseSensitivityDirty = 2.f;
	}

}

void UMyGameInstance::Load_OnConfirmButtonPressed()
{
	PlayUISound(EButtonType::LoadGame);
	
	LoadGame();

	MakeAllWidgetsInvisible();

}

void UMyGameInstance::Load_OnBackButtonPressed()
{
	PlayUISound(EButtonType::Cancel);
	
	MakeWidgetActive(PreviousWidget);

}

void UMyGameInstance::ReturnMain_OnConfirmButtonPressed()
{
	PlayUISound(EButtonType::Cancel);
	
	MakeAllWidgetsInvisible();
	
	/* Destroy player. Maybe could leave everything else how it is. i.e do
	   not respawn AI */
	ACharacter * PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerCharacter)
	{
		PlayerCharacter->Destroy();
	}

	/* The menu cameras are not getting view when returning to menu.
	   ShowMainMenu must be doing it. In OnCreditsMovieFinished I do 
	   these exact 3 steps and the cameras get view fine. */

	ShowMainMenu();

}

void UMyGameInstance::ReturnMain_OnBackButtonPressed()
{
	PlayUISound(EButtonType::Cancel);
	
	MakeWidgetActive(EWidgetTypeEnum::Pause);

}

void UMyGameInstance::ReturnWindows_OnConfirmButtonPressed()
{
	QuitGame();
	
}

void UMyGameInstance::ReturnWindows_OnBackButtonPressed()
{
	PlayUISound(EButtonType::Cancel);
	
	MakeWidgetActive(PreviousWidget);

}

void UMyGameInstance::First_OnAudioDownButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	UMyStatics::AdjustMasterVolume(-0.05f);
	
	UMyStatics::AdjustMenuVolume(-0.05f);

	UpdateFirstScreenSoundVolume();

}

void UMyGameInstance::First_OnAudioUpButtonPressed()
{
	PlayUISound(EButtonType::Select);

	UMyStatics::AdjustMasterVolume(0.05f);

	UMyStatics::AdjustMenuVolume(0.05f);

	UpdateFirstScreenSoundVolume();

}

void UMyGameInstance::First_OnVideoCardQualityButtonPressed(int32 Index)
{
	PlayUISound(EButtonType::Select);
	
	FirstStartUpPresetIndex = Index;

}

void UMyGameInstance::First_OnDontKnowButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	FirstStartUpPresetIndex = 4;

}

void UMyGameInstance::First_OnContinueButtonPressed()
{
	if (FirstStartUpPresetIndex == -1)
	{
		return;
	}
	
	if (FirstStartUpPresetIndex == 4)
	{
		FirstStartUpPresetIndex = 0;
	}
	
	PlayUISound(EButtonType::Confirm);

	SetStartUpVideoPreset(FirstStartUpPresetIndex);
	
	UWorld * World = GetWorld();
	if (World)
	{
		AGameModeBase * GameMode = UGameplayStatics::GetGameMode(World);
		if (GameMode)
		{
			AGameStartGameMode * MyGameMode = Cast<AGameStartGameMode>(GameMode);
			if (MyGameMode)
			{
				if (!MyGameMode->bHasMapLoaded)
				{
					MakeWidgetActive(EWidgetTypeEnum::Loading);
				}
			}
		}
	}
	
	/* Stop sound loop */
	if (FirstAudioComponent)
	{
		FirstAudioComponent->Stop();
	}

	bRemoveLoadingScreen = true;

	/* Comment this if implementing a loading screen */
	//ShowMainMenu();

}

void UMyGameInstance::GameOver_OnLoadButtonPressed()
{
	PlayUISound(EButtonType::Confirm);
	
	HidePlayersWidgets();
	
	LoadGame();

}

void UMyGameInstance::GameOver_OnNewGameButtonPressed()
{
	PlayUISound(EButtonType::NewGame);
	
	HidePlayersWidgets();
	
	MakeAllWidgetsInvisible();
	
	/* Allowing cutscene to be skipped would be good */
	StartNewGame();

}

void UMyGameInstance::GameOver_OnDifficultyButtonPressed()
{
	PlayUISound(EButtonType::Select);
	
	MakeWidgetActive(EWidgetTypeEnum::GameSettings);

}

void UMyGameInstance::GameOver_OnMainButtonPressed()
{
	PlayUISound(EButtonType::Cancel);
	
	HidePlayersWidgets();

	ShowMainMenu();

}

void UMyGameInstance::GameOver_OnQuitButtonPressed()
{
	QuitGame();

}

void UMyGameInstance::SetStartUpVideoPreset(int32 Preset)
{

	switch (Preset)
	{
	case 0:
		VideoSettings = LowPresets;
		break;

	case 1:
		VideoSettings = MediumPresets;
		break;

	case 2:
		VideoSettings = HighPresets;
		break;

	case 3:
		VideoSettings = VeryHighPresets;
		break;

	default:
		break;
	}
	
	if (CheckGameSettings())
	{
		FIntPoint AsFIntPoint = GameSettings->GetDesktopResolution();

		FString StartingResolution = AGameStartGameMode::ToConsoleCommandFormat(AsFIntPoint);

		ApplyVideoSettings(VideoSettings, 0.f, StartingResolution, UMyStatics::GetMasterVolume(), 1.f);

	}
	
	/* Set difficulty to sightseeing */
	if (DifficultyMultipliers.IsValidIndex(1))
	{
		DamageMultiplier = DifficultyMultipliers[1];
	}

}

void UMyGameInstance::SetVideoPresets(int32 Preset)
{
	switch (Preset)
	{
	case 0:
		VideoSettings = LowPresets;
		break;

	case 1:
		VideoSettings = MediumPresets;
		break;

	case 2:
		VideoSettings = HighPresets;
		break;

	case 3:
		VideoSettings = VeryHighPresets;
		break;

	default:
		break;
	}

}

void UMyGameInstance::ApplyVideoSettings(const TArray<int32> & NewSettings, const float NewFramerate, const FString & NewResolution, const float NewAudioVolume, const float NewMouseSensitivity)
{
	// Change all VideoSettings references to NewSettings in here. This will still compile even if they are not
	// all changed but may run incorrectly
	
	UWorld * World = GetWorld();
	
	if (CheckGameSettings())
	{

		/* Apply windowed mode */
		switch (NewSettings[0])
		{
		case 0:
			GameSettings->SetFullscreenMode(EWindowMode::Windowed);
			break;
		
		case 1:
			GameSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
			break;
		
		case 2:
			GameSettings->SetFullscreenMode(EWindowMode::Fullscreen);
			break;
		
		default:
			break;
		}
		
		/* Apply resolution */
		if (World)
		{
			FString Command = "r.setRes " + NewResolution;
		
			if (NewSettings[0] == 0)
			{
				// windowed mode
				Command += "w";
				World->Exec(World, *Command);
			}
			else if (NewSettings[0] == 1)
			{
				// windowed fullscreen mode
				Command += "wf";
				World->Exec(World, *Command);
			}
			else if (NewSettings[0] == 2)
			{
				// fullscreen
				Command += "f";
				World->Exec(World, *Command);
			}
			
			Resolution = NewResolution;

		}
		//GameSettings->SetScreenResolution(AGameStartGameMode::ToResolutionFormat(NewResolution));

		/* Apply framerate limit */
		GameSettings->SetFrameRateLimit(NewFramerate);

		/* Apply VSync setting */
		if (VideoSettings[1] == 1)
		{
			GameSettings->SetVSyncEnabled(true);
		}
		else
		{
			GameSettings->SetVSyncEnabled(false);
		}

		/* Skip preset */

		/* Apply textures setting */
		GameSettings->SetTextureQuality(NewSettings[3]);

		/* Apply detail setting */
		if (World)
		{
			switch (NewSettings[4])
			{
			case 0:
				World->Exec(World, TEXT("r.DetailMode 0"));
				break;

			case 1:
				World->Exec(World, TEXT("r.DetailMode 1"));
				break;

			case 2:
				World->Exec(World, TEXT("r.DetailMode 2"));
				break;

			default:
				break;
			}
				
		}

		/* Apply shadow settings */
		GameSettings->SetShadowQuality(NewSettings[5]);

		/* Apply view distance setting */
		GameSettings->SetViewDistanceQuality(NewSettings[6]);

		/* Set AA mode and quality */
		/* Reference for r.PostProcessAAQuality values
		0: off
		1: Very Low (Faster FXAA)
		2: Low (FXAA)
		3: Medium (Faster TemporalAA)
		4: High (Default TemporallAA)
		5: Very High
		6: Max
		*/
		if (World)
		{
			if (NewSettings[7] == 0)
			{
				World->Exec(World, TEXT("r.PostProcessAAQuality 0"));
			}
			else if (NewSettings[7] == 1)
			{
				// Apply FXAA
				if (NewSettings[8] == 0)
				{
					World->Exec(World, TEXT("r.PostProcessAAQuality 1"));
				}
				else if (NewSettings[8] == 1)
				{
					World->Exec(World, TEXT("r.PostProcessAAQuality 2"));
				}
			}
			else if (NewSettings[7] == 2)
			{
				// Apply TAA
				if (NewSettings[8] == 0)
				{
					World->Exec(World, TEXT("r.PostProcessAAQuality 3"));
				}
				else if (NewSettings[8] == 1)
				{
					World->Exec(World, TEXT("r.PostProcessAAQuality 4"));
				}
				else if (NewSettings[8] == 2)
				{
					World->Exec(World, TEXT("r.PostProcessAAQuality 5"));
				}
				else if (NewSettings[8] == 3)
				{
					World->Exec(World, TEXT("r.PostProcessAAQuality 6"));
				}
			}

		}
			
		/* Set effects setting */
		GameSettings->SetVisualEffectQuality(NewSettings[9]);

		/* Set foliage settings */
		GameSettings->SetFoliageQuality(NewSettings[10]);

		/* Set post processing quality */
		GameSettings->SetPostProcessingQuality(NewSettings[11]);


		/* Make changes take effect */
		GameSettings->ApplySettings(false);

		/* This is only needed to assign values to things that I cannot query the system for e.g AA Mode */
		for (int i = 0; i < VideoSettings.Num(); ++i)
		{
			VideoSettingsApplied[i] = VideoSettings[i];
		}

		/* Set audio volume */
		AudioVolumeDirty = NewAudioVolume;
		UMyStatics::SetMenuVolume(AudioVolumeDirty);
		UMyStatics::SetMasterVolume(AudioVolumeDirty);

		/* Set mouse sensitivity */
		MouseSensitivity = NewMouseSensitivity;

		/* Set mouse sensitivity on player if they are spawned. Otherwise they will have to do it 
		   on their begin play */
		ACharacter * Character = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (Character)
		{
			AFPS_ProjectCharacter * PlayerCharacter = Cast<AFPS_ProjectCharacter>(Character);
			if (PlayerCharacter)
			{
				PlayerCharacter->ChangeMouseSensitivity(NewMouseSensitivity);
			}
		}

	}
	
	

}

void UMyGameInstance::PopulateVideoSettingsApplied()
{
	if (CheckGameSettings())
	{
		int Index = 0;
		if (VideoSettingsApplied.IsValidIndex(Index))
		{
			int32 Setting = -1;
			switch (GameSettings->GetFullscreenMode())
			{
			case EWindowMode::Windowed:
				Setting = 0;
				break;

			case EWindowMode::WindowedFullscreen:
				Setting = 1;
				break;

			case EWindowMode::Fullscreen:
				Setting = 2;
				break;

			default:
				break;
			}

			VideoSettingsApplied[Index] = Setting;
		}

		if (VideoSettingsApplied.IsValidIndex(++Index))
		{
			if (GameSettings->bUseVSync)
			{
				VideoSettingsApplied[Index] = 1;
			}
			else
			{
				VideoSettingsApplied[Index] = 0;
			}
		}

		/* Skip preset */
		Index++;

		if (VideoSettingsApplied.IsValidIndex(++Index))
		{
			VideoSettingsApplied[Index] = GameSettings->GetTextureQuality();
		}

		/* I don't know how to get detail mode so skip loading that one */
		Index++;

		if (VideoSettingsApplied.IsValidIndex(++Index))
		{
			VideoSettingsApplied[Index] = GameSettings->GetShadowQuality();
		}

		if (VideoSettingsApplied.IsValidIndex(++Index))
		{
			VideoSettingsApplied[Index] = GameSettings->GetViewDistanceQuality();
		}

		/* AA Mode goes here */
		Index++;

		/* Skip AA quality */
		Index++;

		if (VideoSettingsApplied.IsValidIndex(++Index))
		{
			VideoSettingsApplied[Index] = GameSettings->GetVisualEffectQuality();
		}

		if (VideoSettingsApplied.IsValidIndex(++Index))
		{
			VideoSettingsApplied[Index] = GameSettings->GetFoliageQuality();
		}

		if (VideoSettingsApplied.IsValidIndex(++Index))
		{
			VideoSettingsApplied[Index] = GameSettings->GetPostProcessingQuality();
		}

		FramerateLimit = GameSettings->GetFrameRateLimit();

		Resolution = AGameStartGameMode::ToConsoleCommandFormat(GameSettings->GetScreenResolution());

		/* Audio volume just uses UMyStatics::MasterVolume */
		AudioVolume = AudioVolumeDirty;

		/* MouseSensitivity for sensitivity */
		MouseSensitivityDirty = MouseSensitivity;


		for (int i = 0; i < VideoSettings.Num(); ++i)
		{
			VideoSettings[i] = VideoSettingsApplied[i];
		}

		FadeAAQualityButtons();

	}

}

void UMyGameInstance::InitVideoSettingsArray(int32 Size)
{
	VideoSettings.Init(INT32_MAX, Size);
	VideoSettingsApplied.Init(INT32_MAX, Size);

}

void UMyGameInstance::InitResolutionComboBox()
{
	
	uint8 Index = (uint8)EWidgetTypeEnum::VideoSettings;

	UMenuWidget * Widget = MenuWidgets[Index];

	if (Widget->ComboBox)
	{
		Widget->ComboBox->ClearOptions();
			
		const TArray<FString> SupportedResolutions = AGameStartGameMode::ChangeResolutionComboBox();
		for (FString Res : SupportedResolutions)
		{
			Widget->ComboBox->AddOption(Res);
		}

		Widget->ComboBox->RefreshOptions();
			
	}

	UpdateResolutionComboBox();

}

void UMyGameInstance::UpdateResolutionComboBox()
{
			
	uint8 Index = (uint8)EWidgetTypeEnum::VideoSettings;
	if (MenuWidgets[Index]->ComboBox)
	{
		MenuWidgets[Index]->ComboBox->SetSelectedOption(Resolution);
		//MenuWidgets[Index]->ComboBox->RefreshOptions();
	}

}

void UMyGameInstance::Initialize()
{
	CurrentWidget = PreviousWidget = EWidgetTypeEnum::None;

	HighlightedColor = FSlateColor(FLinearColor(0.5f, 0.015f, 0.f, 1.f));
	UnhighlightedColor = FSlateColor(FLinearColor(0.95f, 0.95f, 0.95f, 1.f));

	uint8 NumWidgets = ((uint8)EWidgetTypeEnum::None) + 1;
	InitWidgetArray(NumWidgets);

	InitVideoSettingsArray(12);

	InitUISounds();

}

void UMyGameInstance::ShowMainMenu()
{

	/* Unposses player pawn, make input mode UI only and show mouse cursor */
	APlayerController * PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		AMyPlayerController * MyPlayerController = Cast<AMyPlayerController>(PlayerController);
		if (MyPlayerController)
		{
			MyPlayerController->EnableMainMenuMode();
		}
	}

	// Play menu music and mute game sounds
	UMyStatics::SetMasterVolume(0.f);
	MainMenuAudioComponent = UMyStatics::PlayMenuSound(this, MainMenuMusic, FVector(0.f));

	// Put this here for if it's game over, but will freeze AI. Might not be needed anyway
	//UGameplayStatics::SetGamePaused(this, false);

	/* Destroy then spawn all AI */
	SpawnAI();

	SpawnPhysicsActors();

	SetLoadGameButtonOpacity();
	 
	MakeWidgetActive(EWidgetTypeEnum::Main);

	SwitchToMenuCamera();

}

void UMyGameInstance::SwitchToMenuCamera()
{

	if (MenuCameras.Num() == 0)
	{

		PopulateMenuCameraArray();
	}

	if (MenuCameras.IsValidIndex(MenuCameraIndex))
	{
		AMenuCamera * Camera = MenuCameras[MenuCameraIndex];
		if (Camera)
		{
			
			Camera->OnSwitchFrom();
		}
	}

	if (++MenuCameraIndex >= MenuCameras.Num())
	{
		
		MenuCameraIndex = 0;
	}

	UWorld * World = GetWorld();
	if (World && MenuCameras.IsValidIndex(MenuCameraIndex))
	{
		AMenuCamera * Camera = MenuCameras[MenuCameraIndex];
		if (Camera)
		{
			Camera->OnSwitchTo();

			PlayCameraFadeIn();

			TArray<AActor*> DebugArray;
			UGameplayStatics::GetAllActorsOfClass(this, AMenuCamera::StaticClass(), DebugArray);
			int32 NumCameras = DebugArray.Num();

			World->GetTimerManager().SetTimer(MenuCameraTimerHandle, this, &UMyGameInstance::SwitchToMenuCamera, Camera->Duration, false);

			World->GetTimerManager().SetTimer(CameraFadeOutTimerHandle, this, &UMyGameInstance::PlayCameraFadeOut, Camera->Duration - 1.6f, false);

		}
		
	}

}

void UMyGameInstance::PopulateMenuCameraArray()
{
	UWorld * World = GetWorld();
	if (World)
	{
		TArray<AActor*> Cameras;
		UGameplayStatics::GetAllActorsOfClass(World, AMenuCamera::StaticClass(), Cameras);
		
		int32 i = 0;
		for (AActor * Actor : Cameras)
		{
			AMenuCamera * Camera = Cast < AMenuCamera > (Actor);
			if (Camera)
			{
				MenuCameras.Insert(Camera, i);
			}

			i++;
		}

	}

}

void UMyGameInstance::ShowFirstTimeStartingGameScreen()
{
	/* This doesn't do anything to my knowledge */
	UWorld * World = GetWorld();
	if (World && CheckGameSettings())
	{
		GameSettings->SetFullscreenMode(EWindowMode::Windowed);
		
		FIntPoint AsFIntPoint = GameSettings->GetDesktopResolution();

		FString StartingResolution = AGameStartGameMode::ToConsoleCommandFormat(AsFIntPoint);
		FString Command = "r.setRes " + StartingResolution + "w";

		World->Exec(World, *Command);
	}
	
	MakeWidgetActive(EWidgetTypeEnum::FirstGameStartUp);

	/* Play sound on loop */
	FirstAudioComponent = UMyStatics::PlayMenuSound(this, FirstStartSound, FVector(0.f));

}

void UMyGameInstance::OnGamePaused()
{
	UWorld * World = GetWorld();
	if (World)
	{
		
		ACharacter * PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (PlayerCharacter)
		{
			AFPS_ProjectCharacter * Player = Cast<AFPS_ProjectCharacter>(PlayerCharacter);
			if (Player)
			{
				Player->bIsTABPressed = false;
			}
		}

		AController * Controller = UGameplayStatics::GetPlayerController(World, 0);
		if (Controller)
		{
			APlayerController * PlayerController = Cast<APlayerController>(Controller);
			if (PlayerController)
			{
				PlayerController->SetInputMode(FInputModeUIOnly());
				PlayerController->bShowMouseCursor = true;

			}
		}
	}

	MakeWidgetActive(EWidgetTypeEnum::Pause);

	UGameplayStatics::SetGamePaused(World, true);

}

void UMyGameInstance::StartNewGame()
{
	// Fade out menu music

	
	UWorld * World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(MenuCameraTimerHandle);
		World->GetTimerManager().ClearTimer(CameraFadeOutTimerHandle);

		APlayerCameraManager * PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0);
		if (PlayerCameraManager)
		{
			PlayerCameraManager->StopCameraFade();

			float CameraFadeDuration = 2.0f;
			
			PlayerCameraManager->StartCameraFade(0.f, 1.f, CameraFadeDuration, FColor::Black, false, true);

			APlayerController * PlayerController = UGameplayStatics::GetPlayerController(World, 0);
			if (PlayerController)
			{
				PlayerController->DisableInput(PlayerController);
				PlayerController->bShowMouseCursor = false;

				World->GetTimerManager().SetTimer(CameraFadeOutTimerHandle, this, &UMyGameInstance::StartNewGame_2, CameraFadeDuration + 1.f, false);

			}

			FadeOutMainMenuMusic(CameraFadeDuration);

		}
	}
	
	

}

void UMyGameInstance::StartNewGame_2()
{
	UWorld * World = GetWorld();
	if (World)
	{
		APlayerCameraManager * PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0);
		if (PlayerCameraManager)
		{
			PlayerCameraManager->StopCameraFade();
		}
	}
	
	/* Show opening cutscene if there is one */
	PlayOpeningMovie();

}

AFPS_ProjectCharacter * UMyGameInstance::SpawnPlayerCharacter()
{
	UWorld * World = GetWorld();
	if (World && BP_Player)
	{
		
		TArray<AActor*> PlayerStarts;

		UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerStarts);

		FVector StartLocation;
		FRotator StartRotation;

		for (AActor * Start : PlayerStarts)
		{
			StartLocation = Start->GetActorLocation();
			StartRotation = Start->GetActorRotation();
		}
		
		return World->SpawnActor<AFPS_ProjectCharacter>(BP_Player, StartLocation, StartRotation);

	}
	else
	{
		return nullptr;
	}

}

void UMyGameInstance::GetControlOfCharacter()
{
	MakeAllWidgetsInvisible();

	UWorld * World = GetWorld();
	if (World)
	{
		/* Stop menu cameras */
		World->GetTimerManager().ClearTimer(MenuCameraTimerHandle);

		for (AMenuCamera * Camera : MenuCameras)
		{
			Camera->OnSwitchFrom();

		}
		
		APlayerController * Controller = UGameplayStatics::GetPlayerController(World, 0);
		if (Controller)
		{
			/* Destroy all FPS_ProjectCharacters first so the soon to be spawned character 
			   will be at index 0 for UGameplayStatics methods. This may not cause things to still
			   work since some things get a reference to the player at begin play and that reference 
			   would have been destroyed. The player may need to be on BeginPlay be not allowed to fall
			   and when actually taken control of be teleported to the start location */
			TArray<AActor*> Characters;

			UGameplayStatics::GetAllActorsOfClass(World, AFPS_ProjectCharacter::StaticClass(), Characters);

			for (AActor * Actor : Characters)
			{
				Actor->Destroy();
			}

			AFPS_ProjectCharacter * Player = SpawnPlayerCharacter();
			if (Player)
			{
				/* Don't allow input until specified later */
				Controller->DisableInput(Controller);
				
				Controller->Possess(Player);

				Controller->SetInputMode(FInputModeGameOnly());
				Controller->bShowMouseCursor = false;
				//Controller->SetViewTarget(Player);	// Doesn't appear to be needed, posses does it anyway

				AGameModeBase * GameMode = UGameplayStatics::GetGameMode(World);
				if (GameMode)
				{
					AGameStartGameMode * MyGameMode = Cast<AGameStartGameMode>(GameMode);
					if (MyGameMode)
					{
						MyGameMode->SetupHUD();
					}
				}

			}
			
		}

	}

}

void UMyGameInstance::AllowPlayerInput()
{
	UWorld * World = GetWorld();
	if (World)
	{
		APlayerController * Controller = UGameplayStatics::GetPlayerController(World, 0);
		if (Controller)
		{
			Controller->EnableInput(Controller);
		}
	}
}

void UMyGameInstance::ShowGameOverScreen()
{
	UGameplayStatics::SetGamePaused(this, true);
	
	MakeWidgetActive(EWidgetTypeEnum::GameOver);

}

void UMyGameInstance::LoadGame()
{
	
	UWorld * World = GetWorld();
	if (World)
	{
		ACharacter * Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		if (Character)
		{
			/* Loading game after character has been created. i.e. from pause menu */
			
			AFPS_ProjectCharacter * PlayerCharacter = Cast<AFPS_ProjectCharacter>(Character);
			if (PlayerCharacter)
			{
				PlayerCharacter->LoadGame();
			}
		}
		else
		{
			/* Loading game from main menu - character has not been created */
			
			/* Check if a save game exists */
			bool bSaveGameExists = UGameplayStatics::DoesSaveGameExist(TEXT("AutoSave"), 0);
			if (bSaveGameExists)
			{
				/* Load save game. First spawn character */
				AFPS_ProjectCharacter * PlayerCharacter = SpawnPlayerCharacter();
				if (PlayerCharacter)
				{
					// Fade out menu music
					FadeOutMainMenuMusic(0.5f);
					
					FTimerHandle DummyTimerHandle;
					World->GetTimerManager().SetTimer(DummyTimerHandle, this, &UMyGameInstance::RestoreGameSounds, 0.5f, false);

					PlayerCharacter->LoadGame();
				}

			}
			else
			{
				/* No save game exists. Should never reach here */
			}

		}

		RestoreHUD();

		UGameplayStatics::SetGamePaused(World, false);

		AController * Controller = UGameplayStatics::GetPlayerController(World, 0);
		if (Controller)
		{
			APlayerController * PlayerController = Cast<APlayerController>(Controller);
			if (PlayerController)
			{
				PlayerController->SetInputMode(FInputModeGameOnly());
				PlayerController->bShowMouseCursor = false;
			}
		}

	}

}

void UMyGameInstance::QuitGame()
{
	UWorld * World = GetWorld();
	if (World)
	{
		AController * Controller = UGameplayStatics::GetPlayerController(World, 0);
		if (Controller)
		{
			APlayerController * PlayerController = Cast < APlayerController>(Controller);
			if (PlayerController)
			{
				UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit);
			}
		}

	}

}

void UMyGameInstance::HidePlayersWidgets()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(this);
	
	RestoreHUD();

	// This doesn't seem to work
	//ACharacter * Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	//if (Character)
	//{
	//	AFPS_ProjectCharacter * Player = Cast<AFPS_ProjectCharacter>(Character);
	//	if (Player)
	//	{
	//		if (Player->GameOverColorWidget)
	//		{
	//			Player->GameOverColorWidget->RemoveFromViewport();
	//		}
	//
	//		if (Player->LowHealthWidget)
	//		{
	//			Player->LowHealthWidget->RemoveFromViewport();
	//		}
	//	}
	//	
	//}

}

void UMyGameInstance::SaveGameSettingsToStorage()
{
	USaveGame* Save = UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass());
	if (Save)
	{
		UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(Save);
		if (SaveGameInstance)
		{
			SaveGameInstance->SaveSettings();
			
			UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("SettingsSave"), 1);
			
		}
	}

}

void UMyGameInstance::LoadGameSettingsFromStorage()
{
	/* load audio/mouse sens/difficulty settings */
	USaveGame* Load = UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSave"), 1);
	if (Load)
	{
		UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(Load);
		if (SaveGameInstance)
		{
			SaveGameInstance->LoadSettings();
		}
	}
	
	/* Load video settings */
	if (CheckGameSettings())
	{
		GameSettings->LoadSettings(true);
	}

}

bool UMyGameInstance::CheckGameSettings()
{
	if (GameSettings)
	{
		return true;
	}
	else
	{
		GameSettings = UGameUserSettings::GetGameUserSettings();
		if (GameSettings)
		{
			return true;
		}
		else 
		{
			return false;
		}
	}
	
}

void UMyGameInstance::FadeAAQualityButtons()
{
	/* Using an array of UserWidgets did not work. A UTextBlock and UButton had to 
	   be used to change alpha of the widgets */
	
	float FadeAlpha = 0.3f;
	
	uint8 Index = (uint8)EWidgetTypeEnum::VideoSettings;
	if (MenuWidgets.IsValidIndex(Index))
	{
		UMenuWidget * VideoWidget = MenuWidgets[Index];
		
		if (VideoWidget)
		{
			if (VideoSettings[7] == 0)
			{
				/* Fade text and all buttons */
				if (VideoWidget->TextWidget)
				{
					FLinearColor Color = VideoWidget->TextWidget->ColorAndOpacity.GetSpecifiedColor();
					VideoWidget->TextWidget->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, FadeAlpha));

					for (int i = 0; i < 4; ++i)
					{
						FLinearColor ButtonColor = VideoWidget->Buttons[i]->ColorAndOpacity;
						VideoWidget->Buttons[i]->SetColorAndOpacity(FLinearColor(ButtonColor.R, ButtonColor.G, ButtonColor.B, FadeAlpha));

					}
				}
				
			}
			else if (VideoSettings[7] == 1)
			{
				/* Fade in text and button 1 and 2, and fade out buttons 3 and 4*/
				if (VideoWidget->TextWidget)
				{
					FLinearColor Color = VideoWidget->TextWidget->ColorAndOpacity.GetSpecifiedColor();
					VideoWidget->TextWidget->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, 1.f));

					/* Fade in buttons */
					for (int i = 0; i < 2; ++i)
					{
						FLinearColor ButtonColor = VideoWidget->Buttons[i]->ColorAndOpacity;
						VideoWidget->Buttons[i]->SetColorAndOpacity(FLinearColor(ButtonColor.R, ButtonColor.G, ButtonColor.B, 1.f));

					}

					/* Fade out buttons*/
					for (int i = 2; i < 4; ++i)
					{
						FLinearColor ButtonColor = VideoWidget->Buttons[i]->ColorAndOpacity;
						VideoWidget->Buttons[i]->SetColorAndOpacity(FLinearColor(ButtonColor.R, ButtonColor.G, ButtonColor.B, FadeAlpha));

					}
				}
			}
			else if (VideoSettings[7] == 2)
			{
				/* Fade in text and all buttons */
				if (VideoWidget->TextWidget)
				{
					FLinearColor Color = VideoWidget->TextWidget->ColorAndOpacity.GetSpecifiedColor();
					VideoWidget->TextWidget->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, 1.f));

					for (int i = 0; i < 4; ++i)
					{
						FLinearColor ButtonColor = VideoWidget->Buttons[i]->ColorAndOpacity;
						VideoWidget->Buttons[i]->SetColorAndOpacity(FLinearColor(ButtonColor.R, ButtonColor.G, ButtonColor.B, 1.f));

					}
				}
			}
		}
	}

}

FSlateColor UMyGameInstance::GetButtonColor(const int32 SettingType, const int32 ButtonIndex)
{
	int32 Value = VideoSettings[SettingType];
	if (Value == ButtonIndex)
	{
		return HighlightedColor;
	}
	else
	{
		return UnhighlightedColor;
	}
	
}

FSlateColor UMyGameInstance::GetStartupButtonColor(const int32 Index)
{
	if (FirstStartUpPresetIndex == Index)
	{
		return HighlightedColor;
	}
	else
	{
		return UnhighlightedColor;
	}
	
}

FSlateColor UMyGameInstance::GetDifficultyButtonColor(const int32 Index)
{
	if (DifficultyMultipliers[Index] == DamageMultiplier)
	{
		return HighlightedColor;
	}
	else
	{
		return UnhighlightedColor;
	}
	
}

void UMyGameInstance::PlayCameraFadeIn()
{
	UWorld * World = GetWorld();
	if (World)
	{
		APlayerCameraManager * PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0);
		if (PlayerCameraManager)
		{
			PlayerCameraManager->StartCameraFade(1.f, 0.f, 1.5f, FColor::Black);
	
		}
	
	}
	
}

void UMyGameInstance::PlayCameraFadeOut()
{
	
	UWorld * World = GetWorld();
	if (World)
	{
		APlayerCameraManager * PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0);
		if (PlayerCameraManager)
		{
			PlayerCameraManager->StartCameraFade(0.f, 1.f, 1.5f, FColor::Black, false, true);

		}

	}

}

void UMyGameInstance::RestoreHUD()
{
	/* With how I remove widgets now this function never needs to be called */
	
	UWorld * World = GetWorld();
	if (World)
	{
		APlayerController * PlayerController = UGameplayStatics::GetPlayerController(World, 0);
		if (PlayerController->GetPawn())
		{
			AGameModeBase * GameMode = UGameplayStatics::GetGameMode(World);
			if (GameMode)
			{
				AGameStartGameMode * MyGameMode = Cast<AGameStartGameMode>(GameMode);
				if (MyGameMode)
				{
					MyGameMode->SetupHUD();
				}
			}
		}
		
	}

}

void UMyGameInstance::PlayOpeningMovie()
{
	bIsOpeningMoviePlaying = true;
	
	UWorld * World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(CameraFadeOutTimerHandle);
		World->GetTimerManager().ClearTimer(MenuCameraTimerHandle);
	}
	
	MakeWidgetActive(EWidgetTypeEnum::Cutscene);

	AddCutsceneOverlayToViewport();

	uint8 Index = (uint8)EWidgetTypeEnum::Cutscene;
	if (MenuWidgets.IsValidIndex(Index))
	{
		UMenuWidget * Widget = MenuWidgets[Index];
		if (Widget && Widget->MoviePlayer && Widget->Playlist)
		{
			/* This widget can be hidden when a cutscene is skipped, so this is needed */
			Widget->SetVisibility(ESlateVisibility::Visible);
			
			UMediaPlayer * MediaPlayer = Widget->MoviePlayer;
			UMediaPlaylist * Playlist = Widget->Playlist;

			MediaPlayer->OnEndReached.AddDynamic(this, &UMyGameInstance::OnOpeningMovieFinished);

			MediaPlayer->Rewind();

			MediaPlayer->SetLooping(false);

			MediaPlayer->SetRate(1.f);

			MediaPlayer->OpenPlaylistIndex(Playlist, 0);

		}

	}
	
}

void UMyGameInstance::OnOpeningMovieFinished()
{
	bIsOpeningMoviePlaying = false;
	
	RestoreGameSounds();
	
	/* Clear timers again cause of multithreading*/
	UWorld * World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(CameraFadeOutTimerHandle);
		World->GetTimerManager().ClearTimer(MenuCameraTimerHandle);
	}

	uint8 Index = (uint8)EWidgetTypeEnum::Cutscene;
	if (MenuWidgets.IsValidIndex(Index))
	{
		UMenuWidget * Widget = MenuWidgets[Index];
		if (Widget && Widget->MoviePlayer)
		{
			Widget->MoviePlayer->OnEndReached.RemoveDynamic(this, &UMyGameInstance::OnOpeningMovieFinished);

			Widget->MoviePlayer->Pause();
			Widget->MoviePlayer->Close();

			UWorld * World = GetWorld();
			if (World)
			{
					
				float FadeDuration = 1.f;

				Widget->FadeOut(FadeDuration);

				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &UMyGameInstance::GetControlOfCharacter, FadeDuration + 1.f, false);

			}

		}

	}

	/* Spawn bots */
	SpawnAI();

	SpawnPhysicsActors();

}

void UMyGameInstance::AddCutsceneOverlayToViewport()
{
	uint8 Index = (uint8)EWidgetTypeEnum::CutsceneMessageOverlay;
	if (MenuWidgets.IsValidIndex(Index))
	{
		UMenuWidget * Widget = MenuWidgets[Index];
		if (Widget)
		{
			Widget->AddToViewport(2);
		}

	}

}

void UMyGameInstance::StopOpeningMovie()
{
	if (bIsOpeningMoviePlaying)
	{
		bIsOpeningMoviePlaying = false;

		OnOpeningMovieFinished();

	}
	
	

}

void UMyGameInstance::SpawnAI()
{

	UWorld * World = GetWorld();
	if (World)
	{
		/* Destroy all AI first */
		TArray<AActor*> AllAI;

		UGameplayStatics::GetAllActorsWithInterface(World, UDamagableBot::StaticClass(), AllAI);

		for (AActor * Actor : AllAI)
		{
			Actor->Destroy();

		}

		/* Destroy sniper bot */
		TArray<AActor*> SniperBots;

		UGameplayStatics::GetAllActorsOfClass(World, ASniper_Bot::StaticClass(), SniperBots);

		for (AActor * Actor : SniperBots)
		{
			Actor->Destroy();

		}

		/* Spawn new AI now */

		TArray<AActor*> SpawnPoints;

		UGameplayStatics::GetAllActorsOfClass(World, ABotStartLocation::StaticClass(), SpawnPoints);

		for (AActor * Actor : SpawnPoints)
		{
			ABotStartLocation * SpawnPoint = Cast<ABotStartLocation>(Actor);
			if (SpawnPoint)
			{
				SpawnPoint->SpawnBot();

			}
		}

	}

}

void UMyGameInstance::SpawnPhysicsActors()
{
	/* Destroy all physics actors first. Done all in one loop below*/
	//TArray<AActor*> ToDestroy;
	//
	//UGameplayStatics::GetAllActorsOfClass(this, AItem::StaticClass(), ToDestroy);
	//for (AActor * Actor : ToDestroy)
	//{
	//	Actor->Destroy();
	//}
	//
	//ToDestroy.Empty();
	//UGameplayStatics::GetAllActorsOfClass(this, AFlamingRock::StaticClass(), ToDestroy);
	//for (AActor * Actor : ToDestroy)
	//{
	//	Actor->Destroy();
	//}

	TArray<AActor*> ToSpawn;

	UGameplayStatics::GetAllActorsOfClass(this, AActorSpawnPoint::StaticClass(), ToSpawn);

	for (AActor * Actor : ToSpawn)
	{
		AActorSpawnPoint * SpawnPoint = Cast<AActorSpawnPoint>(Actor);
		if (SpawnPoint)
		{
			SpawnPoint->DestroyLinkedActor();
			SpawnPoint->Spawn();
		}
	}

}

void UMyGameInstance::CopyArray(TArray<int32> & ToCopy, TArray<int32> & Clone)
{
	/* This doesn't appear to be working */
	
	if (ToCopy.Num() != Clone.Num())
	{
		return;
	}

	int32 Size = ToCopy.Num();
	for (int i = 0; i < Size; ++i)
	{
		Clone[i] = ToCopy[i];
	}

}

void UMyGameInstance::SetLoadGameButtonOpacity()
{
	/* This function works, just that assigning the button variable in the editor does not work 
	   on event construct so the first time the main menu is seen it does not have the fade */

	float FadeAlpha = 0.25f;

	if (UGameplayStatics::DoesSaveGameExist(TEXT("AutoSave"), 0))
	{
		
		uint8 Index = (uint8)EWidgetTypeEnum::Main;
		if (MenuWidgets.IsValidIndex(Index))
		{
			
			UMenuWidget * Widget = MenuWidgets[Index];
			if (Widget && Widget->Buttons.IsValidIndex(0))
			{
				
				FLinearColor Color = Widget->Buttons[0]->ColorAndOpacity;
				Widget->Buttons[0]->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, 1.f));
			}
		}
	}
	else
	{
		uint8 Index = (uint8)EWidgetTypeEnum::Main;
		if (MenuWidgets.IsValidIndex(Index))
		{
			UMenuWidget * Widget = MenuWidgets[Index];
			if (Widget && Widget->Buttons.IsValidIndex(0))
			{
				FLinearColor Color = Widget->Buttons[0]->ColorAndOpacity;
				Widget->Buttons[0]->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, FadeAlpha));
			}
		}
	}
}

void UMyGameInstance::InitUISounds()
{
	int32 Index = 0;
	
	UISounds.Insert(&UISelectSounds, Index++);

	UISounds.Insert(&UIConfirmSounds, Index++);

	UISounds.Insert(&UICancelSounds, Index++);
	
	UISounds.Insert(&UIStartNewGameSounds, Index++);
	
	UISounds.Insert(&UILoadGameSounds, Index++);
	
	UISounds.Insert(&UIResumeGameSounds, Index++);
}

void UMyGameInstance::PlayUISound(EButtonType Type)
{
	uint8 Index = (uint8)Type;
	if (UISounds.IsValidIndex(Index))
	{
		UMyStatics::PlayMenuSound(this, *UISounds[Index], FVector(0.f));
	}

}

void UMyGameInstance::FadeOutMainMenuMusic(float FadeDuration)
{
	if (MainMenuAudioComponent)
	{
		MainMenuAudioComponent->FadeOut(FadeDuration, 0.f);
	}

}

void UMyGameInstance::RestoreGameSounds()
{
	UMyStatics::SetMasterVolume(AudioVolumeDirty);

	UpdateAmbientSounds();

}

void UMyGameInstance::AdjustAudioVolumeDirty(float Amount)
{
	AudioVolumeDirty += Amount;

	if (AudioVolumeDirty < 0.f)
	{
		AudioVolumeDirty = 0.f;
	}
	else if (AudioVolumeDirty > 1.f)
	{
		AudioVolumeDirty = 1.f;
	}

	UMyStatics::AdjustMenuVolume(Amount);

}

void UMyGameInstance::UpdateFirstScreenSoundVolume()
{
	if (FirstAudioComponent)
	{
		float NewVolume = UMyStatics::GetMenuVolume();
		if (NewVolume <= 0.f)
		{
			NewVolume = 0.1f;
		}
		
		FirstAudioComponent->SetVolumeMultiplier(NewVolume);
	}

}

void UMyGameInstance::UpdateMenuMusicVolume()
{
	if (MainMenuAudioComponent)
	{
		float NewVolume = UMyStatics::GetMenuVolume();
		if (NewVolume <= 0.f)
		{
			NewVolume = 0.1f;
		}
		
		MainMenuAudioComponent->SetVolumeMultiplier(NewVolume);
	}

}

void UMyGameInstance::UpdateAmbientSounds()
{
	TArray<AActor *> AmbientSounds;

	UGameplayStatics::GetAllActorsOfClass(this, AMyAmbientSound::StaticClass(), AmbientSounds);

	for (AActor * Actor : AmbientSounds)
	{
		AMyAmbientSound * Sound = Cast<AMyAmbientSound>(Actor);
		if (Sound)
		{
			Sound->Update();

		}
	}

}

void UMyGameInstance::PlaySniperCutscene()
{
	/* Got to get pawn instead of character. */
	APawn * PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		AController * Controller = PlayerPawn->GetController();
		if (Controller)
		{
			APlayerController * PlayerController = Cast<APlayerController>(Controller);
			if (PlayerController)
			{
				PlayerPawn->DisableInput(PlayerController);
			}
		}

	}

	MakeWidgetActive(EWidgetTypeEnum::Cutscene);

	uint8 Index = (uint8)EWidgetTypeEnum::Cutscene;
	if (MenuWidgets.IsValidIndex(Index))
	{
		UMenuWidget * Widget = MenuWidgets[Index];
		if (Widget && Widget->MoviePlayer && Widget->Playlist)
		{
			/* This widget can be hidden when a cutscene is skipped, so this is needed */
			Widget->SetVisibility(ESlateVisibility::Visible);

			UMediaPlayer * MediaPlayer = Widget->MoviePlayer;
			UMediaPlaylist * Playlist = Widget->Playlist;

			MediaPlayer->OnEndReached.AddDynamic(this, &UMyGameInstance::OnSniperCutsceneFinished);

			UWorld * World = GetWorld();
			if (World)
			{
				FTimerHandle TeleportPlayerTimerHandle;
				World->GetTimerManager().SetTimer(TeleportPlayerTimerHandle, this, &UMyGameInstance::TeleportPlayerForSniperCutscene, 3.f, false);

				FTimerHandle SpawnSniperTimerHandle;
				World->GetTimerManager().SetTimer(SpawnSniperTimerHandle, this, &UMyGameInstance::SpawnSniperBot, 4.f, false);


			}

			MediaPlayer->Rewind();
			
			MediaPlayer->SetLooping(false);

			MediaPlayer->SetRate(1.f);

			MediaPlayer->OpenPlaylistIndex(Playlist, 1);

		}

	}

}

void UMyGameInstance::OnSniperCutsceneFinished()
{

	uint8 Index = (uint8)EWidgetTypeEnum::Cutscene;
	if (MenuWidgets.IsValidIndex(Index))
	{
		UMenuWidget * Widget = MenuWidgets[Index];
		if (Widget && Widget->MoviePlayer)
		{
			Widget->MoviePlayer->OnEndReached.RemoveDynamic(this, &UMyGameInstance::OnSniperCutsceneFinished);

			Widget->MoviePlayer->Pause();
			Widget->MoviePlayer->Close();

			UWorld * World = GetWorld();
			if (World)
			{
				ReenableInput();
			
				/* Hide cutscene widget. I think the movies keep playing in the background and therefore this
				   is necessary, but there's the CPU cost of constantly playing the video still there */
				MakeAllWidgetsInvisible();

			}

		}

	}

}

void UMyGameInstance::TeleportPlayerForSniperCutscene()
{
	
	APawn * PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		PlayerPawn->SetActorLocation(SniperCutsceneEndLocation);
		PlayerPawn->SetActorRotation(SniperCutsceneEndRotation);
		
		AController * Controller = PlayerPawn->GetController();
		if (Controller)
		{
			APlayerController * PlayerController = Cast<APlayerController>(Controller);
			if (PlayerController)
			{
				PlayerController->SetControlRotation(SniperCutsceneEndRotation);
			}
		}
	}

}

void UMyGameInstance::SpawnSniperBot()
{
	if (BP_SniperBot)
	{
		UWorld * World = GetWorld();
		if (World)
		{
			World->SpawnActor<ASniper_Bot>(BP_SniperBot, FVector(-53141.f, 19009.f, -1409.f), FRotator(0.f));
		}
	}

}

void UMyGameInstance::ReenableInput()
{
	/* Got to get pawn instead of character. */
	APawn * PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		AController * Controller = PlayerPawn->GetController();
		if (Controller)
		{
			APlayerController * PlayerController = Cast<APlayerController>(Controller);
			if (PlayerController)
			{
				PlayerPawn->EnableInput(PlayerController);
			}
		}

	}

}

void UMyGameInstance::PlayCreditsMovie()
{
	/* Got to get pawn instead of character. */
	APawn * PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		AController * Controller = PlayerPawn->GetController();
		if (Controller)
		{
			APlayerController * PlayerController = Cast<APlayerController>(Controller);
			if (PlayerController)
			{
				// Input is never reenabled but it seems ok for the menus. Game input will be 
				// reenabled when new/load game is done.
				PlayerPawn->DisableInput(PlayerController);
			}
		}

	}

	MakeWidgetActive(EWidgetTypeEnum::Cutscene);

	uint8 Index = (uint8)EWidgetTypeEnum::Cutscene;
	if (MenuWidgets.IsValidIndex(Index))
	{
		UMenuWidget * Widget = MenuWidgets[Index];
		if (Widget && Widget->MoviePlayer && Widget->Playlist)
		{
			/* This widget can be hidden when a cutscene is skipped, so this is needed */
			Widget->SetVisibility(ESlateVisibility::Visible);

			UMediaPlayer * MediaPlayer = Widget->MoviePlayer;
			UMediaPlaylist * Playlist = Widget->Playlist;

			/* For whatever reason this does not get the function called when the video finishes. 
			   This is only for this credit video - the other videos work fine. Have to use a timer handle
			   here instead. */
			//MediaPlayer->OnEndReached.AddDynamic(this, &UMyGameInstance::OnCreditsMovieFinished);

			UWorld * World = GetWorld();
			if (World)
			{
				float CreditsDuration = 7.f;
				
				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &UMyGameInstance::OnCreditsMovieFinished, CreditsDuration, false);

			}

			MediaPlayer->Rewind();

			MediaPlayer->SetLooping(false);

			MediaPlayer->SetRate(1.f);

			MediaPlayer->OpenPlaylistIndex(Playlist, 2);
			
		}

	}

}

void UMyGameInstance::OnCreditsMovieFinished()
{
	
	uint8 Index = (uint8)EWidgetTypeEnum::Cutscene;
	if (MenuWidgets.IsValidIndex(Index))
	{
		
		UMenuWidget * Widget = MenuWidgets[Index];
		if (Widget && Widget->MoviePlayer)
		{
			
			Widget->MoviePlayer->OnEndReached.RemoveDynamic(this, &UMyGameInstance::OnCreditsMovieFinished);

			Widget->MoviePlayer->Pause();
			Widget->MoviePlayer->Close();

			/* Hide cutscene widget. I think the movies keep playing in the background and therefore this 
			   is necessary, but there's the CPU cost of constantly playing the video still there */
			MakeAllWidgetsInvisible();

			// Destroy player to hide crosshair
			ACharacter * Character = UGameplayStatics::GetPlayerCharacter(this, 0);
			if (Character)
			{
				Character->Destroy();
			}

		}

	}
	
	ShowMainMenu();

}

