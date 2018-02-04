// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 *   This is the controller for the menus only.
 */
UCLASS()
class FPS_PROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()


	virtual void BeginPlay() override;

	UPROPERTY()
	class UMyGameInstance * GameInstance;

	UFUNCTION()
	void SetupInput();

	/* Skip cutscene */
	UFUNCTION()
	void SkipCutscene();

	/* Checks if game instance is valid. If not then makes it valid and returns  true if GameInstance 
	   is not nullptr */
	bool CheckGameInstance();


public:

	// Change input for main menu only
	void EnableMainMenuMode();
	
	// Hide player HUD
	void HideHUD();

};
