// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "CameraRobot.generated.h"

UCLASS()
class FPS_PROJECT_API ACameraRobot : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACameraRobot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Camera
	UCameraComponent* Camera;

	// Reference to player
	class AFPS_ProjectCharacter* Player;

	// The max height this robot can fly
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHeight;
	
	// Turn rate when looking around
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseTurnRate;

	// Reference to controller
	AController* RobotController;

	// Reference to world settings
	AWorldSettings* WorldSettings;

	// Change robots yaw
	void ChangeYaw(float Rate);

	// Change robots pitch
	void ChangePitch(float Rate);

	// Camera overlay image blupeprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> CameraOverlayClass;

	// Camera overlay
	UPROPERTY()
	class UUserWidget* CameraOverlay;

	// Text to show on HUD when using camera robot. Overwritten on begin play because I can't add
	// new lines in the editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HUDMessage;

	// Fly speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FlySpeed;

	// Fly speed when holding shift
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoostedFlySpeed;

};
