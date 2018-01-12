// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamagableBot.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UDamagableBot : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
*  Exactly the same as IDamagable but this is implemented by enemy bots so they can be distiguished 
*  from the player for multi sphere traces for objects
*/
class FPS_PROJECT_API IDamagableBot
{
	GENERATED_IINTERFACE_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// REMEMBER: When calling the overriden versions of these functions write Execute_ before them
	// e.g. Execute_TakeDamage instead of just TakeDamage

	/* Take damage */
	UFUNCTION()
	virtual bool ReceiveDamage(float Amount) = 0;

	/* Fade in/out mouseover display */
	UFUNCTION()
	virtual bool FadeMouseoverDisplay(float DeltaTime) = 0;

	/* Enter alert phase */
	UFUNCTION()
	virtual bool GoAlert() = 0;

protected:

	// I think its ok to put variables here

};