// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MenuCamera.generated.h"

UCLASS()
class FPS_PROJECT_API AMenuCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMenuCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CameraMesh;

	UPROPERTY(EditAnywhere)
	USceneComponent * StartLocation;

	UPROPERTY(EditAnywhere)
	USceneComponent * EndLocation;

	// Curve float reference
	UPROPERTY(EditAnywhere)
	UCurveFloat* AnimCurve;

	// Duration of timeline. Should be assigned a value automatically on begin play
	// Doesn't really need to be EditAnywhere
	UPROPERTY(EditAnywhere)
	float Duration;

	// Initial rotation
	FVector ActorInitialLocation;

	// Target location
	FVector TargetLocation;

	// Timeline to make moveable move
	UPROPERTY()
	FTimeline MyTimeline;

	UFUNCTION(BlueprintCallable)
	void HandleProgress(float Value);

	/* Called when the view switches to this camera. It should then start its timeline */
	void OnSwitchTo();

	/* Called when this camera no longer becomes the camera to view from */
	void OnSwitchFrom();

	/* True if this is the active camera */
	bool bIsActive;

	/* Reference to player controller */
	UPROPERTY()
	class APlayerController * PlayerController;

};
