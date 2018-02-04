// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "Runtime/UMG/Public/Components/SpinBox.h"
#include "GameStartGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AGameStartGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;	// added by me


public:

	AGameStartGameMode();
	

	/* Reference to player controller used only for menus */
	//UPROPERTY()
	//class AMyPlayerController * Control;

	/* A reference to the game instance */
	UPROPERTY()
	class UMyGameInstance * GameInstance;

	/* Timer handle that keeps track of when map has loaded */
	FTimerHandle HasLoadedTimerHandle;

	/* Time handle for loading screen */
	FTimerHandle LoadingScreenTimerHandle;
	
	/* Constantly check whether to show loading screen */
	UFUNCTION()
	void CheckForLoadingScreen();

	/* Called when the landscape has loaded. Not actually true right now - it just gets 
	   called after a certain amount of time */
	void OnMapLoad();

	/* True if the map has loaded */
	bool bHasMapLoaded;


	/** Static methods */

	// Returns a array of FStrings that are resolutions in the format "1920x1080" etc. 
	// This function shouldn't really be in this class
	UFUNCTION(BlueprintCallable)
	static TArray<FString> ChangeResolutionComboBox();
	
	// Changes a single FIntPoint.ToString() to the desired format for console commands
	// e.g. "X=1920 Y=1080" gets changed to "1920x1080". Once again this function should not be in this class
	UFUNCTION(BlueprintCallable)
	static FString ToConsoleCommandFormat(FIntPoint IntPoint);

	// Changes the format found above back into a FIntPoint
	UFUNCTION(BlueprintCallable)
	static FIntPoint ToResolutionFormat(const FString Resolution);

	// Helper function for the above method
	static int32 AddToResolution(int32 j, int32 StringLength, int32 Number);

	// Yet another static function that should not be here. Iterates over a FString input and 
	// adds a newline every CharPerLine character. Useful for text 
	// boxes that try to print everything on one line
	UFUNCTION(BlueprintCallable)
	static FString CreateMultilineText(FString InString, int32 CharPerLine);

	// Changes the value of the spinbox
	UFUNCTION(BlueprintCallable)
	static void SetSpinboxValue(USpinBox * SpinBox, float Value);


protected:
	// The things below are added for the HUD
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> PlayerHUDClass;

public:

	UPROPERTY(EditAnywhere)	// to enable garbage collection
	class UUserWidget* CurrentWidget;

	void SetupHUD();

};
