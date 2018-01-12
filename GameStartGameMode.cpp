// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "GameStartGameMode.h"
#include "Runtime/UMG/Public/Components/ComboBox.h"
#include "MyPlayerController.h"
#include "MyGameInstance.h"
#include "Runtime/Landscape/Classes/Landscape.h"


AGameStartGameMode::AGameStartGameMode()
{

}

void AGameStartGameMode::CheckForLoadingScreen()
{
	/* Get all landscapes using getallactorsofclass and override their PostLoad fucntion 
	   to make a HAsLoaded boolean true. Also include header file and add Landscape module 
	   to FPS_Project.build.cs */
	
	UWorld * World = GetWorld();
	if (World)
	{	
	
		/* Check if landscape has loaded. Can't do this so just adding a delay */
		if (bHasMapLoaded && GameInstance->bRemoveLoadingScreen)
		{
			GameInstance->ShowMainMenu();

		}
		else
		{
			World->GetTimerManager().SetTimer(LoadingScreenTimerHandle, this, &AGameStartGameMode::CheckForLoadingScreen, 0.1f, false);
		}
			
		return;

		
		
	}

}

void AGameStartGameMode::OnMapLoad()
{
	bHasMapLoaded = true;

}

TArray<FString> AGameStartGameMode::ChangeResolutionComboBox()
{

	TArray<FString> ToReturn;

	TArray<FIntPoint> SupportedResolutions;

	if (UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions))
	{
		for (int i = 0; i < SupportedResolutions.Num(); i++)
		{
			FString StringForm = SupportedResolutions[i].ToString();

			// Format is like so: "X=1920 Y=1080"

			// Remove "X=" from start
			StringForm.RemoveAt(0, 2, true);

			// Check how many digits are in the x part of resolution. Store it in Index
			int32 Index = 0;
			while (StringForm[Index] != *TEXT(" "))
			{
				Index++;
			}

			// Replace space with a "x"
			StringForm = StringForm.Replace(TEXT(" "), TEXT("x"));

			// Remove "Y="
			StringForm.RemoveAt(Index + 1, 2, true);

			ToReturn.Insert(StringForm, i);

		}

	}

	return ToReturn;

}

FString AGameStartGameMode::ToConsoleCommandFormat(FIntPoint IntPoint)
{
	// Format is like so: "X=1920 Y=1080"
	FString StringForm = IntPoint.ToString();

	// Remove "X=" from start
	StringForm.RemoveAt(0, 2, true);

	// Check how many digits are in the x part of resolution. Store it in Index
	int32 Index = 0;
	while (StringForm[Index] != *TEXT(" "))
	{
		Index++;
	}

	// Replace space with a "x"
	StringForm = StringForm.Replace(TEXT(" "), TEXT("x"));

	// Remove "Y="
	StringForm.RemoveAt(Index + 1, 2, true);

	return StringForm;

}

FIntPoint AGameStartGameMode::ToResolutionFormat(const FString Resolution)
{
	if (!Resolution.Contains(FString("x")))
	{
		return FIntPoint();
	}

	int32 X = 0, Y = 0;
	int32 Index = 0;
	for (int i = 0; i < 2; ++i)
	{
		FString String = FString();
		while (Resolution[Index] != *TEXT("x") && Index < Resolution.Len())
		{
			String.Append(&Resolution[Index]);
			++Index;
		}

		int32 Length = String.Len();
		for (int j = 0; j < Length; ++j)
		{
			
			// This is the only way I know how to convert an FString to an int
			int32 NumberToAdd = 0;
			switch (String[j])
			{
			case *TEXT("0"):
				NumberToAdd = 0;
				break;

			case *TEXT("1"):
				NumberToAdd = 1;
				break;

			case *TEXT("2"):
				NumberToAdd = 2;
				break;

			case *TEXT("3"):
				NumberToAdd = 3;
				break;

			case *TEXT("4"):
				NumberToAdd = 4;
				break;

			case *TEXT("5"):
				NumberToAdd = 5;
				break;

			case *TEXT("6"):
				NumberToAdd = 6;
				break;

			case *TEXT("7"):
				NumberToAdd = 7;
				break;

			case *TEXT("8"):
				NumberToAdd = 8;
				break;
			
			case *TEXT("9"):
				NumberToAdd = 9;
				break;

			default:
				break;
			}

			if (i == 0)
			{
				X += AGameStartGameMode::AddToResolution(j, Length, NumberToAdd);
			}
			else
			{
				Y += AGameStartGameMode::AddToResolution(j, Length, NumberToAdd);
			}

		}

	}
	
	return FIntPoint(X, Y);

}

int32 AGameStartGameMode::AddToResolution(int32 j, int32 StringLength, int32 Number)
{
	int32 Multiplier = 0;
	switch (StringLength - j)
	{
	case 1:
		Multiplier = 1;
		break;

	case 2:
		Multiplier = 10;
		break;

	case 3:
		Multiplier = 100;
		break;

	case 4:
		Multiplier = 1000;
		break;
	
	case 5:
		Multiplier = 10000;
		break;

	default: 
		break;
	}
	
	return Multiplier * Number;

}

FString AGameStartGameMode::CreateMultilineText(FString InString, int32 CharPerLine)
{
	
	FString ToReturn = InString;
	
	int32 Length = ToReturn.Len();

	int32 Count = 0;

	int32 IndexOfLastSpace = 0;

	for (int i = 0; i < Length; i++)
	{
		Count++;

		if (ToReturn[i] == *TEXT(" "))
		{
			IndexOfLastSpace = i;
		}
		else if (ToReturn[i] == *TEXT("\n"))
		{
			Count = 0;
		}

		if (Count == CharPerLine)
		{
			
			if (ToReturn[i] == *TEXT(" "))
			{
				// Already on space, just replace with newline
				ToReturn[i] = *TEXT("\n");
			}
			else
			{
				ToReturn[IndexOfLastSpace] = *TEXT("\n");
				i = IndexOfLastSpace;
			}
			
			Count = 0;

		}

	}

	return ToReturn;

}

void AGameStartGameMode::SetSpinboxValue(USpinBox * SpinBox, float Value)
{
	SpinBox->SetValue(Value);

}

void AGameStartGameMode::SetupHUD()
{
	AFPS_ProjectCharacter* MyCharacter = Cast<AFPS_ProjectCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerHUDClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);

		if (CurrentWidget != nullptr)
		{

			CurrentWidget->AddToViewport(-5);

			// Setup pointers for flashing HUD text aniamtion
			if (MyCharacter)
			{
				MyCharacter->HUDWidget = Cast<UVer2_MouseoverDisplay>(CurrentWidget);
				if (MyCharacter->HUDWidget)
				{
					MyCharacter->HUDTextFlashAnimation = MyCharacter->HUDWidget->Animation;
				}

			}

		}

	}

}

void AGameStartGameMode::BeginPlay()
{
	Super::BeginPlay();

	
	/* I think things you want to happen when the game starts for the first time 
	   should go here */

	UWorld * World = GetWorld();
	if (World)
	{
		UGameInstance * GameInst = UGameplayStatics::GetGameInstance(World);
		if (GameInst)
		{
			GameInstance = Cast<UMyGameInstance>(GameInst);
			if (GameInstance)
			{
				GameInstance->Initialize();

				/* Check if first time starting game. Just assume first time starting game for now */
				GameInstance->ShowFirstTimeStartingGameScreen();

				//else
				//{
				// GameInstance->bRemoveLoadingScreen = true;
				// GameInstance->LoadVideoSettingsFromStorage();
				// GameInstance->MakeWidgetActive(EWidgetTypeEnum::Loading);
				//}

				World->GetTimerManager().SetTimer(HasLoadedTimerHandle, this, &AGameStartGameMode::OnMapLoad, 10.f, false);

				CheckForLoadingScreen();
			}
		}
	}



}


