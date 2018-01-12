// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQuery.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQueryManager.h"
#include "DamagableBot.h"
#include "Extreme_Bot.generated.h"

UCLASS()
class FPS_PROJECT_API AExtreme_Bot : public ACharacter, public IDamagableBot
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AExtreme_Bot();

	// Skeletal mesh for bot
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//USkeletalMeshComponent* BotMesh;

	// Weapon mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* WeaponMesh;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f) override;

	// Called whenever the player enters the bots line of sight
	UFUNCTION()
	void OnPlayerCaught(APawn* Pawn);

	// Overriden from IDamagable
	virtual bool GoAlert() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree* BotBehavior;

	// For the cone of vision
	UPROPERTY(VisibleAnywhere, Category = AI)
	class UPawnSensingComponent* PawnSensingComp;

	// A reference to the controller
	class AExtreme_BotController* BotController;

	// A reference to the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AFPS_ProjectCharacter* Player;

	// ID that must match patrol point ID for this bot to patrol
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PatrolPointID;

	// The name of the socket for the weapon to attach to
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName WeaponSocket;

	// The rate at which the bot rotates
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeed;

	// Check for line of sight from one vector to another. Returns true if they have line of sight
	UFUNCTION(BlueprintCallable)
	bool CheckLoS(const FVector & Source, const FVector & Destination);

	// Make the bot crouch
	UFUNCTION(BlueprintCallable)
	void BotCrouch();

	// Fire a salvo of shots
	UFUNCTION(BlueprintCallable)
	void FireWeapon();

	// Fire a single shot. Called multiple times by FireWeapon()
	UFUNCTION(BlueprintCallable)
	void FireShot();

	// The number of shots FireWeapon() will fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumShots;

	// Makes bot stand and check for LoS
	UFUNCTION(BlueprintCallable)
	void CheckLoSWhileStanding();

	// True if the bot is standing to check for LoS
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsStandingToCheckForLoS;

	// Sets IsCrouching to true
	UFUNCTION(BlueprintCallable)
	void SetCrouching();

	// Reloads weapon
	UFUNCTION(blueprintCallable)
	void Reload();

	// Called when reload is complete
	UFUNCTION(blueprintCallable)
	void ReloadComplete();

	// Time it takes to reload weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadTime;

	// Reload timer handle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle ReloadTimerHandle;

	// True if reloading
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsReloading;

	// Current clip ammo of rifle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RifleClipAmmo;

	// Max rifle clip ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxRifleClipAmmo;

	// Firing timer handle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle FiringTimerHandle;

	// Called when the bot stops firing.
	UFUNCTION(BlueprintCallable)
	void StopFiring();

	// The time it takes the fire animation to play
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireAnimationLength;

	// True if the bot was crouching before firing
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWasCrouching;

	// The half height of the capsule when not crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeightStanding;

	// The half height of the capsule when crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeightCrouched;

	/* Fields for animation plus AI */
	
	// True if the bot has zero health left
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	bool bHasZeroHealth;

	// True if the bot if firing their weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	bool bIsFiring;

	// True if the bot is crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	bool bIsCrouching;

	// True if the bot is moving at a jogging speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	bool bIsJogging;

	// True if the bot is moving at a fast sprinting speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	bool bIsSprinting;

	// True if the bot is moving to cover
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	bool bIsMovingToCover;

	// True if the bot can see the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	bool bCanSeePlayer;

	// True if the bot is behind cover
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	bool bIsBehindCover;

	// True if the bot is not patrolling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	bool bIsAlerted;

	// True if the bot is strafing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	bool bIsStrafing;

	/** Non AI/Animation related fields */

	/* Take damage */
	UFUNCTION(BlueprintCallable)
	virtual bool ReceiveDamage(float Amount) override;

	virtual bool FadeMouseoverDisplay(float DeltaTime) override;

	// Health of bot
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	// The bots sprint speed which is used when it is dashing from the open to cover
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SprintSpeed;

	// The bots jogging speed which is used when it runs its patrol route
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float JogSpeed;

	// The bots walk speed which is used when it is strafing and aiming at the player out in the open
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float WalkSpeed;

	// The bots max acceleration for when it is sprinting. Higher value implies reaching the speed quicker
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SprintAcceleration;

	// The bots max acceleration for when it is jogging. Higher value implies reaching the speed quicker
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float JogAcceleration;

	// The bots max acceleration for when it is walking. Higher value implies reaching the speed quicker
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float WalkAcceleration;

};
