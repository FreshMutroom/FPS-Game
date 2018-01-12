// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "MyStatics.h"

DEFINE_LOG_CATEGORY(DebugMsgLogCategory);	// Needed for logging messages to file


// Default value for sound volume
float UMyStatics::SoundVolume = 0.0f;

float UMyStatics::MenuVolume = 0.0f;

// Volume limit only for when playing sounds via functions. The master volume limit is 1.0f
float UMyStatics::VolumeLimit = 1.5f;


void UMyStatics::DebugMsg(const FString Message, const FColor Color)
{

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.f, Color, Message);

		// Log message to file
		UE_LOG(DebugMsgLogCategory, Log, TEXT("%s"), *Message);

	}

}

FVector UMyStatics::CrossProduct(FVector & V1, FVector & V2)
{
	
	
	return FVector();
}

void UMyStatics::PlaySound(const UObject * Object, const TArray<USoundBase*> & Sounds, const FVector Location, float Volume, float Pitch)
{
	if (Volume > UMyStatics::VolumeLimit)
	{
		Volume = UMyStatics::VolumeLimit;
	}
	else if (Volume < 0.f)
	{
		Volume = 0.f;
	}

	if (Pitch > 2.f)
	{
		Pitch = 2.f;
	}

	int32 Size = Sounds.Num();

	if (Size == 0 || !Object)
	{
		return;
	}
	
	int32 Index = FMath::RandRange(0, Size - 1);

	if (Sounds.IsValidIndex(Index))
	{
		UGameplayStatics::PlaySoundAtLocation(Object, Sounds[Index], Location, UMyStatics::SoundVolume * Volume, Pitch);
	}

}

void UMyStatics::PlayASound(const UObject * Object, USoundBase * Sound, const FVector Location, float Volume, float Pitch)
{
	if (Object && Sound)
	{	
		if (Volume > UMyStatics::VolumeLimit)
		{
			Volume = UMyStatics::VolumeLimit;
		}
		else if (Volume < 0.f)
		{
			Volume = 0.f;
		}

		if (Pitch > 2.f)
		{
			Pitch = 2.f;
		}

		UGameplayStatics::PlaySoundAtLocation(Object, Sound, Location, UMyStatics::SoundVolume * Volume, Pitch);
	}

}

void UMyStatics::PlaySoundAttached(USceneComponent * AttachToComponent, TArray<USoundBase*>& Sounds, FVector Location, float Volume, float Pitch)
{
	if (Volume > UMyStatics::VolumeLimit)
	{
		Volume = UMyStatics::VolumeLimit;
	}
	else if (Volume < 0.f)
	{
		Volume = 0.f;
	}

	if (Pitch > 2.f)
	{
		Pitch = 2.f;
	}

	int32 Size = Sounds.Num();

	if (Size == 0 || !AttachToComponent)
	{
		return;
	}

	int32 Index = FMath::RandRange(0, Size - 1);

	if (Sounds.IsValidIndex(Index))
	{
		UGameplayStatics::PlaySoundAttached(Sounds[Index], AttachToComponent, NAME_None, Location, EAttachLocation::KeepRelativeOffset, false, UMyStatics::SoundVolume * Volume, Pitch);
	}

}

void UMyStatics::PlayASoundAttached(USceneComponent * AttachToComponent, USoundBase * Sound, FVector Location, float Volume, float Pitch)
{
	if (AttachToComponent && Sound)
	{
		if (Volume > UMyStatics::VolumeLimit)
		{
			Volume = UMyStatics::VolumeLimit;
		}
		else if (Volume < 0.f)
		{
			Volume = 0.f;
		}

		if (Pitch > 2.f)
		{
			Pitch = 2.f;
		}

		UGameplayStatics::PlaySoundAttached(Sound, AttachToComponent, NAME_None, Location, EAttachLocation::KeepRelativeOffset, false, UMyStatics::SoundVolume * Volume, Pitch);
	}

}

UAudioComponent * UMyStatics::PlayMenuSound(const UObject * Object, const TArray<USoundBase*> & Sounds, const FVector Location, float Volume, float Pitch)
{
	if (Volume > UMyStatics::VolumeLimit)
	{
		Volume = UMyStatics::VolumeLimit;
	}
	else if (Volume < 0.f)
	{
		Volume = 0.f;
	}

	if (Pitch > 2.f)
	{
		Pitch = 2.f;
	}

	int32 Size = Sounds.Num();

	if (Size == 0 || !Object)
	{
		return nullptr;
	}

	int32 Index = FMath::RandRange(0, Size - 1);

	if (Sounds.IsValidIndex(Index))
	{
		float StartVolume = UMyStatics::MenuVolume * Volume;
		if (StartVolume <= 0.f)
		{
			StartVolume = 0.1f;
		}

		UAudioComponent * Component = UGameplayStatics::SpawnSound2D(Object, Sounds[Index], StartVolume, Pitch);
		
		return Component;
	
	}

	return nullptr;
}

void UMyStatics::AdjustMasterVolume(float Amount)
{
	UMyStatics::SoundVolume += Amount;
	
	if (UMyStatics::SoundVolume > 1.f)
	{
		UMyStatics::SoundVolume = 1.f;
	}
	else if (UMyStatics::SoundVolume < 0.f)
	{
		UMyStatics::SoundVolume = 0.f;
	}

}

float UMyStatics::GetMasterVolume()
{
	return UMyStatics::SoundVolume;
}

void UMyStatics::SetMasterVolume(float NewVolume)
{
	if (NewVolume > 1.f)
	{
		NewVolume = 1.f;
	}
	else if (NewVolume < 0.f)
	{
		NewVolume = 0.f;
	}
	
	UMyStatics::SoundVolume = NewVolume;

}

void UMyStatics::AdjustMenuVolume(float Amount)
{
	UMyStatics::MenuVolume += Amount;

	if (UMyStatics::MenuVolume > 1.f)
	{
		UMyStatics::MenuVolume = 1.f;
	}
	else if (UMyStatics::MenuVolume < 0.f)
	{
		UMyStatics::MenuVolume = 0.f;
	}

}

float UMyStatics::GetMenuVolume()
{
	return UMyStatics::MenuVolume;

}

void UMyStatics::SetMenuVolume(float NewAmount)
{
	if (NewAmount < 0.f)
	{
		UMyStatics::MenuVolume = 0.f;

	}
	else if (NewAmount > 1.f)
	{
		UMyStatics::MenuVolume = 1.f;
	}
	else
	{
		UMyStatics::MenuVolume = NewAmount;
	}

}


