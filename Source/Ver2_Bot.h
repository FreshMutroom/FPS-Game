// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Runtime/AIModule/Classes/Perception/PawnSensingComponent.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQueryManager.h"
#include "DamagableBot.h"
#include "Ver2_Bot.generated.h"


UENUM(BlueprintType)
enum class EWeaponEnum : uint8
{
	Rifle 	UMETA(DisplayName = "Rifle"),
	Launcher 	UMETA(DisplayName = "Grenade Launcher"),

};


UCLASS()
class FPS_PROJECT_API AVer2_Bot : public ACharacter, public IDamagableBot
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVer2_Bot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Use own function to make bot face player
	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f) override;

	// Rifle muzzle offset
	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* RifleMuzzleLocation;

	// Launcher muzzle offset
	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* LauncherMuzzleLocation;

	/* Take damage */
	UFUNCTION(BlueprintCallable)
	virtual bool ReceiveDamage(float Amount) override;

	// Current health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	// Max health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	// Weapon mesh for rifle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* RifleWeaponMesh;

	// Mesh for launcher
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* LauncherMesh;

	// Projectile to spawn when a hitting rifle shot is fired
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AVer2_Projectile> BP_RifleHit;

	// Projectile to spawn when a missing rifle shot is fired
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AVer2_Projectile> BP_RifleMiss;

	// Projectile to spawn when firing launcher
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AVer2_LauncherProjectile> BP_LauncherShot;

	// Muzzle flash for rifle fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* RifleMuzzleflash;

	// Muzzle flash for launcher fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* LauncherMuzzleFlash;

	// Muzzle smoke for launcher fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* LauncherMuzzleSmoke;

	// For the cone of vision
	UPROPERTY(VisibleAnywhere, Category = AI)
	class UPawnSensingComponent* PawnSensingComp;

	// Widget for floating health bar and ammo that appears when player looks at bot long enough
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* MouseoverInfoBox;

	// Widget for overhead ! and ? to show alert status
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* OverheadIconWidget;

	// A reference to the controller
	class AVer2_Controller* BotController;

	// A reference to the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AFPS_ProjectCharacter* Player;

	// ID that must match patrol point ID for this bot to patrol
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PatrolPointID;

	// The name of the socket for the weapon to attach to
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName WeaponSocket;

	// The rate at which the bot rotates
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeed;

	// If true print debug messages
	UPROPERTY(EditAnywhere)
	bool bIsDebugging;

	// Function that makes debug message
	void DebugMsg(const FString Message, FColor Color);

	// Called whenever the player enters the bots line of sight
	UFUNCTION()
	void OnSensePawn(APawn* Pawn);

	// Muzzle location of rifle relative to rifle SK
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RifleMuzzleOffset;

	// Rotation offset for when spawning PS at rifle muzzle location
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RifleMuzzleFlashRotationOffset;

	// Muzzle location of launcher relative to launcher SK
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LauncherMuzzleOffset;

	/** Behavior fields */

	// If true then this bot will spawn doing a "fire at shooting range target" routine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing range")
	bool bIsAtFiringRange;

	// If true is spawned from spawn point after computer event and does not stop to 
	// wait at patrol points
	UPROPERTY(EditDefaultsOnly)
	bool bIsAlwaysMoving;

	// Do firing range routine
	void StartFiringRangeRoutine();

	// Focus for firing range. This will be assigned automatically
	UPROPERTY(EditAnywhere, Category = "Firing range")
	class AActor* FiringRangeTarget;

	// 0 = Chooses closest target as firing range target
	// 1 = chooses 2nd closest target
	// 2 = chooses 3rd closest target, etc
	UPROPERTY(EditAnywhere, Category = "Firing range")
	int32 FiringRangeTargetDistance;

	void PrepareFireAtFiringRangeTarget();

	// Fire at firing range target
	void FireAtFiringRangeTarget();

	// Play reload animation at firing range
	void ReloadAtFiringRange();

	void OnReloadAtFiringRangeFinish();

	// Lower weapon at firing range by playing idle hip montage
	void LowerWeapon();

	// BP for firing range projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing range")
	TSubclassOf<class AVer2_FiringRangeProjectile> BP_FiringRangeProjectile;

	// Start patrolling around patrol points routine
	void StartPatrol();

	// Current target for patrol
	class AVer2_PatrolPoint* NextPatrolPoint;

	// Move to next patrol point
	void MoveToNextPatrolPoint();
	
	// Check if arrived at patrol point with MoveTo
	void CheckProgressOfPatrolPointMoveTo();

	// Timer handle to check if at MoveTo patrol point
	FTimerHandle PatrolPointMoveToTimerHandle;

	// Sets bIsIdle to true for animation only
	void IsIdle();

	// Wait at patrol point
	void WaitAtPatrolPoint();

	// Called when the bot sees the player and is not already in the alert phase
	UFUNCTION(BlueprintCallable)
	void EnterAlertPhase();

	// Overridden from IDamagableBot. Enter alert phase if not already in it
	virtual bool GoAlert() override;

	// Alert other nearby bots when entering alert phase
	void AlertNearbyBots();

	// EQS for finding cover locations
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEnvQuery* FindCoverQuery;

	// Needed to do the strafe query
	FEnvQueryRequest EQSFindCoverQuery;

	// Run strafe Query
	void RunFindCoverQuery();

	// Called when the strafe query has finished
	void OnFindCoverQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	// Location of cover
	FVector CoverLocation;

	// Check progress of move to cover
	void CheckProgressOfMoveToCover();

	// Timer handle to constantly check if at cover location
	FTimerHandle CheckIfAtCoverLocationTimerHandle;

	// Amount of time to allow for a move to cover before considering it failed (i.e. the distance
	// check CheckProgressOfMoveToCover() does not correctly consider the bot at the location
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeAllowedForMoveToCover;

	// A timer handle that if after TimeAllowedForMoveToCover the bot has not reached cover it 
	// will continue actions
	FTimerHandle MoveToCoverFailsafeTimerHandle;

	// Decide next action randomly
	UFUNCTION(BlueprintCallable)
	void DecideNextAction();

	// Decide next action when the launcher is equipped
	void DecideNextActionLauncher();

	// Decide next action while doing strafing routine
	void DecideNextActionFromStrafe();

	// True if the bot is in the alert phase.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAlerted;

	// True if the bot is in the caution phase because they have recently lost sight of the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCautious;

	// Makes bot start routine of standing, shooting, then crouching and checking for LoS along the way too
	UFUNCTION(BlueprintCallable)
	void BeginCoverRoutine();

	// Returns true if bot is in high cover, false if in low cover. Low cover means if it crouches it is 
	// not visibile to player but if it stands it is visibile. High cover means if it stands it 
	// is not visible
	bool CheckCoverHeight();

	// For CheckCoverHeight: this is the difference in head height between crouched and standing
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StandingCrouchingHeightDifference;

	// If behind high cover then move out of it, fire, then move back to cover
	void BeginHighCoverRoutine();

	// Number of times to pop out of high cover before choosing another action
	int32 NumHighCoverPopOuts;

	// The location where the high cover is so the bot can move back to it after it has fired 
	// a salvo from out in the open
	FVector HighCoverLocation;

	// EQS query to find a location to the left/right of the bot to strafe to
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEnvQuery* HighCoverStrafeQuery;

	// Needed to do the strafe query
	FEnvQueryRequest EQSHighCoverStrafeQuery;

	// Run strafe Query
	void RunHighCoverStrafeQuery();

	// Called when the strafe query has finished
	void OnHighCoverStrafeQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	// Check if high cover strafe move has completed
	void CheckProgressOfHighCoverStrafeMove();

	// Fire salvo from the strafe location near high cover, then go back to high cover
	void FireHighCoverSalvo();

	// Fire a single shot from out in the open after moving out of high cover
	void FireHighCover();

	void OnFireHighCoverFinish();

	void ReloadHighCover();

	void OnReloadHighCoverFinish();

	void ReturnToHighCover();

	void CheckProgressOfReturnToHighCover();

	// Fire straight away upon reaching cover
	void FireUponReachingCover();

	// Makes bot go from crouching to standing and then shoot at player if in LoS
	UFUNCTION(BlueprintCallable)
	void StandAndFire();

	// Timer handle for handling fire animation
	FTimerHandle FinishedShotTimerHandle;

	// Called after FireDelay after firing a shot
	UFUNCTION(BlueprintCallable)
	void OnFireFinished();

	// Go crouched
	UFUNCTION(BlueprintCallable)
	void CrouchInCover();

	// Stand up
	// @param bCrouchAfterwards whether to crouch again after
	void StandUp(bool bCrouchAfterwards);

	// Clears focus and makes the bot look left to right for a bit
	UFUNCTION(BlueprintCallable)
	void LookAround();

	// Look to the left
	UFUNCTION(BlueprintCallable)
	void LookLeft();

	// Look straight ahead
	UFUNCTION(BlueprintCallable)
	void LookStraightAhead();

	// Look to the right
	UFUNCTION(BlueprintCallable)
	void LookRight();

	// The amount to rotate by when looking left/right
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LookAroundAmount;

	// The number of times the bot will change look direction before giving up
	int32 NumLooks;

	// Timer handle for looking handling time looking in one direction
	FTimerHandle LookAroundTimerHandle;

	// The minimum time the bot spends looking in one direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinLookTime;

	// The maximum time the bot speeds looking in one direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLookTime;

	// true if looking left
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsLookingLeft;

	// true if looking straight ahead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsLookingStraightAhead;

	// true if looking right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsLookingRight;

	// The time inverval in which damage is considered recent
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecentDamageTimeInterval;

	// How much damage the bot has taken in the last RecentDamageTimeInterval
	float RecentDamage;

	// Timer handle that keeps track of how much recent damage has been taken
	FTimerHandle RecentDamageTimerHandle;

	// If true then crouch again after firing salvo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWasCrouched;

	// Leave high alert and go into a caution phase where the bot patrols around for a bit
	UFUNCTION(BlueprintCallable)
	void GoIntoCautionPhase();

	// Timer handle to control when to fire after standing
	FTimerHandle FireAfterStandingTimerHandle;

	// Timer handle to handle delay in seeing player and entering alert phase
	FTimerHandle EnterAlertTimerHandle;

	// Start standing, strafing and shooting routine
	UFUNCTION(BlueprintCallable)
	void StartStandAndStrafeRoutine();

	// True if the bot is going through the strafe routine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDoingStrafeRoutine;

	// EQS query to find a location to the left/right of the bot to strafe to
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEnvQuery* StrafeQuery;

	// Needed to do the strafe query
	FEnvQueryRequest EQSStrafeQuery;

	// Run strafe Query
	void RunStrafeQuery();

	// Called when the strafe query has finished
	void OnStrafeQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	// Checks to see if strafe move command has finished
	void CheckIfStrafeMoveToHasCompleted();

	// Fire rifle salvo while strafing
	void FireSalvoWhileStrafing();

	// Fire a single shot while strafing
	void FireWhileStrafing();

	// Called by timer handle when strafe shot finishes
	void OnStrafeFireFinished();

	// Reload while doing strafe routine
	void ReloadWhileStrafing();

	// Called when reloading while strafing finishes
	void OnReloadWhileStrafingFinish();

	// Accuracy penalty for firing while strafing. Should be between 0 and 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StrafeAccuracyPenalty;

	// True if firing salvo while strafing
	bool bIsFiringAndStrafing;

	// Timer handle to check is strafe move has completed
	FTimerHandle StrafeMoveTimerHandle;

	// Starts grenade launcher routine
	UFUNCTION(BlueprintCallable)
	void BeginGrenadeLauncherRoutine();

	// True if doing grenade launcher routine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDoingGrenadeLauncherRoutine;

	// Time it takes to equip grenade launcher
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GrenadeLauncherEquipTime;

	// Start movement pattern for when launcher is equipped. This involves getting close to the player
	void StartGrenadeLauncherMovement();

	// Move to a point nearthe player
	void StrafeWithLauncher();

	// Run close to player. Called when a MoveTo command decides the path is not reachable
	void RunAtPlayer();

	// Fire rifle while running at player
	void FireRifleSalvoWhileRunningAtPlayer();

	// Fire a single shot while running at player
	void FireRunningAtPlayer();

	// Called when finished firing a shot while running at player
	void OnFireRunningAtPlayerFinish();

	// Reload while running at player
	void ReloadWhileRunningAtPlayer();

	// Called when ReloadWhileRunningAtPlayer finishes
	void OnReloadRunningAtPlayerFinish();

	// Timer handle to handle firing rifle while running at player
	FTimerHandle FireRifleWhileRunningAtPlayerTimerHandle;

	// Min value of how close the bot should get to the player when running at them before stopping
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunAtPlayerStopDistanceMin;

	// Max value of how close the bot should get to the player when running at them before stopping
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunAtPlayerStopDistanceMax;
	
	// Reload launcher
	void ReloadLauncher();

	// Called when reload animation has finished
	void OnLauncherReloadFinish();

	// Time it takes to reload launcher 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LauncherReloadTime;

	// Time it takes to play launcher fire animation 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float LauncherFireTime;

	// True if firing launcher
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFiringLauncher;

	// True if reloading launcher
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsReloadingLauncher;

	// Run strafe query for if holding launcher
	void RunLauncherStrafeQuery();

	// Called when launcher strafe query has finished
	void OnLauncherStrafeQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	// EQS query to find a location to the left/right of the bot to strafe to with launcher. 
	// This should make the bot move further than how far the rifle strafe query moves
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEnvQuery* StrafeQueryLauncher;

	// Needed to do the strafe query with launcher
	FEnvQueryRequest EQSStrafeQueryLauncher;

	// Timer handle that constantely checks if we are in range of the MoveTo target for launcher strafe
	FTimerHandle LauncherMoveToTimerHandle;

	// The location the bot is moving towards
	FVector MoveToLocation;

	// Function that constantly checks if at MoveTo location with launcher equipped
	void EnterMoveToCheckLoopLauncher();

	// Max launcher ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLauncherAmmo;

	// Current launcher ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentLauncherAmmo;

	// Start move to new cover routine
	UFUNCTION(BlueprintCallable)
	void BeginMoveToNewCoverRoutine();

	// Runs cover query for when moving to new cover
	void RunFindNewCoverQuery();

	// Called when RunFindNewCoverQuery finishes
	void OnFindNewCoverQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	// Makes the bot fire its weapon many times
	UFUNCTION(BlueprintCallable)
	void FireSalvo();

	// Fires a single shot
	UFUNCTION(BlueprintCallable)
	void Fire();

	// Fires a grenade launcher shot
	UFUNCTION(BlueprintCallable)
	void FireLauncher();

	// For timer handle: make smoke PS disappear
	void MakeSmokeInvisible();

	// Play idle standing ironsights montage. Called by timer handle
	void PlayIdleStandingIronsightsMontage();

	// Fire a shot with a good chance to hit
	UFUNCTION(BlueprintCallable)
	void FireHittingShot(FVector PlayerLocation);

	// Fire a shot with a good chance to miss
	UFUNCTION(BlueprintCallable)
	void FireMissingShot(FVector PlayerLocation);

	// The number of shots to fire for the current salvo
	int32 NumShots;

	// The max clip ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxClipAmmo;

	// The current number of shots in the weapon clip
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ClipAmmo;

	// The time in between firing each shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float FireDelay;

	// Max range bot can hit player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxRange;

	// How close other bots have to be for them to be alerted as well when this bot gets alerted
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AlertOthersDistance;

	// Reload weapon
	UFUNCTION(BlueprintCallable)
	void Reload();

	// Called when reloading has finished
	UFUNCTION(BlueprintCallable)
	void OnReloadFinish();

	// Time it takes to reload
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float ReloadTime;

	// Equipped weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponEnum EquippedWeapon;

	// Equips a weapon
	UFUNCTION(BlueprintCallable)
	void EquipWeapon();

	// Clear RecentDamage
	void ResetRecentDamage();

	// Plays a montage
	void PlayMontage(UAnimMontage* Montage);

	/** AnimMontage to play when taking damage and not alert, chosen at random */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* TakeDamageAnimation1;
	
	/** AnimMontage to play when taking damage and not alert, chosen at random */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* TakeDamageAnimation2;

	/** AnimMontage to play when taking damage and not alert, chosen at random */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* TakeDamageAnimation3;

	/** AnimMontage to play when taking damage and not alert, chosen at random */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* TakeDamageAnimation4;

	/* Animation montage for going from standing to crouching */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* StandToCrouchMontage;

	/* Animation montage for going from crouching to standing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* CrouchToStandMontage;

	/* Animation montage for going from crouching to standing, and holding weapon at hip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* CrouchToStandHipMontage;

	/* Animation montage for firing rifle from ironsights */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* FireRifleIronsightsMontage;

	/* Animation montage for firing rifle from hip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* FireRifleHipMontage;

	/* Animation montage for firing launcher */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* FireLauncherMontage;
	
	/* Animation montage for reloading rifle and ending at hip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* ReloadRifleMontage;

	/* Animation montage for reloading rifle and ending in ironsights */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* ReloadRifleIronsightsMontage;

	/* Animation montage for reloading launcher */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* ReloadLauncherMontage;

	/* Animation montage for switching weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* SwitchWeaponsMontage;

	/* Animation montage for holding weapon at hip. Should be played after equipping grenade launcher */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* IdleHipMontage;

	/* Animation montage 1 for when idle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* IdleMontage1;

	/* Animation montage 2 for when idle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* IdleMontage2;

	/* Animation montage for when looking around */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* IdleIronsightsMontage;

	/* Animation montage for zero health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* ZeroHealthMontage1;

	/* Animation montage for zero health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* ZeroHealthMontage2;

	/* Animation montage for zero health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* ZeroHealthMontage3;

	/* Animation montage for zero health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* ZeroHealthMontage4;

	// Play a random zero health montage
	void PlayZeroHealthMontage();

	// Pause animation. Used to keep the pose at the end of the zero health animation
	void PauseAnimation();

	// Current take damage montage playing
	class UAnimMontage* TakeDamageMontage;

	// Change move speed
	void ChangeMoveSpeed(float NewSpeed);

	// Stops all montages that are playing
	void StopMontages();

	// Timer handle to make bot go alert after taking damage
	FTimerHandle MakeAlertAfterDamageTimerHandle;

	// Timer handle that alerts nearby bots after taking damage
	FTimerHandle AlertNearbyBotsAfterDamageTimerHandle;

	// True if bot has taken a lot of damage recently
	bool bIsLoosingHealthFast;

	// The time from when the change weapon animation started to when the weapon mesh should change
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float WeaponChangeHalfTime;

	// True if equipping a weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsChangingWeapon;

	// True if idle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsIdle;

	// True if firing weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsFiring;

	// True if reloading
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsReloading;

	// true if crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsCrouching;

	// True if the bot has zero health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bHasZeroHealth;

	// Run speed when going for cover
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed;

	// Jog speed for when patrolling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JogSpeed;

	// Movement speed while strafing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StrafeMovementSpeed;

	// Crouched movement speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchMovementSpeed;


	/** Mouseover info display fields */

	// Fade the info display in/out. Does this by calling one of the two functions below.
	virtual bool FadeMouseoverDisplay(float DeltaTime) override;

	// True if Mouseover display is fading in
	bool bIsMouseoverDisplayFadingIn;

	// True if mouseover display is fading out
	bool bIsMouseoverDisplayFadingOut;

	// How fast the mouseover display fades in
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseoverDisplayFadeInSpeed;

	// How fast the mouseover display fades out
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseoverDisplayFadeOutSpeed;

	// Where to position the mouseover box relative to the root component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MouseoverBoxOffset;

	// Update the position of the mouseover display so it stays glued to the bot
	void UpdateMouseoverDisplayPosition();

	// Makes mouseover display invisible
	void MakeMouseoverDisplayInvisible();

	// Removes this bot from players mouseover arrays
	void RemoveFromMouseoverArrays();


	/** Alert icon fields */

	// Show alert "!" above head
	void ShowExclamationPointIcon();

	// Show "?" icon above head
	void ShowQuestionMarkIcon();

	// Fade out overhead icon text if it is a "!"
	void FadeOutOverheadExclamationPoint();

	// Fade out overhead icon text if it is a "?"
	void FadeOutOverheadQuestionMark();

	// A reference to the user widget of the overhead icon widget that has been cast to 
	// a UVer2_MouseoverDisplay so the animation can be referenced
	UPROPERTY()
	class UVer2_MouseoverDisplay* OverheadIconUserWidget;

	// Text to be displayed in the overhead icon widget.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OverheadIconText;

	// Color for overhead icon text
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateColor OverheadIconTextColor;

	// Offset from player for overhead icon text
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector OverheadIconOffset;

	// Update position of overhead icon
	void UpdateOverheadIconLocation();


	/** Fields for sounds */

	// Array of rifle fire sounds. Should be populated in editor
	UPROPERTY(EditAnywhere, Category = "Sound Effects")
	TArray<USoundBase*> RifleFireSounds;

	// Array of launcher fire sounds. Should be populated in editor
	UPROPERTY(EditAnywhere, Category = "Sound Effects")
	TArray<USoundBase*> LauncherFireSounds;

	// Array of zero health sounds. Should be populated in editor
	UPROPERTY(EditAnywhere, Category = "Sound Effects")
	TArray<USoundBase*> ZeroHealthSounds;

};
