// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "MySaveGame.h"
#include "FPS_ProjectCharacter.h"
#include "MyGameInstance.h"
#include "MyStatics.h"


UMySaveGame::UMySaveGame()
{

}


bool UMySaveGame::Save(AFPS_ProjectCharacter* Player)
{

	if (Player)
	{
		PlayerLocation = Player->GetActorLocation();
		
		PlayerRotation = Player->GetActorRotation();

		AController* Controller = Player->GetController();
		if (Controller)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Controller);
			if (PlayerController)
			{
				PlayerCameraRotation = PlayerController->GetControlRotation();
			}

		}
		
		EquippedWeapon = Player->EquippedWeapon;
		OtherWeapon = Player->OtherWeapon;
		WeaponAmmoValues = Player->WeaponAmmoValues;

		bHasKeyCard = Player->bHasKeyCard;

		


		return true;
	}
	
	return false;
}

bool UMySaveGame::Load(AFPS_ProjectCharacter* Player)
{
	if (Player)
	{
		// Load location/rotation
		Player->SetActorLocation(PlayerLocation);
		
		Player->SetActorRotation(PlayerRotation);

		AController* Controller = Player->GetController();
		if (Controller)
		{
			Controller->SetControlRotation(PlayerCameraRotation);
		}


		// Load weapon values
		Player->EquippedWeapon = EquippedWeapon;
		Player->OtherWeapon = OtherWeapon;
		Player->WeaponAmmoValues = WeaponAmmoValues;

		// Make sure to adjust players rate of fire, damage, bIsWeaponAutomatic, weapon mesh etc
		Player->AdjustWeaponValuesOnLoad();

		// Load keycard value
		Player->bHasKeyCard = bHasKeyCard;

		// Change a whole lot of booleans to safe values. These were never saved, they just reset to safe values
		Player->AdjustValuesOnLoad();

		// Despawn then spawn all AI
		UWorld * World = GetWorld();
		if (World)
		{
			UGameInstance * GameInstance = UGameplayStatics::GetGameInstance(World);
			if (GameInstance)
			{
				UMyGameInstance * MyGameInstance = Cast<UMyGameInstance>(GameInstance);
				if (MyGameInstance)
				{
					MyGameInstance->SpawnAI();

					

				}
			}
		}

		return true;

	}
	
	return false;
}

void UMySaveGame::SaveSettings()
{
	//MasterAudioVolume;
	//
	//EffectsAudioVolume;
	//
	//MusicAudioVolume;
	//
	//MouseSensitivity;
	//
	//
	//WindowedMode;
	//
	//Resolution;
	//
	//FramerateLimit;

	//VerticalSync
	//
	//QualityPreset
	//
	//Textures
	//
	//DetailMode
	//
	//ShadowQuality
	//
	//ViewDistance
	//
	//AAMode
	//AAQuality
	//
	//EffectsQuality
	//
	//Foliage
	//
	//PostProcessing

	MasterAudioVolume = UMyStatics::GetMasterVolume();

	UGameInstance * GameInstance = UGameplayStatics::GetGameInstance(this);
	if (GameInstance)
	{
		UMyGameInstance * MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			MouseSensitivity = MyGameInstance->MouseSensitivity;

			DifficultySetting = MyGameInstance->DamageMultiplier;

		}

	}

}

void UMySaveGame::LoadSettings()
{
	UGameInstance * GameInstance = UGameplayStatics::GetGameInstance(this);
	if (GameInstance)
	{
		UMyGameInstance * MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			// Load mouse sensitivity
			MyGameInstance->MouseSensitivity = MouseSensitivity;

			// Load difficulty setting
			MyGameInstance->DamageMultiplier = DifficultySetting;

		}
	}

	// Load audio volume
	UMyStatics::SetMasterVolume(MasterAudioVolume);

}
