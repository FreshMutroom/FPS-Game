// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Advanced_BotController.h"
#include "FPS_ProjectCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"


/* This kind of constructor is needed if using GENERATED_UCLASS_BODY() in the .h file
   instead of GENERATED_BODY() */
AAdvanced_BotController::AAdvanced_BotController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// member of AIController.h
	bWantsPlayerState = true;

}

void AAdvanced_BotController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);

	// Extra line I added to keep the reference to the owning bot.
	// Should delete the function casts and just use this
	if (InPawn)
	{
		OwningBot = Cast<AAdvanced_Bot>(InPawn);
	}
	
	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AAdvanced_BotController::StartBehavior, 1.0f, false);

	}

}

void AAdvanced_BotController::UnPossess()
{
	Super::UnPossess();

}

void AAdvanced_BotController::BeginInactiveState()
{
	Super::BeginInactiveState();

}

void AAdvanced_BotController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	// Look toward focus
	FVector FocalPoint = GetFocalPoint();
	if (!FocalPoint.IsZero() && GetPawn())
	{
		FVector Direction = FocalPoint - GetPawn()->GetActorLocation();
		FRotator NewControlRotation = Direction.Rotation();

		NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);

		SetControlRotation(NewControlRotation);

		APawn* const P = GetPawn();
		if (P && bUpdatePawn)
		{
			P->FaceRotation(NewControlRotation, DeltaTime);
		}

	}

}

void AAdvanced_BotController::StartBehavior()
{
	if (OwningBot)
	{
		OwningBot->Init();
	}
}



