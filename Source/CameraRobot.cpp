// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "CameraRobot.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "MyStatics.h"


// Sets default values
ACameraRobot::ACameraRobot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	Camera->bUsePawnControlRotation = true;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	HUDMessage = FString("WASD: Control Camera Robot\nArrow keys: Control player\n0: Stop using robot");

}

// Called when the game starts or when spawned
void ACameraRobot::BeginPlay()
{
	Super::BeginPlay();

	RobotController = GetController();

	WorldSettings = GetWorldSettings();

	if (RobotController)
	{
		RobotController->SetControlRotation(FRotator(-18.f, 0.f, 0.f) + RobotController->GetControlRotation());
	}

	// Make camera overlay visible
	if (CameraOverlayClass && !CameraOverlay)
	{

		UWorld* World = GetWorld();
		if (World)
		{
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);

			if (PlayerController)
			{
				CameraOverlay = CreateWidget<UUserWidget>(PlayerController, CameraOverlayClass);

				if (CameraOverlay)
				{
					CameraOverlay->AddToViewport();

				}
			}
		}
		
	}

	UCharacterMovementComponent* RobotMoveComp = GetCharacterMovement();
	if (RobotMoveComp)
	{
		RobotMoveComp->MaxFlySpeed = FlySpeed;
	}

	HUDMessage = FString("WASD: Control robot\nArrow keys: Control player\nShift: Move faster\n0: Return control to player");

}

// Called every frame
void ACameraRobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Make sure robot is not too low or high
	if (GetActorLocation().Z > MaxHeight)
	{
		FVector Loc = GetActorLocation();
		SetActorLocation(FVector(Loc.X, Loc.Y, MaxHeight));
	}
	else
	{
		if (WorldSettings)
		{
			if (GetActorLocation().Z < WorldSettings->KillZ + 200.f)
			{
				SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, WorldSettings->KillZ + 200.f));
			}
		}
	}

	// Clamp pitch angle. Does not work
	//FRotator Rot = GetActorRotation();
	//if (Rot.Pitch > 80.f)
	//{
	//	SetActorRotation(FRotator(80.f, Rot.Yaw, Rot.Roll));
	//}
	//else if (Rot.Pitch < -80.f)
	//{
	//	SetActorRotation(FRotator(-80.f, Rot.Yaw, Rot.Roll));
	//}

}

// Called to bind functionality to input
void ACameraRobot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACameraRobot::ChangeYaw(float Rate)
{
	if (RobotController)
	{
		float ClampedRate = FRotator::ClampAxis(Rate);
	
		FRotator NewRotation = FRotator(0.f, ClampedRate, 0.f) + RobotController->GetControlRotation();

		RobotController->SetControlRotation(NewRotation);

	}
	
}

void ACameraRobot::ChangePitch(float Rate)
{
	if (RobotController)
	{
		float ClampedRate = FRotator::ClampAxis(Rate);
		
		FRotator NewRotation = FRotator(ClampedRate, 0.f, 0.f) + RobotController->GetControlRotation();
		
		RobotController->SetControlRotation(NewRotation);

	}
	
}

