// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Sniper_Controller.h"

ASniper_Controller::ASniper_Controller(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// member of AIController.h
	bWantsPlayerState = true;
}

void ASniper_Controller::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);

	if (InPawn)
	{
		OwningBot = Cast<ASniper_Bot>(InPawn);
	}

	// Make sure to start behavior here, possibly using a 1 second delay
	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Controller::StartBehavior, 1.0f, false);

	}


}

void ASniper_Controller::UnPossess()
{
	Super::UnPossess();

}

void ASniper_Controller::BeginInactiveState()
{
	Super::BeginInactiveState();

}

void ASniper_Controller::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	// Look toward focus
	FVector FocalPoint = GetFocalPoint();
	if (!FocalPoint.IsZero() && GetPawn())
	{
		FVector Direction = FocalPoint - GetPawn()->GetActorLocation();
		FRotator NewControlRotation = Direction.Rotation();

		NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);

		SetControlRotation(NewControlRotation);

		if (OwningBot && bUpdatePawn)
		{
			OwningBot->FaceRotation(NewControlRotation, DeltaTime);
		}

	}

}

void ASniper_Controller::StartBehavior()
{
	if (OwningBot)
	{
		OwningBot->Init();
	}

}
