// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Ver2_Controller.h"
#include "Ver2_Bot.h"


/* This kind of constructor is needed if using GENERATED_UCLASS_BODY() in the .h file
instead of GENERATED_BODY() */
AVer2_Controller::AVer2_Controller(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));

	// BrainComponent: member of AIController.h
	BrainComponent = BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));

	// member of AIController.h
	bWantsPlayerState = true;

}

void AVer2_Controller::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);

	if (InPawn)
	{
		OwningBot = Cast<AVer2_Bot>(InPawn);
	}
	
	if (OwningBot)
	{

		// Temp array that holds all patrol points
		TArray<AActor*> AllPatrolPoints;

		/* Populate patrol point array */
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVer2_PatrolPoint::StaticClass(), AllPatrolPoints);

		// Only assign points that have the correct ID
		for (AActor* Actor : AllPatrolPoints)
		{
			AVer2_PatrolPoint* Point = Cast<AVer2_PatrolPoint>(Actor);
			if (Point->ID == OwningBot->PatrolPointID)
			{
				PatrolPoints.Add(Point);
			}
		}

		// This timer handle is needed otherwise pawn will do nothing
		FTimerHandle StartBehaviorTimerhandle;
		GetWorld()->GetTimerManager().SetTimer(StartBehaviorTimerhandle, this, &AVer2_Controller::StartBehavior, 1.0f, false);
		
	}

}

void AVer2_Controller::UnPossess()
{
	Super::UnPossess();

	if (BehaviorComp)
	{
		BehaviorComp->StopTree();
	}

}

void AVer2_Controller::BeginInactiveState()
{
	Super::BeginInactiveState();

}

void AVer2_Controller::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	if (OwningBot && OwningBot->bHasZeroHealth)
	{
		return;
	}
	
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

void AVer2_Controller::StartBehavior()
{
	if (OwningBot)
	{
		// start behavior
		if (OwningBot->bIsAtFiringRange)
		{
			OwningBot->StartFiringRangeRoutine();
		}
		else
		{
			OwningBot->StartPatrol();
		}
		
	}

}
