// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamagableAI.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UDamagableAI : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
*    This interface should be implemented by all enemy bots. It is different from IDamagable in no way, but
*    this is needed to ignore all bot classes when doing multi sphere trace by channel
*/
class FPS_PROJECT_API IDamagableAI
{
	GENERATED_IINTERFACE_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// REMEMBER: When calling the overriden versions of these functions write Execute_ before them
	// e.g. Execute_TakeDamage instead of just TakeDamage

	/* Take damage */
	UFUNCTION()
	virtual bool ReceiveDamage(float Amount) = 0;

};
