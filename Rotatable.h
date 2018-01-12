// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Rotatable.generated.h"

UCLASS()
class FPS_PROJECT_API ARotatable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotatable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The axis to rotate the mesh on
	UPROPERTY(EditAnywhere, Category = "Animation", meta = (ToolTip = "The axis to rotate the mesh on. This must be one of either Yaw, Pitch or Roll"))
	FString RotationAxis = "Yaw";
	
	// The static mesh for the rotatable
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RotatableMesh;

	// Box trigger for the rotatable 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* AnimTrigger;

	// The max spin that the rotatable will reach
	UPROPERTY(EditAnywhere, Category = "Animation")
	float MaxRotation;

	// Curve float reference
	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* AnimCurve;

	// True if the animation will only ever trigger once
	UPROPERTY(EditAnywhere, Category = "Animation")
	bool bTriggerOnlyOnce;

	// A delay in seconds on when the rotatable should trigger. May only work reliably when TriggerOnlyOnce is true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float TriggerDelay;

	// A function that plays the animation
	UFUNCTION(BlueprintCallable)
	void PlayAnimation();

	// The amount of time dilation this rotatable causes when the player enters its trigger box. 
	// A value of 1 means no time dilation occurs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Dilation")
	float TimeDilationAmount;

	// The delay in activating the time dilation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Dilation")
	float TimeDilationDelay;

	// The curve used for the 'slow down time' transition and the 'speed up time' transition.
	// This curve should therefore start at 0 and end at 0 if you want time dliation to return to normal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Dilation")
	UCurveFloat* TimeDilationCurve;

	// Timer handle for time dilation delay
	FTimerHandle TimeDilationDelayTimerHandle;

	// Timer handle for duration of time dilation
	FTimerHandle TimeDilationDurationTimerHandle;

	// For timer handle: makes time start to slow down and then eventually speed up again
	void SlowDownTime();

	// Function for progressing time dilation curves. Must be a UFUNCTION
	UFUNCTION()
	void HandleTimeDilationProgress(float Value);

	// Timeline for slowing down time
	FTimeline TimeDilationTimeline;


	/** Fields for making the player face the rotatable when it is triggered */

	/* True if this rotatable will make the player face it when it is triggered */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Change Facing")
	bool bChangesPlayerFacing;

	/* Delay in making the player start to face the rotatable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Change Facing")
	float ChangeFacingDelay;

	/* Curve for changing facing direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Change Facing")
	UCurveFloat* ChangeFacingCurve;

	// For timer handle: makes player start changing their facing direction
	void ChangePlayerFacing();

	// Function for progressing change facing curves. Must be a UFUNCTION
	UFUNCTION()
	void HandleChangeFacingProgress(float Value);

	// Timeline for change facing
	FTimeline ChangeFacingTimeline;

	// A reference to the player controller. Only needed if changing their rotation
	AFPS_ProjectCharacter* Player;

	// The players rotation when right before their facing direction chnages
	FRotator PlayerInitialRotation;


	/** Fields for making flaming rocks go lit */

	// If true this trap has flaming boulders in it and will light them
	UPROPERTY(EditAnywhere, Category = "Flaming Boulder Trap")
	bool bIsFlamingBoulderTrap;

	// Delay from player overlap to time boulders are lit
	UPROPERTY(EditAnywhere, Category = "Flaming Boulder Trap")
	float BoulderLightDelay;

	// Timer handle for lighting flaming boulders
	FTimerHandle LightBouldersTimerHandle;

	// Light all boulders in world
	void LightBoulders();


private:

	// Initial rotation
	FRotator ActorInitialRotation;

	// Target rotation
	FRotator TargetRotation;

	// Timeline to make rotatable spin
	FTimeline MyTimeline;

	// True if the animation has played at least once
	bool bHasBeenTriggered;

	UFUNCTION()
	void HandleProgress(float Value);

public:

	UFUNCTION()
	void OnAnimTriggered(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Lock for overlap
	FCriticalSection OverlapLock;

};
