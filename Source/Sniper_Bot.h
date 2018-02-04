// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "FPS_ProjectCharacter.h"
#include "Sniper_Bot.generated.h"


UCLASS()
class FPS_PROJECT_API ASniper_Bot : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASniper_Bot();

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


	/** Essential components */
	
	// Sniper Rifle muzzle offset
	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* SniperRifleMuzzleLocation;
	
	// Weapon mesh for sniper rifle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SniperRifleWeaponMesh;

	// Muzzle flash PS
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystemComponent* MuzzleFlash;

	// Smoke bomb PS
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystemComponent* SmokeBomb_PS;

	// A reference to the player
	UPROPERTY()
	AFPS_ProjectCharacter * Player;

	// A reference to the AI controller
	UPROPERTY()
	class ASniper_Controller* BotController;

	/** Editor fields */

	// The number of sniper hits before zero health
	UPROPERTY(EditAnywhere)
	int32 Health;

	// Max clip ammo. 5 is good
	UPROPERTY(EditAnywhere)
	int32 MaxClipAmmo;
	
	// The rate at which the bot rotates
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeed;

	// Movement speed when running
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunSpeed;

	// Movement speed when crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrouchSpeed;

	// Minimum time spent at a vantage point before moving to a new one
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinSnipingLocationTime;

	// Maximum time spent at a vantage point before moving to a new one
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSnipingLocationTime;

	// The focus point to look at when getting to a sniping location
	UPROPERTY(EditAnywhere)
	AActor* DefaultFocus;

	// How often LoS checks are made towards player in seconds. Checks are done during tick
	UPROPERTY(EditAnywhere)
	float LoSCheckRate;

	// The minimum amount of time the player must spend in sight for the bot to focus on them
	UPROPERTY(EditAnywhere)
	float LookAtTime;

	// The minimum amount of time the player must spend in sight for the bot to fire their first 
	// shot at them. Default value = 1.2
	UPROPERTY(EditAnywhere)
	float OpenFireTime;

	// Projectile blueprint
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASniper_Projectile> BP_Projectile;

	// Sniper rifle rotation offset
	UPROPERTY(EditAnywhere)
	FRotator RifleRotationOffset;

	// Sniper rifle location offset
	UPROPERTY(EditAnywhere)
	FVector RifleLocationOffset;

	// Sniper rifle muzzle rotation offset
	UPROPERTY(EditAnywhere)
	FRotator MuzzleRotationOffset;

	// Sniper rifle muzzle location offset
	UPROPERTY(EditAnywhere)
	FVector MuzzleLocationOffset;

	// Typewriter message to create when knows player has sniper rifle
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATutorialMessage> BP_SeenSniperRifleMessage;


	/** Helper functions called at begin play */

	void Init();

	// Populate SnipingLocations with all Sniper_VantagePoints
	void PopulateSnipingLocations();


	/** Functions for behavior */

	// Move to the starting vantage point
	void MoveToStartingVantagePoint();

	// Called when bot sees player holds a sniper rifle

	// Randomly pick a sniper vantage point to move to, but do not move to the one previously visited. 
	// If there are a total of 5 points then this will randomly choose from 3
	void SelectNewVantagePoint();

	// Move to the vantage point CurrentVantagePoint which should have been chosen by SelectNewVantagePoint()
	void MoveToNewVantagePoint();

	// Called by timer handle after animation montage has finished and actually makes bot move to new point
	void IssueMoveCommand();

	// Constantly checks whether the move command has completed. When it has the bot goes prone and resumes
	// sniping procedure
	void CheckProgressOfMoveCommand();

	// Either go prone or crouch and resume trying to snipe player
	void ResumeSnipingRoutine();

	// Called when bot has fully set up at new location
	void OnChangeLocationFinish();

	// Check LoS to player. Returns true if there is LoS
	bool CheckLoSToPlayer();

	// Check line of sight from player to bot. This prevents player from sniping bot in a spot where 
	// the bot can't see the player. Returns true if there is LoS
	bool CheckLoSFromPlayer();

	// Create "has seen sniper rifle message" and set boolean to true
	void OnHasSeenRifle();

	// Check if player is aiming with sniper rifle. Returns true if they are
	bool IsPlayerAimingWithSniper();

	// Fire at player
	void FireAtPlayer();

	// Reload
	void Reload();

	// Called when reloading finishes
	void OnReloadFinish();

	//Make bReadyToMoveLocation true
	void MakeReadyToMoveLocationTrue();

	// Called when hit by sniper round from player
	void OnHit();

	// Drop smoke bomb at current location
	void DropSmokeBomb();

	// Makes bHasBeenRecentlyHit false
	void ResetHasBeenRecentlyHit();

	// Plays run montage
	void PlayRunMontage();

	// Makes the bot move regardless of whether they can see the player or not
	void MakeForcedMove();


	/** Fields for behavior */

	// Timer handle that controls when to move to a new vantage point 
	FTimerHandle MoveTimerHandle;

	// Timer handle that controls firing at target
	FTimerHandle FireTimerHandle;

	// Array of all vantage points
	UPROPERTY()
	TArray<AActor*> SnipingLocations;

	// The vantage point the bot is currently at
	UPROPERTY()
	AActor* CurrentVantagePoint;

	// The vantage point the bot just came from
	UPROPERTY()
	AActor* PreviousVantagePoint;

	// The vantage point selected as the next location to move to
	UPROPERTY()
	AActor* NewVantagePoint;


	/** Fields for animation */

	// Plays a montage
	void PlayMontage(UAnimMontage* Montage);

	// Crouches bot
	void GoCrouched();

	// Called by timer handle: play crouched walk montage
	void CrouchedWalk();

	// Change movement speed
	void ChangeMoveSpeed(float NewMovespeed);

	// Called when bot has lost all health
	void OnZeroHealth();

	// Freeze animations so zero health montage stops at end
	void FreezeAnimations();

	// Timer handle for saying when to change animation state
	FTimerHandle AnimationTimerHandle;

	// Animation montage for running
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* RunMontage;

	// Animation montage for walking while crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CrouchedWalkMontage;

	// Animation montage for idle while crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CrouchedIdleMontage;

	// Duration of montage, excluding idle part tagged on to end
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StandToProneMontageDuration;

	// Animation montage for going from standing to prone
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* StandToProneMontage;

	// Duration of montage, excluding idle part tagged on to end
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StandToCrouchMontageDuration;

	// Animation montage for going from standing to crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* StandToCrouchMontage;

	// Duration of montage, excluding idle part tagged on to end
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrouchToStandMontageDuration;

	// Animation montage for going from standing to crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CrouchToStandMontage;

	// Duration of montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProneToStandMontageDuration;

	// Animation montage for going from prone to standing
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ProneToStandMontage;

	// Duration of montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireProneMontageDuration;

	// Animation montage for firing sniper rifle prone
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* FireProneMontage;
	
	// Duration of montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireCrouchedMontageDuration;

	// Animation montage for firing sniper rifle crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* FireCrouchedMontage;

	// Duration of montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadProneMontageDuration;

	// Animation montage for reloading sniper rifle while prone
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ReloadProneMontage;

	// Duration of montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadCrouchedMontageDuration;

	// Animation montage for reloading sniper rifle while crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ReloadCrouchedMontage;

	// Duration of montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TakeDamageMontageDuration;

	// Animation montage for zero health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* TakeDamageMontage;

	// Duration of montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZeroHealthMontageDuration;

	// Animation montage for zero health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ZeroHealthMontage;

	// True if the bot is prone
	bool bIsProne;

	// True if can see player
	bool bCanSeePlayer;

	// True if reloading
	bool bIsReloading;

	// True if ready to move location
	bool bReadyToMoveLocation;

	// True if changing location
	bool bIsChangingLocation;

	// True if has zero health
	bool bHasZeroHealth;

	// True if the bot has seen the player with a sniper rifle equipped
	bool bKnowsPlayerHasSniperRifle;

	// True if recently hit by player. Without it the bot sometimes/always fires at the player right after
	// the player hits them. If your framerate is lower than 3 this isn't guaranteed to always work
	bool bHasBeenRecentlyHit;

	// Timer handle for calling ResetHasBeenRecentlyHit()
	FTimerHandle RecentHitTimerHandle;

	/** Variables and fucntions only used in source file */

	// Current running total for time between checking for LoS
	float LoSCheckTotal;

	// Current running total for time player has spent in LoS
	float LoSTotal;

	// Clip ammo
	int32 ClipAmmo;

	
	/** Sound fields */

	// Sound to play when firing rifle.
	UPROPERTY(EditDefaultsOnly)
	class USoundBase * RifleSound;

	// Play rifle fire sound after delay based on distance to player
	void PlayRifleFireSoundAfterDelay();

	// Play the sniper rifle sound
	void PlayRifleFireSound();


};


