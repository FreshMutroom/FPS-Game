// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "MyPlayerController.h"
#include "GameStartGameMode.h"
#include "MyGameInstance.h"


void AMyPlayerController::BeginPlay()
{
	/* Disable player input - only allow menu navigation */
	EnableMainMenuMode();

	SetupInput();


}

void AMyPlayerController::SetupInput()
{
	EnableInput(this);

	check(InputComponent);

	FInputActionBinding Input = InputComponent->BindAction("SkipCutscene", IE_Pressed, this, &AMyPlayerController::SkipCutscene);
	
	Input.bConsumeInput = false;

}

void AMyPlayerController::SkipCutscene()
{
	if (CheckGameInstance())
	{
		GameInstance->StopOpeningMovie();
	}

}

bool AMyPlayerController::CheckGameInstance()
{
	if (GameInstance)
	{
		return true;
	}
	else
	{
		UGameInstance * GameInst = UGameplayStatics::GetGameInstance(this);
		if (GameInst)
		{
			GameInstance = Cast<UMyGameInstance>(GameInst);
			if (GameInstance)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		return false;

	}
	
}

void AMyPlayerController::EnableMainMenuMode()
{
	UnPossess();
	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;

	HideHUD();

}

void AMyPlayerController::HideHUD()
{
	AGameModeBase * GameMode = UGameplayStatics::GetGameMode(this);
	if (GameMode)
	{
		AGameStartGameMode * MyGameMode = Cast<AGameStartGameMode>(GameMode);
		if (MyGameMode)
		{
			if (MyGameMode->CurrentWidget)
			{
				MyGameMode->CurrentWidget->RemoveFromViewport();
			}
		}
	}

}
