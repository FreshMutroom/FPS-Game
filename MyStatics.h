// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/GameplayStatics.h"
#include "MyStatics.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(DebugMsgLogCategory, Log, All);	// This is for logging output to a file

/**
 *  Holds static methods. GameStartGameMode.h has some too.
 */
UCLASS()
class FPS_PROJECT_API UMyStatics : public UGameplayStatics
{
	GENERATED_BODY()
	
public:
	
	// Print a debug message. Should only work with editor and not in a packaged game
	UFUNCTION(BlueprintCallable)
	static void DebugMsg(const FString Message, const FColor Color);
	
	// My implementation of cross product of two vectors to test if the UE version is correct
	UFUNCTION(BlueprintCallable)
	static FVector CrossProduct(FVector & V1, FVector & V2);

	// Play a sound at random from an array at a location. The array passed in should not
	// have any gaps in it. i.e if Array.Num() == 3 then indexes 0, 1 and 2 must be populated.
	// This should be used for anything that isn't music
	UFUNCTION(BlueprintCallable)
	static void PlaySound(const UObject * Object, const TArray<USoundBase*> & Sounds, const FVector Location, float Volume = 1.f, float Pitch = 1.f);

	// Play sound. This should be used for anything that isn't music
	UFUNCTION(BlueprintCallable)
	static void PlayASound(const UObject * Object, USoundBase * Sound, const FVector Location, float Volume = 1.f, float Pitch = 1.f);

	// Play a random sound that is attached to a scene component
	UFUNCTION(BlueprintCallable)
	static void PlaySoundAttached(USceneComponent * AttachToComponent, TArray<USoundBase *> & Sounds, FVector Location, float Volume = 1.f, float Pitch = 1.f);

	// Play a sound that is attached to a scene component
	UFUNCTION(BlueprintCallable)
	static void PlayASoundAttached(USceneComponent * AttachToComponent, USoundBase * Sound, FVector Location, float Volume = 1.f, float Pitch = 1.f);

	// Play a sound when in the main menu. Does not use master volume. This is so other sounds can be muted while 
	// in main menu
	UFUNCTION(BlueprintCallable)
	static UAudioComponent * PlayMenuSound(const UObject * Object, const TArray<USoundBase*> & Sounds, const FVector Location, float Volume = 1.f, float Pitch = 1.f);

	// Lower/raise master volume
	UFUNCTION(BlueprintCallable)
	static void AdjustMasterVolume(float Amount);

	UFUNCTION(BlueprintCallable)
	static float GetMasterVolume();

	UFUNCTION(BlueprintCallable)
	static void SetMasterVolume(float NewVolume);

	UFUNCTION(BlueprintCallable)
	static void AdjustMenuVolume(float Amount);

	UFUNCTION(BlueprintCallable)
	static float GetMenuVolume();

	UFUNCTION(BlueprintCallable)
	static void SetMenuVolume(float NewAmount);


protected:

	// Global volume of sound when not in main menu
	static float SoundVolume;

	// Sound level when in main menu
	static float MenuVolume;

	// Volume limit
	static float VolumeLimit;

};
