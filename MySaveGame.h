// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

/*
	Enum for type of AI
*/
UENUM(BlueprintType)
enum class EAIType : uint8
{
	AIPatrol,
	Advanced_Bot,
	Ver2_Bot,
};


/*
	Structure that holds information about AI
*/
USTRUCT()
struct FAIData
{
	GENERATED_USTRUCT_BODY()

	// Location of AI
	UPROPERTY(EditAnywhere)
	FVector Location;

	// Current health
	UPROPERTY(EditAnywhere)
	float Health;
	
	// Type of AI
	UPROPERTY(EditAnywhere)
	EAIType Type;

};



/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UMySaveGame();
	
	// Returns true if save is successful
	bool Save(AFPS_ProjectCharacter* Player);

	// Returns true if load is successful
	bool Load(AFPS_ProjectCharacter* Player);


	/** All the things to save */

	// Player values. Everything must be a UPROPERTY for it to save correctly

	UPROPERTY()
	FVector PlayerLocation;

	UPROPERTY()
	FRotator PlayerRotation;

	UPROPERTY()
	FRotator PlayerCameraRotation;

	UPROPERTY()
	uint8 EquippedWeapon;

	UPROPERTY()
	uint8 OtherWeapon;

	UPROPERTY()
	TArray<int> WeaponAmmoValues;

	UPROPERTY()
	bool bHasKeyCard;


	// AI values

	// Holds all info about all AI
	UPROPERTY(EditAnywhere)
	TArray<struct FAIData> AIInfo;


	/** Fields for saving video/audio/controls settings */

	// Save video/audio/controls settings. Should be called when "Apply" is clicked 
	// in settings menu
	void SaveSettings();

	// Load video/audio/controls settings. Should be called when starting up game
	void LoadSettings();


	// Settings to save
	
	UPROPERTY()
	float MasterAudioVolume;
		
	UPROPERTY()
	float MouseSensitivity;
		
	UPROPERTY()
	float DifficultySetting;

		
	UPROPERTY()
	int8 WindowedMode;
		
	UPROPERTY()
	FString Resolution;
		
	UPROPERTY()
	float FramerateLimit;
		
	UPROPERTY()
	bool VerticalSync;

	UPROPERTY()
	int8 QualityPreset;

	UPROPERTY()
	int8 Textures;

	UPROPERTY()
	int8 DetailMode;
		
	UPROPERTY()
	int8 ShadowQuality;
		
	UPROPERTY()
	int8 ViewDistance;
		
	UPROPERTY()
	int8 AAMode;
		
	UPROPERTY()
	int8 AAQuality;
		
	UPROPERTY()
	int8 EffectsQuality;

	UPROPERTY()
	int8 Foliage;

	UPROPERTY()
	int8 PostProcessing;

};


