// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "DamagableBot.h"
#include "Advanced_BotProjectile.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQueryManager.h"
#include "Advanced_Bot.generated.h"


/*
	- Bots could use one of two different weapons
	
	Behavior:

	- Patrol around points until see player
	- When see player enter alert phase
	Alert phase:
	Fire at random intervals while doing the following
	- Running at player for a certain amount of time or until within a certain range of player
	- Strafing

*/

UCLASS()
class FPS_PROJECT_API AAdvanced_Bot : public ACharacter, public IDamagableBot
{
	GENERATED_UCLASS_BODY()

public:
	// Sets default values for this character's properties
	AAdvanced_Bot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	
	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f);	// removed override

	// My extra functions start here

	// The number of shots fired in each burst
	uint8 NumberOfShots;

	// A reference to the player
	UPROPERTY()
	class AFPS_ProjectCharacter* Player;

	/* Take damage */
	UFUNCTION(BlueprintCallable)
	virtual bool ReceiveDamage(float Amount) override;

	// Called when hit by player
	void OnHit();

	// Called by timer handle, makes bot go alert
	void BeforeGoAlert();

	virtual bool FadeMouseoverDisplay(float DeltaTime) override;

	// Reference to controller
	UPROPERTY()
	class AAdvanced_BotController* BotController;

	// The skeletal mesh for the weapon
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	// Trail particle effect from shots
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* BulletTrails;

	// Muzzle flash particle system
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* MuzzleFlash;

	// The location of the muzzle offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MuzzleOffset;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(EditAnywhere)
	class USceneComponent* Weapon_MuzzleLocation;

	/* Pawn sensing comp*/
	UPROPERTY(EditAnywhere)
	class UPawnSensingComponent* PawnSensingComp;

	// Widget for overhead ! and ? to show alert status
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* OverheadIconWidget;

	/* A blueprint for the projectile. */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AVer2_Projectile> BP_Projectile;

	/* Array of patrol points. Using AIPatrolPoints but that should be ok */
	UPROPERTY()
	TArray<class AAIPatrolPoint*> PatrolPoints;

	int32 CurrentPatrolPoint;

	// Next patrol point to move to
	UPROPERTY()
	class AAIPatrolPoint* NextPatrolPoint;

	/** Fields that should be changed in editor */
	
	// Max health for the bot
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth;

	// The health for the bot
	UPROPERTY(BlueprintReadOnly)
	float Health;

	// Sight range
	UPROPERTY(EditAnywhere)
	float MaxRange;

	// Max amount of ammo in clip
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon System")
	int32 MaxClipAmmo;

	// Time ebtween shots
	UPROPERTY(EditAnywhere, Category = "Weapon System")
	float TimeBetweenShots;

	// Time it takes to reload
	UPROPERTY(EditAnywhere, Category = "Weapon System")
	float ReloadTime;

	// The min distance from the player the bot will stop at when running at them
	UPROPERTY(EditAnywhere)
	float MinRunAtPlayerDistance;

	// The max distance from the player the bot will stop at when running at them
	UPROPERTY(EditAnywhere)
	float MaxRunAtPlayerDistance;

	// Movement speed while patrolling
	UPROPERTY(EditAnywhere)
	float PatrolMovementSpeed;

	// Movement speed when alerted
	UPROPERTY(EditAnywhere)
	float AlertMovementSpeed;

	// Length of delay between end of salvo and start of a new one
	UPROPERTY(EditAnywhere)
	float SalvoFireRate;

	// ID for patrol points
	UPROPERTY(EditAnywhere)
	int32 PatrolPointID;

	// If true print debug messages about what the bot is doing
	UPROPERTY(EditAnywhere)
	bool bDebug;

	// If true bot was spawned from spawn point after computer event and should not
	// wait at patrol points
	UPROPERTY(EditDefaultsOnly)
	bool bWasSpawnedAfterComputerEvent;

	// If true bot does not move
	UPROPERTY(EditDefaultsOnly)
	bool bIsIdleGuard;

	// Removes this bot from players mouseover arrays
	void RemoveFromMouseoverArrays();


	/**  Fields for behavior */

	// Timer handle that handles what the next movement action is
	FTimerHandle NextActionTimerHandle;

	// Timer handle that handles when to fire and reload
	FTimerHandle FireTimerHandle;

	// Set up things like populating patrol point array, then start patrolling
	void Init();

	// Populate patrol point array
	void PopulatePatrolPointArray();

	// Begin patrol
	void BeginPatrolling();

	// Move to next patrol point
	void MoveToPatrolPoint();

	// Check progress of a MoveTo command
	void CheckProgressOfMoveTo();

	// Called when seeing player and not already alert. Must be UFUNCTION
	UFUNCTION()
	void OnSensePlayer(APawn * Pawn);

	// Overriden from IDamagable
	virtual bool GoAlert() override;

	// Fire at player
	void Fire();

	// Reload
	void Reload();

	// Called when reload is finished
	void OnReloadFinish();

	// Randomly pick a movement action
	void PickMovementAction();

	// Do a random number of strafes
	void Strafe();

	// Select a point to strafe to and move to it
	void MoveToStrafePoint();

	// Run at player until close enough or a certain amount of time has elapsed
	void RunAtPlayer();

	// Move to a random point near player
	void MoveNearPlayer();

	// Number of strafes to do when calling Strafe()
	int32 NumStrafes;

	// True if has seen player
	bool bIsAlerted;

	// Amount of ammo in clip
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ClipAmmo;

	// Number of shots to fire in a salvo
	int32 NumShots;

	/**  Helper functions */

	// Do function after a delay. If OverridePendingAction is true then if there is another action 
	// waiting for a delay then it will be cancelled. IF not then if another action is waiting to 
	// execute then this function will do nothing
	void ActionAfterDelay(void(AAdvanced_Bot:: * Function)(), float Delay, bool OverridePendingAction = true);

	// Same as ActionAfterDelay except this is for firing and reload actions only
	void FireActionAfterDelay(void(AAdvanced_Bot:: * Function)(), float Delay, bool OverridePendingAction = true);


	// EQS for finding strafe locations
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UEnvQuery* StrafeQuery;

	// Needed to do the strafe query
	FEnvQueryRequest EQSStrafeQuery;

	// Run strafe Query
	void RunStrafeQuery();

	// Called when the strafe query has finished
	void OnStrafeQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	// sets up strafe query on begin play
	void SetupStrafeQueryOnBeginPlay();




	/** Fields for animation */

	// Play animation montage, cancelling any montages that are already playing
	void PlayMontage(UAnimMontage* Montage);

	// Montage for reloading
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ReloadMontage;

	// Go ragdoll
	void EnableRagdoll();

	// Disable ragdoll
	void DisableRagdoll();

	// Montage for zero health
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ZeroHealthMontage;

	// True if firing weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFiring;

	// True if reloading
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsReloading;

	// True if has zero health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasZeroHealth;


	/** Fields for sound **/

	// Sound effect for firing rifle
	UPROPERTY(EditAnywhere)
	USoundBase* RifleFireSound;


	/** Fields for mouseover info box */

	// Setup mouseover info box on begin play
	void SetupMouseoverInfoBoxForBeginPlay();

	// Widget for floating health bar and ammo that appears when player looks at bot long enough
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* MouseoverInfoBox;

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

	// Makes mouseover display invisible. Not implemented
	void MakeMouseoverDisplayInvisible();


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

	// Setup overhead icon for begin play
	void SetupOverheadIconForBeginPlay();

	// Make overhead icon rotate to face player. Should be called in tick
	void UpdateOverheadIconRotation();

};

