// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Damagable.h"


// This function does not need to be modified.
UDamagable::UDamagable(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

// Add default functionality here for any IDamagable functions that are not pure virtual.

/* Should always be overriden */
bool ReceiveDamage(float Amount)
{
	return false;

}
