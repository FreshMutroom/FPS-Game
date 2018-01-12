// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Advanced_Bot.h"
#include "Advanced_BotController.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AAdvanced_BotController : public AAIController
{
	GENERATED_UCLASS_BODY()
	
private:
	
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

public:

	// Added this myself. A reference to the bot using this controller
	UPROPERTY(BlueprintReadWrite)
	AAdvanced_Bot* OwningBot;

	// Start bot behavior
	void StartBehavior();

};
