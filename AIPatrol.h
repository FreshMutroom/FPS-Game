// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "FPS_ProjectCharacter.h"
#include "DamagableBot.h"
#include "AIPatrol.generated.h"

/*
	Buglist:
	- Will chase camera robot instead of player when it is active
*/


UCLASS()
class FPS_PROJECT_API AAIPatrol : public ACharacter, public IDamagableBot
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIPatrol();

	// I added this myself
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Max health
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth;

	// This bots health
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health;
	
	/* Take damage */
	UFUNCTION(BlueprintCallable)
	virtual bool ReceiveDamage(float Amount) override;
	
	virtual bool FadeMouseoverDisplay(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = AI)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = AI)
	class UPawnSensingComponent* PawnSensingComp;

	// Widget for overhead ! and ? to show alert status
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* OverheadIconWidget;

	// True if the pawn has spotted the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
	bool bHasSpottedPlayer = false;

	// True if the pawn is chasing the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
	bool bIsChasingPlayer = false;

	// True if the death animation should play
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
	bool bZeroHealth = false;

	// True if the pawn is detonating
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
	bool bIsDetonating = false;

	// Speed at which the bot will run when it sees the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunSpeed;

	// Changes bIsChasingPLayer to true
	UFUNCTION(BlueprintCallable, Category = Anim)
	void StartRunning();

	// Distance to the player at which the pawn will detonate
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetonationDistance;

	// Called when the pawn gets close enough to the player and detonates
	UFUNCTION(BlueprintCallable)
	void Detonate();

	// The particle system for the detonation explosion
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* PS_Bot;

	// The particle system for the fuse of the bomb
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* PS_Bot_Fuse;

	// For the force of the blast
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URadialForceComponent* RF_Bot;

	// A sphere component to check what was hit by the explosion
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* Sphere_Bot;

	// Array of actors caught in the explosion
	TArray<AActor*> HitTargets;

	// The ID for this pawn. It's patrol points must have the same ID
	UPROPERTY(EditAnywhere)
	uint32 PatrolPointID;

	// To stop the bot from exploding from both damage and close range
	bool bHasCalledDeestroy;

	// Removes this bot from players mouseover arrays
	void RemoveFromMouseoverArrays();


private:

	UFUNCTION()
	void OnPlayerCaught(APawn* Pawn);

	// Overriden from IDamagable
	virtual bool GoAlert() override;

	// Called when hit by player
	void OnHit();

	AFPS_ProjectCharacter* MyPlayer;

	// Start destroy animations, deal damage etc
	void Deestroy();

	// Actually destroy the actor
	void DestroyActor();

public:

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

	// Fade out overhead icon text if it is a "!"
	void FadeOutOverheadExclamationPoint();

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
