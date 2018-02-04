// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Bullet.h"
#include "Damagable.h"
#include "DamagableBot.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Ver2_MouseoverDisplay.h"
#include "FPS_ProjectCharacter.generated.h"


/** Project code buglist:
	- Sometimes when entering a weapon pickups pickup range you cannot pick it up without leaving 
	  and coming back again

	- AIPatrol detonates twice if it loses all health while it has gotten within detonation range of player
*/

/** Project editor corrections:
	- Change the skylight to static before packaging
*/

/* 
	To do:
	- Add a component to player that is higher than capsule for homing missles to lock on to
	- Add mouse sensitivity option
	- Add audio volume option
	- Check what was in LevelOne level blueprint and make sure its implemented in BlankLevels blueprint
	- While editing landscapes skylight intensity was changed from 0.15 to 1.0. 0.5 looks decent
	- Implement effects when projectiles hit wood/metal etc
	- Make sure grenade launcher blast damages player and AI
	- Make sure Ver2_Bot launcher blast has an AoE effect and damages player and AI
	- Add muzzle flash to Ver2_Bot when firing rifle
	- Add mouseover health bars for AIPatrol and Advanced_Bot
	- Make Part2 foliage load soon after leaving camp
	- Make part1 foliage unload when entering point of no return leading up to sniper bot
	- Add atmospheric fog maybe
	- Make sure the Z limit is lowered when entering the sniper area
	- Dropped sniper rifle pickups have crazy physics - don't know what to do about that
	- Optional: make weapons less accurate when moving
	- Weapon pickups do not simulate physics when placed in world
	- Despawn all bots when passing point of no return
	- Advanced_Bots and possibly Ver2_Bots can shoot through the brick walls near the firing range
	- Overhead caution icon do not display when Ver_Bots goes to caution mode
	- Add sound
	- Make sure Ver2 firing range bots have patrol points to go to after they leave alert status
	- Make Ver2 launcher projectiles damage player and bots
	- Make damage player deals lower so bots take more damage from their own hits
	- Mouseover box displays Ver2_Bot info when mousing over their spawn location.
	  Maybe some components need to be attached to root component to avoid this
	- Make sure video/audio/sensitivity settings are loaded when the game starts. This will
	  require making another function in MySaveGame that loads those settings
	- Make sure item pickup sounds and reload/run/weapon switch sounds sound correct
	  i.e. they are attached to the player and moving around does not make them sound like
	  they're in the wrong location
	- Go through each of the 4 projectile types and make sure their hit decal arrays and 
	  hit particles arrays are both full to account for all physical material types
	- Make sure firing range ver2 bots have sounds too
	- Remove destroyed bots from the 3 arrays that are in the player tick raycast 
	  for mouseover displays
	- Make AIPatrol rotate gradually like Ver2 and Advanced bots
	- Rotatables could make the player continue looking at it for a certain amount of 
	  time before being able to look away
	- Clicking start new game while map has not loaded will cause player to fall through map
	- From what I can see AI are not spawning (except for idle guard) when a new game is started.
	  Maybe spawn them whenever a new game starts. They may be falling through the map, and 
	  so may physics actors
	- main menu cameras do not get view when returning to main menu from pause
	- Game was crashing when choosing low, medium or high presets on initial startup
	- When returning to main menu from pause the screen goes black and menu cameras do not 
	  tick
	- On load make foliage in part 2 set correct hidden in game value depending on where the game 
	  is loaded from
	- Make all bots be destroyed when exiting base after bunker event
	- Make sure part 2 foliage starts as hidden in game
	- Go through all fields of Ver2_bots/advanced bots/AI patrol - make sure the other BPs
	  all share the same values as the base BP
	- For Ver2 bots muzzle offsets need to be changed in non-base BPs
	- 2nd version of Ver2 bot firing range doesn't fire shots. Maybe clone the 1st firing range bot 
	  and change a few fields to make it use the closest target at the range
	- Make sure no framerate smoothing or fixed framerate is on in project settings
	- Make sure camera robot HUD message says about moving robot/player/speed boost
	- Make opening cutscene video the correct one
	- Make sure videos are in the right folder (possible duplicate task)
	- If AI falls below map (can happen from grenade launcher) then players mouseover arrays
	  code will throw error
	- Sound doesn't work correctly - I think it's just the ambient sound of the waterfall
	- Add UI sounds
	- *** Recently enabled two settings in the project settings under "Streaming"*** 
	- Add tooltips to all the menus
	- Add game description, name, thumbnail etc in project settings
	- Sniper bot crouches when leaving initial sniping location and should instead run straight
	  through the crouch trigger
	- Game appears to be  frozen/paused when starting a new game or loading a game from the
	  game over  screen

	- Test that waterfall makes no sound since its sound cannot be adjusted in game

*/

/* 
	Things to do right before packaging
	- Create the boulder rotatable, rocks rotatable, front gate moveable, back gate moveable and 
	  the idle gaurds gate moveable
	- Ver2 launcher projectile BP needs to be remade because collision comp has disappeared
	- Change the menu button from P to ESC
	- Make sure editor settings are at max
	- Unbind load from the "L" key
	- Remove any player starts except the first one
	- Make sure video files are in the correct location /Content/Movies I think they have to be in
	- increase delay for loading screen
	- make sure video settings are max

*/

/*
	More notes: 
	- Landscape1 coords: X = -25200, Y = -66028, Z = 100, Rotation = 0, Scale = 100

	- Player start location: (12491.998047 , 6334.5, -59.777222)
	- Player start rotation: (0, 0, 130.000137)

	Patrol point scheme
	- Ver2 Bots use Ver2 Points and use IDs 0, 1, 2 etc
	- AIPatrol use AI patrol points and use IDs 0, 1, 2 etc
	- Advanced Bot use AI patrol points and use IDs 101, 102, etc

	- Spawned bots from spawn point 1 use IDs 1001
	- Spawned bots from spawn point 2 use IDs 1002
	- Spawned bots from spawn point 3 use IDs 1003

	AIPatrol:
	- There is the base BP plus the BP for each of the 3 spawn points

	Advanced Bot:
	- There is the idle BP, there is a BP for the 1st patrol route and a BP for the 2nd patrol route
	- There is a BP for the idle guard
	- There is a BP for each of the 3 spawn points

	Ver2 Bot:
	- There is the base BP
	- There is a BP for the firing range version
	- There is a BP for each of the 3 spawn points

	Old KillZ value: -4180

*/

/*
	What to do:
	- check sound is working
	- remove radial force from grenade launcher
	- make game over screen show when falling in water
	- weapons do not drop when switching weapons. ACtually they do I think but they get picked up straight away
	- Advanced bots can shoot through walls

	- change pause binding from P to ESC
	- change skipcutscene binding to ESC

*/


USTRUCT(BlueprintType)
struct FHUDInfoText
{
	GENERATED_USTRUCT_BODY()

	// The text to display in the HUD
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Text;

	// The time in seconds the message is displayed in the HUD
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Time;

	// The amount of ammo displayed in the message currently (only relevant if the message is for ammo)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoAmount;

	// The message ID which is used for figuring out what type of ammo is being displayed.
	// -1 indicates the text field is empty
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoID = -1;

	
	

};

/* Struct for typewriter messages */
//USTRUCT()
struct FTypewriterMessage
{
	//GENERATED_USTRUCT_BODY()

	// Message to show
	//UPROPERTY()
	FString Message;

	// Delays between each letter
	//UPROPERTY()
	TArray<float> Delays;

	// Length of words
	//UPROPERTY()
	TArray<int32> WordLengths;

	// Time from when message finishes to removing it from HUD
	//UPROPERTY()
	float EraseTimer;

	// Priority of message since messages added around the same time may not play in the correct order
	// 0 = will play first, INT32_MAX = will play last
	//UPROPERTY()
	//int32 Priority;

	FTypewriterMessage()
	{

	}
	
	FTypewriterMessage(FString & Message1, TArray<float> & Delays1, TArray<int32> WordLengths1, float EraseTimer1)
	{
		Message = Message1;
		Delays = Delays1;
		WordLengths = WordLengths1;
		EraseTimer = EraseTimer1;
		//Priority = Priority1;
	}

};


class UInputComponent;

UCLASS(config=Game)
class AFPS_ProjectCharacter : public ACharacter, public IDamagable
{
	GENERATED_BODY()

public:

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

public:
	/** Gun mesh: 1st person view (seen only by self). Made public by me*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

public:
	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

private:
	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

public:
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

private:
	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

public:
	AFPS_ProjectCharacter();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay();

public:

	// Assigns value to game instance on begin play
	void SetupGameInstanceForBeginPlay();

	// Pause game and bring up pause menu
	void PauseGame();

	// Setup clamp for max up/down look angle on begin play
	void ClampPitchLookAngleOnBeginPlay();

	// Blueprint for the sniper trail during the sniper cutscene
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASniperCutsceneProjectile> BP_SniperCutsceneProjectile;
	
	// Third person mesh for when using the camera robot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* Mesh3P;

	// Arrow that floats above players head when using the camera robot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetComponent* FloatingArrow;

	// A function that checks if jumping is ok before actually jumping
	UFUNCTION(BlueprintCallable)
	void CheckJump();

	// Delay between switching weapons
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WeaponSwitchCooldown;
	
	// True if it's ok to switch weapons since there 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOkToSwitchWeapons = true;

	// Toggles the value of OkToSwitchWeapons
	UFUNCTION(BlueprintCallable)
	void MakeOkToSwitchWeaponsTrue();

	// True when a weapon switch has occured while reloading a weapon. This stops 
	// the reload from changing any ammo values
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasRecentlySwitchedWeapons;

	// Skeletal mesh for the auto rifle
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_GunAutoRifle;
	
	// Skeletal mesh for the grenade launcher
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_GunGrenadeLauncher;

	// Skeletal mesh for the sniper rifle
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UStaticMeshComponent* FP_GunSniperRifle;

	// A widget to draw an arrow above the player when using the camera drone
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* OverheadWidget;

	// For the aim animations: the original relative location of Mesh1P
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Mesh1POriginalLocation;

	// For the aim animations: the original relative rotation of Mesh1P
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Mesh1POriginalRotation;

	// Text to be displayed in the top left corner of the HUD. This is for things like error messages 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HUDText;
	
	// Create a HUD message
	UFUNCTION(BlueprintCallable)
	void CreateHUDMessage(FString & Text, bool bCanBeOverwritten = true, bool bShouldFlash = false);

	// If true then the current HUD message cannot be overwritten by a 
	// new one until ClearHUDText() is called
	bool bCanHUDTextBeOverwritten = true;

	// Reference to HUD
	UPROPERTY()
	UVer2_MouseoverDisplay* HUDWidget;

	// Reference to widget animation that makes HUD text flash
	UPROPERTY()
	UWidgetAnimation* HUDTextFlashAnimation;


	/** Fields for the typewriter text box that is used for things like displaying controls messages
	    or objectives */

	// The text to be displayed by the typewriter message box
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TypewriterText;

	// The full typewriter message that is waiting to be displayed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TypewriterFullText;

	// An array of the delays between each letter in the current message being printed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> TypewriterDelays;

	// An array that holds where each space is in TypewriterFullMessage. This is so the text box knows 
	// when to wrap words pre-emptivaly
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> TypewriterWordLengths;

	// The time the current typewriter message stays on the HUD after it has completed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TypewriterEraseTime;

	// The time in seconds between each letter being displayed by the typewriter
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TypewriterDefaultDelay;

	// The timer handle for handling when to add another letter
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle TypewriterTimerHandle;

	// Timer handle to control the block at the end flashing back and forth once the message is complete
	FTimerHandle TypewriterBlockFlashTimerHandle;

	// The index of TypewriterFullText that the typewriter is at at the moment.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TypewriterIndex;

	// How many characters can be on a line of the typewriter text box.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TypewriterLineLength;

	// True if a new typewriter message is waiting to be displayed
	bool bIsNewTypewriterMessageWaiting;

	// True if it is ok to make the block character at the end of the typewriter message flash. Different 
	// from bShouldTypewriterBlock Flash in that this is only true if the critical section is not being used.
	//bool bCanTypewriterBlockFlash;

	// What the current words index is
	int32 TypewriterWordIndex;

	// Current number of characters on the current line
	int32 TypewriterLineCharNum;

	// A space character
	const wchar_t Space = *TEXT(" ");

	// A block character
	UPROPERTY(BlueprintReadOnly)
	FString Block = TEXT("\u2588");

	// Controls accessing and modifying bShouldTypewriterBlockFlash and TypewriterText
	FCriticalSection TypewriterLock;

	// If a thread holds this then it can modify the typewriter message
	FCriticalSection TypewriterModifyLock;

	// True if the block at the end of the typewriter message should flash
	bool bShouldTypewriterBlockFlash;

	// Array of typewriter messages waiting to show
	TQueue<FTypewriterMessage *> TypewriterQueue;


	/* Typewriter Functions */

	/* Creates the typewriter message
	@param Message The message to show
	@param Delays An array of the delays between each letter.
	@param WordLengths An array that has the length of each word e.g. Index 0 = 3 implies the first word is of length 3
	@param EraseTimer When to remove the message from the HUD after it is completed
	*/
	UFUNCTION(BlueprintCallable)
	void CreateTypewriterMessage(FString & Message, TArray<float> Delays, TArray<int32> WordLengths, float EraseTimer);

	// Display next typewriter message in queue
	void DisplayNextTypewriterMessage();

	// Called by the timer handle. It adds NextTypewriterLetter on to the typewriter message
	void AppendTypewriterMessage();

	// A helper function that finds out how many words a message has and how long each word is
	UFUNCTION(BlueprintCallable)
	TArray<int32> FindWordLengths(FString & Input);

	// Timer handle for erasing the whole typewriter text after it has completed its message
	FTimerHandle TypewriterEraseTimerHandle;

	// Erases the typewriter message
	void ClearTypewriterMessage();

	// For timer handle: toggles ending block between a block and a space
	void ToggleEndingBlock();

	/** Reference to the video/audio settings menu */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	//UUserWidget* SettingsMenu;


	/** Fields for the 3 info text boxes for things like ammo pickup information */
	
	// For HUD info text: Have 3 text boxes in the HUD. Have 3 FStrings in this code
	
	// Text to be displayed in 1st HUD info textbox 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHUDInfoText HUDInfoText1;
	
	// Text to be displayed in 2nd HUD info textbox
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHUDInfoText HUDInfoText2;

	// Text to be displayed in 3rd HUD info textbox
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHUDInfoText HUDInfoText3;

	// Resets the values of a FHUDInfoText to values that represent no text in it
	UFUNCTION(BlueprintCallable)
	void ResetAHUDInfoText(FHUDInfoText & HUDInfoText);
	
	// Clear the HUDText
	UFUNCTION(BlueprintCallable)
	void ClearHUDText();
	
	// Updates the info HUD during tick
	UFUNCTION(BlueprintCallable)
	void UpdateHUDInfoOnTick(float DeltaTime);


	// Finds the highest message slot that doesn't have a message in it
	UFUNCTION(BlueprintCallable)
	int GetEmptyIndexForHUDInfo();

	/*
	// Creates a HUD warning message.
	//@Param Length The length of time in seconds the message should be displayed for. If 0.0f then indefinetly
	//@Param bShouldFlash Whether the message should flash or not
	//
	UFUNCTION(BlueprintCallable)
	void CreateHUDMessage(const FString & Text, const float Length, bool bShouldFlash);

	// A function that gets the weapon name from the weapon type uint8
	UFUNCTION(BlueprintCallable)
	FString GetWeaponTypeString(uint8 Type, float MessageLength);
	*/
	// A function called by the 2 functions above that returns the final HUD info message to be displayed
	// @Param NewMessage The new message to add onto the current displayed message
	// @Param Index What index of the players HUD array does this message go in. Each index represents a new line
	UFUNCTION(BlueprintCallable)
	void CreateFinalHUDInfoMessage(int32 Index, int32 AmmoAmount, float MessageLength);

	// Gets the message corrisponding to a message ID.
	UFUNCTION(BlueprintCallable)
	FString GetMessageFromID(int32 Index, int32 AmmoAmount, float MessageLength);
	
	// A function that copies the values of on FHUDInfoText to another
	UFUNCTION(BlueprintCallable)
	void CopyFHUDInfoText(FHUDInfoText & ToBeChanged, FHUDInfoText & ToBeCloned);
	
	/*
	// A function that removes the HUDInfoText
	UFUNCTION(BlueprintCallable)
	void ClearHUDInfoText();
	*/
	// A boolean that says whether or not HUDText should flash
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldHUDTextFlash;
	
	/* Whether the player is looking down weapon sights*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAiming;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperOverlay();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideSniperOverlay();

	// Toggle whether aiming down sight
	UFUNCTION(BlueprintCallable)
	void ToggleSniperScope(bool Value);


	/** Recoil fields */

	// Camera shake for firing laser rifle. Not used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UCameraShake> BP_LaserRifleCameraShake;

	// Camera shake for firing automatic rife. Not used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UCameraShake> BP_AutoRifleCameraShake;

	// Camera shake for firing grenade launcher. Not used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UCameraShake> BP_GrenadeLauncherCameraShake;

	// Camera shake for firing sniper rifle. Not used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UCameraShake> BP_SniperRifleCameraShake;

	// Plays a camera shake when firing weapon
	void PlayOnFireCameraShake();

	// Add recoil when firing
	void AddRecoilOnFire();

	// Curve for laser rifle recoil
	UPROPERTY(EditAnywhere)
	UCurveFloat* LaserRifleRecoilCurve;

	// Curve for auto rifle recoil
	UPROPERTY(EditAnywhere)
	UCurveFloat* AutoRifleRecoilCurve;

	// Curve for grenade launcher recoil
	UPROPERTY(EditAnywhere)
	UCurveFloat* GrenadeLauncherRecoilCurve;

	// Curve for sniper rifle recoil
	UPROPERTY(EditAnywhere)
	UCurveFloat* SniperRifleRecoilCurve;

	// Array that holds the min and max recoil amounts for each weapon. Make sure to populate this in the editor
	// 0 = laser rifle min, 1 = auto rifle min, 2 = grenade launcher min, 3 = sniper rifle min
	// 4 == laser rifle max, 5 = auto rifle max, 6 = grenade launcher max, 7 = sniper rifle max
	UPROPERTY(EditAnywhere)
	TArray<FRotator> RecoilAmounts;

	// The recoil amount of the currently equipped weapon
	FRotator CurrentRecoilAmount;

	// Timeline for recoil
	FTimeline RecoilTimeline;

	// Sets up recoil animation on begin play
	void SetupRecoilForBeginPlay();

	UFUNCTION()
	void HandleRecoilAnimationProgress(float Value);

	// Needed for recoil animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnTimelineFloat RecoilProgressFunction;

	// Update recoil animation on tick
	UFUNCTION()
	void UpdateRecoilAnimationOnTick(float DeltaTime);


	/** Muzzle flash fields */

	// Create muzzle flash for equipped weapon
	void CreateMuzzleFlash();

	// Array of muzzle flashes for each weapon. Populate through editor. Not 100% sure if this is true
	// but if you do not populate all indexes before inserting into an index then it will cause a crash
	// Index 0 = Laser rifle, 1 = auto rifle, 2 = grenade launcher, 3 = sniper rifle
	UPROPERTY(EditAnywhere, Category = "Muzzle Flashes")
	TArray<class UParticleSystem*> MuzzleFlashes;

	// Holds muzzle locations for each weapon. Populate through editor
	// Index 0 = Laser rifle, 1 = auto rifle, 2 = grenade launcher, 3 = sniper rifle
	UPROPERTY(EditAnywhere)
	TArray<FVector> MuzzleLocations;

	// Change muzzle location. Call when switching or picking up new weapon
	// @ param Type The ID of the weapon that you want the muzzle location to switch to
	void ChangeMuzzleLocation(uint8 Type);

	// Removes scope picture and resets aim animation to start. Called when hit by a sniper rifle shot
	void KnockOutOfScope();

	// Camera shake for when knocked out of scope with sniper rifle. Used when hit by another sniper shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UCameraShake> BP_SniperRifleScopeLossCameraShake;

	/* Take damage, not implemented yet*/
	UFUNCTION(BlueprintCallable)
	virtual bool ReceiveDamage(float Amount) override;

	/* A function that should be called every tick that makes health increase if it is recharging */
	UFUNCTION(BlueprintCallable)
	void HandleHealthForTick(float DeltaTime);

	/* Amount of health, goes from 0 - 100*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth;

	/* Max health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	// The amount of recent damage the player has taken
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecentDamage;

	// How long recent damage should be considered recent. This is for the health bar 
	// on the HUD displaying yellow blocks for recent damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecentDamageTime;

	// After RecentDamageTime has passed, this variable is how long the yellow block 
	// will fade from the HUD (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecentDamageFadeTime;

	// A function that sets the value of recent damage to zero and makes
	UFUNCTION(BlueprintCallable)
	void ResetRecentDamage();

	// Here is where the recent damage that has been there for RecentDamageTime seconds is assigned. 
	// It will be faded out after RecentDamageFadeTime seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FadingRecentDamage;

	// A timer handle for handling how long recent damage stays recent
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle RecentDamageTimerHandle;

	// A function implemented in blueprints that updates the length of the recent damage bar
	// when damage is taken
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AddToRecentDamageBar(float Damage);

	/* A function that should be called when the players health falls to zero. 
	   It is called by ReceiveDamage already */
	UFUNCTION(BlueprintCallable)
	void GameOver();

	/* Shows the game over screen */
	void ShowGameOverScreen();

	// Widget that changes color of screen on game over
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UVer2_MouseoverDisplay> BP_GameOverScreenColor;

	// The Game over fade color widget once it has been created
	UPROPERTY()
	UVer2_MouseoverDisplay * GameOverColorWidget;

	/* True if a cutscene is being shown so the HUD does not need to be drawn*/
	UPROPERTY(EditAnywhere)
	bool bIsShowingCutscene = false;

	/* Get whether a cutscene is playing or not*/
	UFUNCTION(BlueprintCallable)
	bool GetShowingCutscene();

	/* Change whether a cutscene is playing or not*/
	UFUNCTION(BlueprintCallable)
	void SetShowingCutscene(bool value);
	
	/* To be called from blueprints by a trigger volume actor. Does everything required 
	to play the sniper start cutscene 
	@param StartViewTarget The actor to blend to at the start of the cutscene
	*/
	UFUNCTION(BlueprintCallable)
	void PlaySniperStartCutscene(AActor * StartViewTarget);

	/* True if sniper cutscene has already played */
	bool bHasSniperCutscenePlayed;

	/* The amount of time to blend from the position the cutscene is triggered to where 
	the cutscene starts */
	UPROPERTY(EditAnywhere)
	float SniperCutsceneStartBlendTime;

	/* Actually play the sniper cutscene. Separate function as it needs to be called by 
	a timer handle */
	void ContinueSniperCutscene();

	// Sequence player for sniper start cutscene
	UPROPERTY()
	class ULevelSequencePlayer* SequencePlayer;

	// Sequence assets for sniper start cutscene
	UPROPERTY(EditAnywhere)
	class ULevelSequence* SniperCutsceneSequenceAsset;

	// Play blend for end of sniper cutscene
	void PlaySniperCutsceneEndBlend();

	/* The amount of time to blend from the position the curscene ends to where
	the player will be when they get control again */
	UPROPERTY(EditAnywhere)
	float SniperCutsceneEndBlendTime;

	/* The blend target for the end of the sniper cutscene. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SniperCutsceneEndBlendTarget;

	// Re-enables input from character.
	void EnablePlayerInput();

	// Make the sniper bot fire at the player during the cutscene
	UFUNCTION(BlueprintCallable)
	void SniperFireDuringCutscene();


	/* A function that should be called in BeginPlay(). It adjusts mesh visibility for motion controls
	   and is just code that was there when the project was first created. */
	UFUNCTION(BlueprintCallable)
	void AdjustForMotionControls();

	/* A function that should be called in BeginPlay(). It sets up all the weapon ammo values 
	   and assigns the players values such as rate of fire, damage for the starting weapon */
	UFUNCTION(BlueprintCallable)
	void SetupWeaponValuesForBeginPlay();

	/* A function that should be called in BeginPlay(). It attaches all weapon meshs to the character SK,
	   scales/translates/rotates some of them and makes all but one invisible */
	UFUNCTION(BlueprintCallable)
	void AttachWeaponMeshesForBeginPlay();

	/* Changes the mesh of the equipped weapon*/
	UFUNCTION()
	void ChangeWeaponMesh(uint8 Type);
	
	/* Holds the info for the 4 ammo values of each weapon. Use a getter
	* or adjuster function to modify them such as 
	* AdjustCurrentClipAmmo(uint8 WeaponID, int Value) or
	* GetMaxNonClipAmmo(uint8 WeaponID)
	* WeaponAmmoValues[0] = CurrentClipAmmo for WeaponID 0
	* WeaponAmmoValues[1] = CurrentNonClipAmmo for WeaponID 0
	* WeaponAmmoValues[2] = MaxClipAmmo for WeaponID 0
	* WeaponAmmoValues[3] = MaxNonClipAmmo for WeaponID 0
	* WeaponAmmoValues[4] = CurrentClipAmmo for WeaponID 1
	* WeaponAmmoValues[5] = CurrentNonClipAmmo for WeaponID 1 
	* etc
	*/
	UPROPERTY()
	TArray<int> WeaponAmmoValues;

	// Holds the rate of fire for each weapon
	// Index 0 = Laser rifle, 1 = auto rifle, 2 = grenade launcher, 3 = sniper rifle
	UPROPERTY()
	TArray<float> RatesOfFire;

	// Holds whether the weapon is automatic
	// Index 0 = Laser rifle, 1 = auto rifle, 2 = grenade launcher, 3 = sniper rifle
	UPROPERTY()
	TArray<bool> AutomaticStatus;

	// Holds camera shakes for when firing weapon
	// Index 0 = Laser rifle, 1 = auto rifle, 2 = grenade launcher, 3 = sniper rifle
	UPROPERTY(EditAnywhere)
	TArray< TSubclassOf<class UCameraShake> > WeaponCameraShakes;

	// Holds fire sound effects for each weapon. Sould be populated from editor
	// Index 0 = Laser rifle, 1 = auto rifle, 2 = grenade launcher, 3 = sniper rifle
	// Index 1 for auto rifle does will not be played. Auto rifle has its own array 
	// of sounds it will play at random
	UPROPERTY(EditAnywhere, Category = "Sound Effects")
	TArray<class USoundBase*> WeaponFireSoundEffects;


	// Array of sounds to play at random when firing auto rifle
	UPROPERTY(EditAnywhere, Category = "Sound Effects")
	TArray<class USoundBase*> AutoRifleFireSounds;

	// Plays a sound at random from an array of sounds, The array passed in must be populated in 
	// every index less than its size i.e. no gaps in it. UMyStatics has a method that will do this
	// so uses of this function could be replaced by that instead
	void PlaySoundAtRandom(TArray<class USoundBase*> & Sounds);

	// An offset that changes where sounds are played
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound Effects")
	FVector MuzzleSoundOffset;

	// Switch to other weapon in inventory
	UFUNCTION(BlueprintCallable)
	void SwitchWeapons();

	// Play weapon switch animation montage
	UFUNCTION(BlueprintCallable)
	void PlayWeaponSwitchAnimation(float Playrate = 1.f);

	/* The other weapon the player does not have as equipped. A value of 255 means
	    they do not have another weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 OtherWeapon;

	/* Get the current clip ammo for the weapon with ID WeaponID*/
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	int GetCurrentClipAmmo(uint8 WeaponID);

	/* Get the current non clip ammo for the weapon with ID WeaponID*/
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	int GetCurrentNonClipAmmo(uint8 WeaponID);

	/* Get the max clip ammo for the weapon with ID WeaponID*/
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	int GetMaxClipAmmo(uint8 WeaponID);

	/* Get the max non clip ammo for the weapon with ID WeaponID*/
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	int GetMaxNonClipAmmo(uint8 WeaponID);

	/* Set the current clip ammo for the weapon with ID WeaponID to NewAmount*/
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	void AdjustCurrentClipAmmo(uint8 WeaponID, int Amount);

	/* Set the current non clip ammo for the weapon with ID WeaponID to NewAmount*/
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	void AdjustCurrentNonClipAmmo(uint8 WeaponID, int Amount);

	/* Set the max clip ammo for the weapon with ID WeaponID to NewAmount*/
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	void AdjustMaxClipAmmo(uint8 WeaponID, int Amount);

	/* Set the max non clip ammo for the weapon with ID WeaponID to NewAmount*/
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	void AdjustMaxNonClipAmmo(uint8 WeaponID, int Amount);

	/* Reloads the equipped weapon */
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	void Reload();

	/* Reload delay timer handle*/
	FTimerHandle ReloadDelayTimerHandle;

	/* True if reloading */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponSystem)
	bool bIsReloading = false;

	/* Call it right before the reload animation. Currently switching weapon 
	   is not possible while reloading */
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	void StartReloading();

	/* Call it when you finish reloading. This will then update the clip and non clip
	   ammo values for the HUD */
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	void StopReloading();

	/* The type of the currently equipped weapon 
	*  0 = Laser rifle, 1 = Auto Rifle, 2 = grenade launcher, 3 = sniper rifle
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponSystem)
	uint8 EquippedWeapon;

	/* True: The weapon is an automatic weapon and holding the fire button
	(usually the left mouse button) will allow constant firing of it
	False: The weapon is a semi automatic weapon and the fire button
	cannot be held down to constantly fire it */
	UPROPERTY(EditAnywhere)
	bool bEquippedWeaponIsAutomatic;

	/* True if the left mouse button is being held down meaning an 
	   automatic weapon should keep firing */
	UPROPERTY(EditAnywhere, Category = WeaponSystem)
	bool bHoldingTrigger = false;

	/* A function to set the value of bHoldingTrigger */
	UFUNCTION(BlueprintCallable, Category = WeaponSystem)
	void ReleasedTrigger();

	/* Resets the player's ability to fire */
	void ResetFire();

	/* True if picking up a weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponSystem)
	bool bIsPickingUpWeapon;

	/* The damage the equipped weapon deals. Does nothing - damage is determined by projectile that is spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponSystem)
	float WeaponDamage;

	/* The time between shots in seconds for the equipped weapon AKA rate of fire.
	*  Lower values mean the weapon can fire faster
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponSystem)
	float WeaponRateOfFire;

	TArray<FString> Inventory;
	
	// True if the player holds a keycard. Inventory isn't working so using this instead
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasKeyCard;


	/** Crouch fields */

	// Max maove speed while crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrouchMoveSpeed;

	// The half height of the capsule without crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StandingHalfHeight;

	// The half height of the capsule when crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrouchingHalfHeight;

	// Called when LCTRL is pressed
	UFUNCTION(BlueprintCallable)
	void LCTRLPressed();

	// Called when LCTRL is released
	UFUNCTION(BlueprintCallable)
	void LCTRLReleased();

	// Makes the player crouch
	UFUNCTION(BlueprintCallable)
	void CrouchPlayer();

	// Makes the player uncrouch
	UFUNCTION(BlueprintCallable)
	void UnCrouchPlayer();

	// True if the player is crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCrouching;

	// Tiemline for crouching
	FTimeline CrouchTimeline;

	// A timer handle that tries to uncrouch the player every 0.05 seconds when LCTRL is not being held down
	FTimerHandle UnCrouchTimerHandle;

	UFUNCTION(BlueprintCallable)
	void HandleCrouchProgress(float Value);

	// Progresses crouch animation on tick
	UFUNCTION(BlueprintCallable)
	void HandleCrouchAnimOnTick(float DeltaTime);

	// Does setup stuff for crouch animation on begin play
	UFUNCTION(BlueprintCallable)
	void SetupCrouchAnimForBeginPlay();

	// Curve float reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* CrouchAnimCurve;

	/** Base turn rate, in deg/sec.  scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AProjectile> ProjectileClass;	// Changed from <class AFPS_ProjectProjectile>

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire a automatic weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** AnimMontage to play each time we fire a semi automatic weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimationNoLoop;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	bool bIsPickingUp = false;	// This is used by class Fire and one other class
	bool bIsInteracting = false;

	int32 PlayerXp = 0;

	/* Animation montage for switching weapons to a grenade launcher */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* GrenadeEquipAnimation;

	/* Animation montage for switching weapons to a rifle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* RifleEquipAnimation;

	// HUD variables/functions
	/* Accessor function for intial stamina*/
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetInitialStamina();

	/* Accessor function for current stamina*/
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetCurrentStamina();

	/** Updates the players current stamina 
    *  @param Stamina This is the amount to change the players stamina by. It can be either positive or negative
	*/
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void UpdateCurrentStamina(float Stamina);

	// True if sprinting
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSprinting;

	/* Makes the character sprint */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void Sprint();

	/* Makes the character stop sprinting */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StopSprinting();

	// A function that is called whenever the "W" button is released to help handle when sprinting is allowed
	UFUNCTION(BlueprintCallable)
	void W_Released();

	// A function that is called whenever the "A" button is released to help handle when sprinting is allowed
	UFUNCTION(BlueprintCallable)
	void A_Released();

	// A function that is called whenever the "D" button is released to help handle when sprinting is allowed
	UFUNCTION(BlueprintCallable)
	void D_Released();

	// True if the "W" button is pressed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWantsToMoveForward;

	// True if the "A" button is pressed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWantsToMoveLeft;

	// True if the "D" button is pressed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWantsToMoveRight;

	/* False if the stamina recharge delay is in place*/
	bool bStaminaRechargeDelayTimerHandleIsRunning = false;

	// The time after a weapon switch that the player can fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeaponSwitchFiringTime;

	// False if the player has switched weapons recently and is waiting to be able to fire again
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanFireAfterWeaponSwitch;

	/* The rate at which stamina recharges */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRechargeRate;

	/* The rate at which stamina is used when sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaDepletionRate;

	/* Called when stamina can recharge again*/
	void AllowStaminaToRecharge();

	// Delay in seconds from when damage is taken to when health starts to recharge
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthRechargeDelay;

	// The rate at which health recharges
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthRechargeRate;

	// True if health is allowed to recharge
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsHealthRecharging = true;

	// Called when timer handle finishes. Makes IsHealthRecharging true.
	UFUNCTION(BlueprintCallable)
	void HealthCanRecharge();

	// Timer handle for checking when health can recharge
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTimerHandle HealthRechargeTimerHandle;

	// Reload animation for the auto rifle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* AutoRifleReloadAnimation;

	// Reload animation for the grenade launcher
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* GrenadeLauncherReloadAnimation;

	// Stops the fire animation from playing. Needed for auto rifle fire animatiom asset which loops
	UFUNCTION(BlueprintCallable)
	void StopFireAnimation();

	// Timer handle for checking when to stop auto rifle fire animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FTimerHandle StopFireAnimationTimerHandle;

	// Animation instance for the fire animation. Moved it from inside OnFire()
	// but I ain't moving it back.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimInstance* FireAnimInstance;

	// Animation instance for reload animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimInstance* ReloadAnimInstance;

	// The global time dilation that is gotten on every tick
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GlobalTimeDilation = 1.f;

	
	/** Weapon aiming fields */

	// Move speed while aiming
	UPROPERTY(EditAnywhere)
	float AimMoveSpeed;

	// Target rotation for auto rifle part 1 relative to original rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aiming)
	FRotator AutoRifleAimRotationPart1;

	// Target location for auto rifle part 1 relative to original location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aiming)
	FVector AutoRifleAimLocationPart1;

	// Target rotation for auto rifle part 2 relative to part 1 rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aiming)
	FRotator AutoRifleAimRotationPart2;

	// Target location for auto rifle part 2 relative to part 1 location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aiming)
	FVector AutoRifleAimLocationPart2;

	// Target rotation for sniper rifle part 1 relative to original rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aiming)
	FRotator SniperRifleAimRotationPart1;

	// Target location for sniper rifle part 1 relative to original location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aiming)
	FVector SniperRifleAimLocationPart1;

	// Target rotation for sniper rifle part 2 relative to part 1 rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aiming)
	FRotator SniperRifleAimRotationPart2;
	
	// Target location for sniper rifle part 2 relative to part 1 location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aiming)
	FVector SniperRifleAimLocationPart2;

	// A function that should be called near the end of BeginPlay() to input 
	// the roation/translation values for the weapon aim animations
	UFUNCTION(BlueprintCallable)
	void SetupWeaponAimParameters();

	// A function that should be called every tick. It will progress the aim animation 
	// unless the player is reloading, switched weapons etc.
	UFUNCTION(BlueprintCallable)
	void UpdateAimAnimStatus(float DeltaTime);

	// Holds the initial rotations for the weapon aim animation
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<FRotator>WeaponAimRotInitial;

	// Holds the initial locations for the weapon aim animation
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<FVector>WeaponAimLocInitial;

	// Holds the initial rotations for the weapon aim animation
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<FRotator>WeaponAimRotTarget;

	// Holds the initial rotations for the weapon aim animation
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<FVector>WeaponAimLocTarget;

	// Holds the initial rotations for the weapon aim animation for its second part
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<FRotator>WeaponAimRotTargetPart2;

	// Holds the initial rotations for the weapon aim animation for its second part
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<FVector>WeaponAimLocTargetPart2;

	// An Array that holds the timeline roataton values for each weapon
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<FTimeline>WeaponRotTimelines;

	// An Array that holds the timeline location values for each weapon
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<FTimeline>WeaponLocTimelines;

	// An Array that holds the timeline roataton values for each weapon for the part 2 animation
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<FTimeline>WeaponRotTimelinesPart2;

	// An Array that holds the timeline location values for each weapon for the part 2 animation
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<FTimeline>WeaponLocTimelinesPart2;

	// An Array of anim curves for rotation for part 1
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<UCurveFloat*>RotationAnimInstances;

	// An Array of anim curves for location for part 1
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<UCurveFloat*>LocationAnimInstances;
	
	// An Array of anim curves for rotation for part 2
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<UCurveFloat*>RotationAnimInstancesPart2;

	// An Array of anim curves for location for part 2
	// 0: laser rifle, 1: auto rifle, 2: Grendade launcher, 3: sniper rifle
	UPROPERTY()
	TArray<UCurveFloat*>LocationAnimInstancesPart2;


	// Function called when the RMB is pressed, and should start moving the weapon to the aim position
	UFUNCTION(BlueprintCallable)
	void RMBPressed();

	// Function called when the RMB is released, and should put the weapon back to a non aim position
	UFUNCTION(BlueprintCallable)
	void RMBReleased();

	// True if the character wants to aim i.e the RMB is pressed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWantsToAim;

	// Needed by animations
	UFUNCTION()
	void HandleAimAnimationProgress(float Value);

	// A function that should be called whenever the equipped weapon changes. 
	// It assigns the correct aim animation values to CurrentRotAimAnim and CurrentLocAimAnim 
	// and adjusts the vector/rotator values for the animation
	UFUNCTION(BlueprintCallable)
	void AssignAimAnimsOnWeaponChange(uint8 WType);

	// Animation curve for translation float reference for aiming with the laser rifle
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* LaserRifleAnimCurveLocation;

	// Animation curve for rotation float reference for aiming with the laser rifle
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* LaserRifleAnimCurveRotation;

	// Animation curve for translation float reference for aiming with the laser rifle for part 2
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* LaserRifleAnimCurveLocationPart2;

	// Animation curve for rotation float reference for aiming with the laser rifle for part 2
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* LaserRifleAnimCurveRotationPart2;

	// Animation curve for rotation float reference for aiming with the auto rifle
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* AutoRifleAnimCurveRotation;

	// Animation curve for translation float reference for aiming with the auto rifle
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* AutoRifleAnimCurveLocation;

	// Animation curve for rotation float reference for aiming with the auto rifle for part 2
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* AutoRifleAnimCurveRotationPart2;

	// Animation curve for translation float reference for aiming with the auto rifle for part 2
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* AutoRifleAnimCurveLocationPart2;

	// Animation curve for rotation float reference for aiming with the grenade launcher
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* GrenadeLauncherAnimCurveRotation;

	// Animation curve for translation float reference for aiming with the grenade launcher
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* GrenadeLauncherAnimCurveLocation;

	// Animation curve for rotation float reference for aiming with the auto rifle for part 2
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* GrenadeLauncherAnimCurveRotationPart2;

	// Animation curve for translation float reference for aiming with the auto rifle for part 2
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* GrenadeLauncherAnimCurveLocationPart2;

	// Animation curve for rotation float reference for aiming with the Sniper rifle
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* SniperRifleAnimCurveRotation;

	// Animation curve for location float reference for aiming with the Sniper rifle
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* SniperRifleAnimCurveLocation;

	// Animation curve for rotation float reference for aiming with the auto rifle for part 2
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* SniperRifleAnimCurveRotationPart2;

	// Animation curve for translation float reference for aiming with the auto rifle for part 2
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* SniperRifleAnimCurveLocationPart2;

	// Initial rotation before aiming
	FRotator LaserRifleInitialRotation;

	// Target rotation for when aiming
	FRotator LaserRifleTargetRotation;

	// Initial rotation before aiming
	FVector LaserRifleInitialLocation;

	// Target rotation for when aiming
	FVector LaserRifleTargetLocation;

	// Points to the current rotation aim animation of the equipped weapon.
	FTimeline CurrentRotAimAnim;

	// Points to the current location aim animation of the equipped weapon.
	FTimeline CurrentLocAimAnim;

	// Another timeline for current rotation of the 2nd animation
	FTimeline CurrentRotAimAnimPart2;

	// Another timeline for the current location of the 2nd animation
	FTimeline CurrentLocAimAnimPart2;

	// These 6 variables hold the correct vector/rotator values for the equipped weapons aim animation.
	// Should really be UPROPERTYs for garbage collection
	FRotator*	CurrentInitialRot;
	FVector*	CurrentInitialLoc;
	FRotator*	CurrentTargetRot;
	FVector*	CurrentTargetLoc;
	FRotator*	CurrentTargetRotPart2;
	FVector*	CurrentTargetLocPart2;

	// Timeline for location to make animation play
	FTimeline LaserRifleTimelineLocation;

	// Timeline for rotation to make animation play
	FTimeline LaserRifleTimelineRotation;

	// Timeline for location to make animation play for part 2
	FTimeline LaserRifleTimelineLocationPart2;

	// Timeline for rotation to make animation play for part 2
	FTimeline LaserRifleTimelineRotationPart2;

	//Timeline for the location to make animation play
	FTimeline AutoRifleTimelineLocation;

	// Timeline for rotation to make animation play
	FTimeline AutoRifleTimelineRotation;

	// Timeline for rotation for part 2
	FTimeline AutoRifleTimelineRotationPart2;

	// Timeline for location for part 2
	FTimeline AutoRifleTimelineLocationPart2;

	// Timeline for location to make animation play
	FTimeline GrenadeLauncherTimelineLocation;

	// Timeline for rotation to make animation play
	FTimeline GrenadeLauncherTimelineRotation;

	// Timeline for location to make animation play for part 2
	FTimeline GrenadeLauncherTimelineLocationPart2;

	// Timeline for rotation to make animation play for part 2
	FTimeline GrenadeLauncherTimelineRotationPart2;

	//Timeline for the location to make animation play
	FTimeline SniperRifleTimelineLocation;

	// Timeline for rotation to make animation play
	FTimeline SniperRifleTimelineRotation;

	// Timeline for location to make animation play for part 2
	FTimeline SniperRifleTimelineLocationPart2;

	// Timeline for rotation to make animation play for part 2
	FTimeline SniperRifleTimelineRotationPart2;

	// A boolean that says whether the aim animation is on the 1st or 2nd animation. 
	// Right now this is only used for debugging
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAimAnim1st;

	// A function that resets Mesh1P back to it's original value. Values have been taken from the editor
	UFUNCTION(BlueprintCallable)
	void ResetMesh1P();

	// Needed for the aim animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnTimelineFloat ProgressFunction;

	// Using another one for the second animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnTimelineFloat ProgressFunctionPart2;

	// A function for progressing the 2nd animation
	UFUNCTION(BlueprintCallable)
	void HandleAimAnimationProgressPart2(float Value);

	/** Camera robot fields */

	// Reference to camera robot blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACameraRobot> BP_CameraRobot;

	// If nullptr then camera robot currently is not being used
	class ACameraRobot* CameraRobot;

	// Called when camera robot key binding is pressed
	void ZeroKeyPressed();
	
	// Tries to spawn camera robot if one is not already active. Returns true if successful
	bool SpawnCameraRobot();

	// Makes third person mesh visible, hide first person mesh, and also makes it ok to destroy camera robot
	void MakeMesh3PVisible();

	// Make the arrow above the players head visible
	void MakeOverheadArrowVisible();

	// True if using camera robot
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsUsingCameraRobot;

	// if true then destroying camera robot is ok
	bool bCanDestroyCameraRobot;

	// Destroy camera robot. Returns true if successful
	bool DestroyCameraRobot();

	// Make the arrow above the players head invisible
	void MakeOverheadArrowInvisible();

	// Rotate overhead arrow to face camera robot on tick
	void RotateOverheadArrowOnTick(float DeltaTime);

	// Offset of overhead arrow icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector OverheadIconOffset;

	// Make overhead icon not visible on BeginPlay
	void HideOverheadIconOnBeginPlay();

	// A reference to the players controller
	class APlayerController* PlayerController;

	// Move camera robot forwards
	void MoveCameraRobotForward(float Value);

	// Move camera robot right
	void MoveCameraRobotRight(float Value);

	// Move the player forward while using the camera robot
	void MoveForwardWhileUsingCameraRobot(float Value);

	// Move the player right while using the camera robot
	void MoveRightWhileUsingCameraRobot(float Value);

	// Turn camera robot
	void TurnCameraRobot(float Rate);

	// Make camera robot look up/down
	void LookUpCameraRobot(float Rate);

	// A function that checks if using camera robot before rotating player from X mouse movement
	void CheckBeforeAddingYawInput(float Rate);

	// A function that checks if using camera robot before rotating player from Y mouse movement
	void CheckBeforeAddingPitchInput(float Rate);


	/** Fields for the enemy mouseover displays */

	// Time it takes in seconds mousing over an enemy before their mouseover display shows.
	// The rate at which it fades in is set in each IDamagableBot
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseoverDisplayTime;

	// Time it takes not mousing over and enemy for their display to start fading out. The rate 
	// at which it fades out is set in each IDamagableBot
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseoverDisplayFadeOutTime;

	// An array of all the bots that the player has recently moused over
	UPROPERTY()
	TArray<ACharacter*> MouseoverBots;

	// An array holding how long the mouse has been over the bot
	UPROPERTY()
	TArray<float> MouseoverBotsMouseoverTime;

	// A array that holds how long until the mouseover display should start fading out
	UPROPERTY()
	TArray<float> MouseoverBotsFadeTime;


	/** Fields for the low health flashing screen widget */

	// Editor field for BP of low health widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> BP_LowHealthWidget;

	// The widget that makes the screen flash red when health is low
	UPROPERTY()
	class UVer2_MouseoverDisplay* LowHealthWidget;

	// This is a percentage from 0 to 1. 0.4 = 40%
	// When health percentage drops below this threshold the screen will flash red until 
	// health recovers past this point again
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LowHealthPercentage;

	// A reference to the widget animation used to make the screen flash.
	UPROPERTY()
	class UWidgetAnimation* LowHealthWidgetAnimation;

	// Assign the widget animation of the low health widget to LowHealthWidgetAnimation.
	// Should be called in begin play
	void SetupLowHealthAnimationOnBeginPlay();

	// Makes screen flash red on the border because of low health
	void ShowLowHealthWarning();

	// Stops screen from flashing red from low health
	void HideLowHealthWarning();

	// True if low health animation is playing
	bool bIsLowHealthAnimPlaying;


	/** Save and load game fields */

	// Called when "L" is pressed and loads the game. For debugging purposes only
	void LPressed();

	// Save the game
	UFUNCTION(BlueprintCallable)
	void SaveGame();

	// Load a game
	UFUNCTION(BlueprintCallable)
	void LoadGame();

	// Switch weapon mesh, adjust rate of fire etc
	UFUNCTION(BlueprintCallable)
	void AdjustWeaponValuesOnLoad();

	// Adjust many other variables on load such as bIsSprinting
	UFUNCTION(BlueprintCallable)
	void AdjustValuesOnLoad();

	// Destroy then respawn all AI
	UFUNCTION(BlueprintCallable)
	void ResetAIOnLoad();


	/** Fields for certain events that have occured in the game */
	
	// True if has used computer in bunker
	bool bHasAccessedComputer;

	// True if has defeated all AI that spawned after computer event
	bool bHasDefendedBunker;

	// True if the sniper bot has been defeated
	bool bHasDefeatedSniperBot;

	// Called when the sniper bot has been defeated
	void OnSniperBotDefeated();

	// BP for Typewriter message to play when defeated sniper bot
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATutorialMessage> BP_EndOfLevelMessage;

	// Play credits
	void PlayCredits();


	// The number of bots that need to be defeated defending the bunker 
	// before the gate will open
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumBotsDefeatedThreshold;

	// Number of bots defeated while dedending bunker
	int32 NumBotsDefeated;

	// Function to increment num bots defeated and check if they are over the threshold
	void IncrementNumBotsDefeated();

	// BP for Typewriter message to play when finished defending bunker
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATutorialMessage> BP_BunkerDefendSuccessMessage;

	// Plays bunker defend success message
	void PlayBunkerDefendSuccessMessage();


	/** Fields for things to do when the game starts up */

	// A reference to the game instance
	UPROPERTY()
	class UMyGameInstance * GameInstance;


	/** Fields foe the TAB controls menu */

	// Reference to in TAB controls menu BP
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UUserWidget> BP_ControlsMenu;

	// Reference to TAB control menu widget
	UPROPERTY()
	class UUserWidget * TABControlsMenu;

	// Called when TAB is pressed
	void OnTABPressed();

	// Called when TAB is released
	void OnTABReleased();

	// True if TAB is being held down
	bool bIsTABPressed;

	// Setup TAB controls menu on begin play
	void SetupTABControlsMenuOnBeginPlay();

	// Hanlde the fading in/out of the TAB controls menu on tick
	void HandleControlsMenuVisibilityOnTick(float DeltaTime);

	// Rate at which TAB controls menu fades in/out
	UPROPERTY(EditDefaultsOnly)
	float ControlsMenuFadeRate;


	/** Fields for mouse sensitivity */

	// Change mouse sensitivity. If no param given use the value in the game instance
	void ChangeMouseSensitivity(float NewMouseSensitivity = -1.f);


	/** Fields to make sure movement speed is correct on begin play */

	// When playing in editor movement speed is not correct
	void MakeMovementSpeedCorrectForBeginPlay();


	/** Fields for knowing if fallen out of world */

	// Check if lower than certain Z value on tick
	void CheckIfFallenOutOfWorldOnTick(const float DeltaTime);

protected:
	
	// Handles the cooldown for switching weapons
	FTimerHandle SwitchWeaponTimerHandle;

	void MakeFiringAfterSwitchWeaponsOk();

	// Handles when the player can fire after a weapon switch
	FTimerHandle FireAfterWeaponSwitchTimerHandle;

	/* Handles the small delay in noticing the need to reload when you have 
	   emptied the clip but are still pulling the trigger */
	FTimerHandle NoticeNeedToReloadDelayTimerHandle;

	/* The delay for stamina to start recharging after sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaRechargeDelay;

	/* Handles the delay for recharging stamina */
	FTimerHandle StaminaRechargeDelayTimerHandle;

	/* True if stamina can recharge */
	bool bCanStaminaRecharge;

	/* True is stamina is recharging from 0 */
	bool bStaminaRechargingFromZero;

	/* True if the character wants to sprint i.e shift is pressed down */
	bool bWantsToSprint;

	/* A function that gets the stamina recharge delay timer started if it already isn't running */
	void TryStartStaminaRechargeTimer();

	/* A function that handles everything that needs to be done in Tick() regarding stamina 
	   such as:
	   - Increasing/Decreasing stamina
	   - Increasing/decreasing run speed
	*/
	UFUNCTION(BlueprintCallable)
	void HandleSprintForTick(float DeltaTime);

	/** Fires a projectile. */
	void OnFire();

	/* True if the player can fire, false otherwise*/
	bool bCanFire;

	/* Handles the delay between shots*/
	FTimerHandle FireDelayTimerHandle;

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void BeginPickup();
	void EndPickup();

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	/** Returns muzzle location component **/
	FORCEINLINE class USceneComponent* GetMuzzleLocation() const { return FP_MuzzleLocation; }

private:

	void RaycastForMouseoverDisplaysOnTick(float DeltaTime);

	/* This used to be in the method above but is now here as a UPROPERTY to see if 
	   it stops errors */
	UPROPERTY()
	ACharacter * Bot = nullptr;

	/* The players initial stamina */
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float InitialStamina;
	
	/* The players current stamina */
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float CurrentStamina;

};

