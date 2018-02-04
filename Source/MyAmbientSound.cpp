// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "MyAmbientSound.h"
#include "MyStatics.h"




void AMyAmbientSound::BeginPlay()
{
	Play();

}

void AMyAmbientSound::Update()
{
	UAudioComponent * AC = GetAudioComponent();
	if (AC)
	{
		float NewVolume = ((MaxVolume - MinVolume) * UMyStatics::GetMasterVolume()) + MinVolume;
		
		AC->SetVolumeMultiplier(NewVolume);

	}

}
