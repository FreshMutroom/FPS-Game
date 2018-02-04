// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "Sniper_Controller.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API ASniper_Controller : public AAIController
{
	GENERATED_UCLASS_BODY()
	
	
public:

	// Begin AController interface
	virtual void Possess(class APawn* InPawn) override;
	virtual void UnPossess() override;
	virtual void BeginInactiveState() override;
	// End APlayerController interface
	
	// Begin AAIController interface
	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	// End AAIController interface

	// Reference to owning pawn
	class ASniper_Bot* OwningBot;

	// Makes sniper bot start behavior
	void StartBehavior();

};
