// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "MyGameInstance.generated.h"


#define HIDDEN ESlateVisibility::Hidden
#define VISIBLE ESlateVisibility::Visible


UENUM(BlueprintType)
enum class EWidgetTypeEnum : uint8
{
	Main 	UMETA(DisplayName = "Main Menu"),
	GameSettings 	UMETA(DisplayName = "Game Settings Menu"),
	VideoSettings	UMETA(DisplayName = "Video/Audio/Mouse Settings Menu"),
	Pause	UMETA(DisplayName = "Pause Menu"),
	Controls	UMETA(DisplayName = "Controls Menu"),
	FirstGameStartUp	UMETA(DisplayName = "Menu Shown When First Starting Up Game"),

	ConfirmExitToMain	UMETA(DisplayName = "Confirm Exit To Main Menu Popup"),
	ConfirmExitToWindows	UMETA(DisplayName = "Confirm Exit To Windows Popup"),
	ConfirmLoadSave		UMETA(DisplayName = "Confirm Load Last Save Popup"),

	Loading		UMETA(DisplayName = "Loading Screen on game start up"),
	Cutscene	UMETA(DisplayName = "Cutscene"),

	GameOver	UMETA(DisplayName = "Game Over Screen"),

	CutsceneMessageOverlay	UMETA(DisplayName = "Displays text during cutscene"),

	/* This should always be the last widget type in this enum */
	None	UMETA(DisplayName = "Initial value")
};


UENUM(BlueprintType)
enum class EButtonType : uint8
{
	Select  UMETA(DisplayName = "Select option button"),
	Confirm  UMETA(DisplayName = "Confirm settings change button"),
	Cancel  UMETA(DisplayName = "Cancel settings change button"),
	NewGame  UMETA(DisplayName = "Sound when starting new game"),
	LoadGame  UMETA(DisplayName = "Sound when loading game"),
	ResumeGame  UMETA(DisplayName = "Sound when resuming game from pause"),

};


/**
 *	This is the place to put variables that you only ever need one copy of
 */
UCLASS()
class FPS_PROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_UCLASS_BODY()
	


public:

	/* The menu widget that is visibile to the player right now */
	UPROPERTY()
	EWidgetTypeEnum CurrentWidget;
	
	/* The menu widget that was just made invisible */
	UPROPERTY()
	EWidgetTypeEnum PreviousWidget;
	
	/* Array that holds the blueprints for every menu widget. Should be populated in editor
		0 = Main
		1 = Game Settings
		2 = Video Settings
		3 = Pause Menu
		4 = Controls Menu
		5 = First Game Start Up Menu
		6 = Confirm return to main menu popup
		7 = Confirm exit to windows popup
		8 = Confirm load save game popup
		9 = Loading Screen
		10 = Cutscene 
		11 = Game Over Screen
		12 = Cutscene Message Overlay
	*/
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UUserWidget>> BP_MenuWidgets;


	/* Array that holds every menu widget that has been created 
		0 = Main
		1 = Game Settings
		2 = Video Settings
		3 = Pause Menu
		4 = Controls Menu
		5 = First Game Start Up Menu
		6 = Confirm return to main menu popup
		7 = Confirm exit to windows popup
		8 = Confirm load save game popup
		9 = Loading Screen
		10 = Cutscene
		11 = Game Over Screen
		12 = Cutscene Message Overlay
	*/
	UPROPERTY()
	TArray<class UMenuWidget*> MenuWidgets;

	/* Initilize MenuWidgets by inserting nullptr in first Size indexes */
	UFUNCTION(BlueprintCallable)
	void InitWidgetArray(int32 Size);


	/* Make all menu widgets invisible */
	UFUNCTION(BlueprintCallable)
	void MakeAllWidgetsInvisible();

	/* Make a widget visible
	@param WidgetType The type of widget to make visible
	*/
	UFUNCTION(BlueprintCallable)
	void MakeWidgetActive(EWidgetTypeEnum WidgetType);

	/* If a widget of this type does not exist then create the widget, add it to the players
	array of widgets, add it to the viewport and make it invisible. Returns true if widget 
	is created successfully */
	UFUNCTION(BlueprintCallable)
	bool InitWidget(EWidgetTypeEnum WidgetType);


	/** Fields for the main menu */

	UFUNCTION(BlueprintCallable)
	void Main_OnStartGameButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Main_OnLoadGameButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Main_OnGameSettingsButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Main_OnVideoSettingsButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Main_OnExitButtonPressed();


	/** Fields for game settings menu */
	
	UFUNCTION(BlueprintCallable)
	void Game_OnDifficultyButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Game_OnReturnButtonPressed();


	/** Fields for the pause menu */

	UFUNCTION(BlueprintCallable)
	void Pause_OnResumeButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Pause_OnControlsButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Pause_OnSettingsButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Pause_OnLoadButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Pause_OnReturnToMainButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Pause_OnExitButtonPressed();


	/** Fields for the controls menu */

	UFUNCTION(BlueprintCallable)
	void Controls_OnReturnButtonPressed();


	/** Fields for the video/audio/mouse settings menu */

	UFUNCTION(BlueprintCallable)
	void Video_OnWindowedModeButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnResolutionBoxChanged(FString NewResolution);

	UFUNCTION(BlueprintCallable)
	void Video_OnFramerateSliderChanged(float NewFramerate);

	UFUNCTION(BlueprintCallable)
	void Video_OnVSyncBoxChanged(bool NewValue);

	UFUNCTION(BlueprintCallable)
	void Video_OnPresetsButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnTexturesButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnDetailModeButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnShadowQualityButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnViewDistanceButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnAAModeButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnAAQualityButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnEffectsQualityButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnFoliageQualityButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnPostProcessingButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void Video_OnConfirmButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Video_OnCancelButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Video_OnVolumeDownButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Video_OnVolumeUpButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Video_OnMouseSensDownButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Video_OnMouseSensUpButtonPressed();


	/** Fields for the confirm load last save menu */

	UFUNCTION(BlueprintCallable)
	void Load_OnConfirmButtonPressed();

	UFUNCTION(BlueprintCallable)
	void Load_OnBackButtonPressed();


	/** Fields for the confirm return to main menu menu */

	UFUNCTION(BlueprintCallable)
	void ReturnMain_OnConfirmButtonPressed();

	UFUNCTION(BlueprintCallable)
	void ReturnMain_OnBackButtonPressed();


	/** Fields for the confirm exit to windows menu */

	UFUNCTION(BlueprintCallable)
	void ReturnWindows_OnConfirmButtonPressed();

	UFUNCTION(BlueprintCallable)
	void ReturnWindows_OnBackButtonPressed();


	/** Fields for the game settings menu */




	/** Fields for the first game start up menu */

	UFUNCTION(BlueprintCallable)
	void First_OnAudioDownButtonPressed();

	UFUNCTION(BlueprintCallable)
	void First_OnAudioUpButtonPressed();

	UFUNCTION(BlueprintCallable)
	void First_OnVideoCardQualityButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void First_OnDontKnowButtonPressed();

	UFUNCTION(BlueprintCallable)
	void First_OnContinueButtonPressed();


	/** Fields for game over screen */

	UFUNCTION(BlueprintCallable)
	void GameOver_OnLoadButtonPressed();

	UFUNCTION(BlueprintCallable)
	void GameOver_OnNewGameButtonPressed();

	UFUNCTION(BlueprintCallable)
	void GameOver_OnDifficultyButtonPressed();

	UFUNCTION(BlueprintCallable)
	void GameOver_OnMainButtonPressed();

	UFUNCTION(BlueprintCallable)
	void GameOver_OnQuitButtonPressed();



	/** Video settings helper functions */

	/* Assign preset on starting game for the first time */
	UFUNCTION(BlueprintCallable)
	void SetStartUpVideoPreset(int32 Preset);

	UFUNCTION(BlueprintCallable)
	void SetVideoPresets(int32 Preset);

	/* Apply video settings */
	UFUNCTION(BlueprintCallable)
	void ApplyVideoSettings(const TArray<int32> & NewSettings, const float NewFramerate, const FString & NewResolution, const float NewAudioVolume, const float NewMouseSensitivity);

	/* Populate VideoSettingsApplied with the video settings taken from the GameUserSettings.
	   This should be called every time the video settings menu is opened */
	UFUNCTION(BlueprintCallable)
	void PopulateVideoSettingsApplied();

	/* Initializes VideoSettings and VideoSettingsApplied array filling it with INT32_MAX */
	UFUNCTION(BlueprintCallable)
	void InitVideoSettingsArray(int32 Size);

	/* Populate the resolution combo box with all supported resolutions */
	UFUNCTION(BlueprintCallable)
	void InitResolutionComboBox();

	/* Updates resolution combo box value when the video menu is first opened */
	UFUNCTION(BlueprintCallable)
	void UpdateResolutionComboBox();


	/** Video settings fields */
	
	/* Array that holds most of the video settings that are currently applied
	Indexes:
	0 = Windowed/Fullscreen setting
	1 = VSnyc
	2 = Preset
	3 = Textures
	4 = Detail
	5 = Shadow
	6 = View distance
	7 = AA mode
	8 = AA quality
	9 = Effects
	10 = Foliage
	11 = Post processing
	*/
	UPROPERTY()
	TArray<int32> VideoSettingsApplied;

	/* Array that holds most of the video settings current menu values. These values 
	   may have not been applied.
		Indexes:
		0 = Windowed/Fullscreen setting
		1 = VSnyc
		2 = Preset
		3 = Textures
		4 = Detail
		5 = Shadow
		6 = View distance
		7 = AA mode
		8 = AA quality
		9 = Effects
		10 = Foliage
		11 = Post processing
	*/
	UPROPERTY()
	TArray<int32> VideoSettings;

	/* Holds enforced resolution*/
	FString Resolution;

	/* Holds the current menu value of screen resolution */
	FString ResolutionDirty;

	/* Holds the current enforced value of framerate limit */
	float FramerateLimit;

	/* Holds menu value of framerate limit */
	float FramerateLimitDirty;

	/* The volume when the video menu is opened */
	float AudioVolume;

	/* The audio volume in the video settings menu */
	UPROPERTY(BlueprintReadOnly)
	float AudioVolumeDirty;

	/* Mouse sensitivity enforced. This value should be retrieved by character when they begin play */
	float MouseSensitivity;

	/* Mouse sensitivity in menu */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseSensitivityDirty;

	/* How much damage the player takes */
	float DamageMultiplier;

	/* An array that holds how much the damage multiplier is for each difficulty level.
	   This should be populated in editor
	   0 =  Invulnerable difficulty level
	   1 = sight seeing difficulty level
	   2 = normal difficulty level
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<float> DifficultyMultipliers;


	/* Holds the preset values for low preset 
	Indexes:
	0 = Windowed/Fullscreen setting
	1 = VSnyc
	2 = Preset
	3 = Textures
	4 = Detail
	5 = Shadow
	6 = View distance
	7 = AA mode
	8 = AA quality
	9 = Effects
	10 = Foliage
	11 = Post processing
	*/
	UPROPERTY(EditDefaultsOnly)
	TArray<int32> LowPresets;

	/* Holds the preset values for medium preset 
	Indexes:
	0 = Windowed/Fullscreen setting
	1 = VSnyc
	2 = Preset
	3 = Textures
	4 = Detail
	5 = Shadow
	6 = View distance
	7 = AA mode
	8 = AA quality
	9 = Effects
	10 = Foliage
	11 = Post processing
	*/
	UPROPERTY(EditDefaultsOnly)
	TArray<int32> MediumPresets;

	/* Holds the preset values for high preset 
	Indexes:
	0 = Windowed/Fullscreen setting
	1 = VSnyc
	2 = Preset
	3 = Textures
	4 = Detail
	5 = Shadow
	6 = View distance
	7 = AA mode
	8 = AA quality
	9 = Effects
	10 = Foliage
	11 = Post processing
	*/
	UPROPERTY(EditDefaultsOnly)
	TArray<int32> HighPresets;

	/* Holds the preset values for very high preset 
	Indexes:
	0 = Windowed/Fullscreen setting
	1 = VSnyc
	2 = Preset
	3 = Textures
	4 = Detail
	5 = Shadow
	6 = View distance
	7 = AA mode
	8 = AA quality
	9 = Effects
	10 = Foliage
	11 = Post processing
	*/
	UPROPERTY(EditDefaultsOnly)
	TArray<int32> VeryHighPresets;


	/** Other Helper functions */
	
	/* Do everything needed to initialize this. Called by the game mode */
	void Initialize();

	/* Show main menu */
	void ShowMainMenu();

	/* Switch view to a random menu camera */
	void SwitchToMenuCamera();

	/* Fill menu camera array with all cameras */
	void PopulateMenuCameraArray();

	/* Show the first time starting the game screen */
	void ShowFirstTimeStartingGameScreen();


	/* Open pause menu. Should be called by player */
	void OnGamePaused();

	/* Start a new game */
	void StartNewGame();

	/* Continue setting up new game */
	void StartNewGame_2();

	/* Spawn the player character in the world. Retruns reference to them */
	class AFPS_ProjectCharacter * SpawnPlayerCharacter();

	/* Get control of character */
	void GetControlOfCharacter();

	/* Allow player input */
	void AllowPlayerInput();

	/* Shows game over screen */
	void ShowGameOverScreen();

	/* Load saved game */
	void LoadGame();

	/* Exit game and return to OS */
	void QuitGame();

	/* Hide all the widgets the player uses that are not menu widgets e.g. low health, game over color.
	   May not hide all widgets yet */
	void HidePlayersWidgets();

	/* Save video/game settings to storage */
	void SaveGameSettingsToStorage();

	/* Load game settings from file. This should be all graphical settings */
	void LoadGameSettingsFromStorage();

	/* Check if GameSettings is not equal to nullptr. If it is then assign it a value. 
	   Returns true if GameSettings is not equal to nullptr by the end of the function
	*/
	bool CheckGameSettings();

	/* Check if AA quality buttons need to be faded */
	void FadeAAQualityButtons();

	/* Returns what color a button should be */
	UFUNCTION(BlueprintCallable)
	FSlateColor GetButtonColor(const int32 SettingType, const int32 ButtonIndex);

	/* Returns color for first start up preset buttons */
	UFUNCTION(BlueprintCallable)
	FSlateColor GetStartupButtonColor(const int32 Index);

	/* Returns color game difficulty buttons in game settings shouls be */
	UFUNCTION(BlueprintCallable)
	FSlateColor GetDifficultyButtonColor(const int32 Index);


	/** Other fields */

	/* Array of menu cameras */
	UPROPERTY()
	TArray<class AMenuCamera*> MenuCameras;

	/* Timer handle for deciding when to switch menu camera */
	UPROPERTY()
	FTimerHandle MenuCameraTimerHandle;

	/* Timer handle to play camera fade out */
	UPROPERTY()
	FTimerHandle CameraFadeOutTimerHandle;

	/* Index of camera used in main menu */
	int32 MenuCameraIndex = 0;

	/* Play camera fade out */
	void PlayCameraFadeIn();

	/* Play camera fade in */
	void PlayCameraFadeOut();

	/* Blueprint of the player character to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AFPS_ProjectCharacter> BP_Player;

	/* Color of a highlighted button */
	FSlateColor HighlightedColor;

	/* Color of a unhighlighted button */
	FSlateColor UnhighlightedColor;

	/* Index of highlight video settings button in first start up menu */
	UPROPERTY(BlueprintReadWrite)
	int32 FirstStartUpPresetIndex = -1;

	/* A reference to game user settings */
	UPROPERTY()
	class UGameUserSettings *  GameSettings;

	/* Ok to remove loading screen on startup */
	bool bRemoveLoadingScreen;

	/* Restores players HUD */
	void RestoreHUD();

	/* True if opening movie is playing */
	bool bIsOpeningMoviePlaying;

	/* Plays opening movie */
	UFUNCTION()
	void PlayOpeningMovie();

	/* Called when opening movie has finished */
	UFUNCTION()
	void OnOpeningMovieFinished();

	/* Adds opening cutscene overlay to viewport, making sure it's Zorder is higher than cutscene widget */
	void AddCutsceneOverlayToViewport();

	/* Stop opening movie from playing */
	void StopOpeningMovie();

	/* Spawn all AI. Should be called when starting a new game or loading a game */
	UFUNCTION()
	void SpawnAI();

	/* Spawn things that always fall through the ground when loading the map, like weapon pickups 
	   and the flaming boulders */
	void SpawnPhysicsActors();

	/* Copy contents of array ToCopy to Clone. Both must be same size */
	void CopyArray(TArray<int32> & ToCopy, TArray<int32> & Clone);

	/* Set opacity of load game button in main menu */
	void SetLoadGameButtonOpacity();

	/* Music while in main menu */
	UPROPERTY(EditDefaultsOnly)
	TArray<class USoundBase *> MainMenuMusic;

	/* Reference to the main menu music */
	UPROPERTY()
	class UAudioComponent * MainMenuAudioComponent;
	
	/* Array of pointers to arrays of sounds. Convert EButtonType to a uint8 
	   to get the correct array os sounds.
	   0 = Select sounds
	   1 = Confirm sounds
	   2 = Cancel sounds
	   3 = Start new game sound
	   4 = load game sound
	   5 = resume game from pause sounds
	   */
	TArray<TArray <class USoundBase *> * > UISounds;

	// Initialize UISounds
	void InitUISounds();

	/* UI select option sounds */
	UPROPERTY(EditDefaultsOnly)
	TArray<class USoundBase *> UISelectSounds;

	/* UI confirm options sounds */
	UPROPERTY(EditDefaultsOnly)
	TArray<class USoundBase *> UIConfirmSounds;

	/* UI cancel options sounds*/
	UPROPERTY(EditDefaultsOnly)
	TArray<class USoundBase *> UICancelSounds;

	/* UI start new game sounds */
	UPROPERTY(EditDefaultsOnly)
	TArray<class USoundBase *> UIStartNewGameSounds;

	/* UI load game sounds */
	UPROPERTY(EditDefaultsOnly)
	TArray<class USoundBase *> UILoadGameSounds;

	/* Resume game button sounds */
	UPROPERTY(EditDefaultsOnly)
	TArray<class USoundBase *> UIResumeGameSounds;

	/* Plays a UI sound based on what button type was pressed */
	void PlayUISound(EButtonType Type);

	/* Fade out and stop main menu music */
	void FadeOutMainMenuMusic(float FadeDuration);

	/* Restore game sounds. Call when exiting main menu */
	void RestoreGameSounds();

	/* Adjust audio volume dirty keeping it between 0 and 1 */
	void AdjustAudioVolumeDirty(float Amount);

	/* The sound to play constantly in the first start up menu */
	UPROPERTY(EditDefaultsOnly)
	TArray<USoundBase *> FirstStartSound;

	/* Reference to audio component for first screen sound */
	UPROPERTY()
	class UAudioComponent * FirstAudioComponent;

	/* Adjust the volume of first screen sound after it has started playing */
	void UpdateFirstScreenSoundVolume();

	/* Adjust the volume of the menu music after it has started playing */
	void UpdateMenuMusicVolume();

	/* Update the volume of all ambient sounds in the world. Right now this is only 
	   the waterfall sound. Should be called whenever the volume is changed. */
	void UpdateAmbientSounds();


	/** Fields for playing the sniper cutscene */

	// Play sniper cutscene
	UFUNCTION(BlueprintCallable)
	void PlaySniperCutscene();

	// Called when sniper cutscene finishes
	UFUNCTION()
	void OnSniperCutsceneFinished();
	
	// Teleport player to where cutscene ends
	void TeleportPlayerForSniperCutscene();

	// Location to teleport player to after sniper cutscene
	UPROPERTY(EditDefaultsOnly)
	FVector SniperCutsceneEndLocation;

	// Rotation to teleport player to after sniper cutscene
	UPROPERTY(EditDefaultsOnly)
	FRotator SniperCutsceneEndRotation;

	// Spawn sniper bot
	void SpawnSniperBot();

	// BP for sniper bot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class ASniper_Bot> BP_SniperBot;

	// Location to spawn sniper bot at
	UPROPERTY(EditDefaultsOnly)
	FVector SniperBotSpawnLocation;

	// Reenable input after sniper cutscene
	void ReenableInput();


	/** Fields for the end of the level */

	// Play credits movie
	void PlayCreditsMovie();

	// Called when credits movie has finished
	void OnCreditsMovieFinished();


};
