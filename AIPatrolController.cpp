// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"

// includes added by me
#include "AIPatrol.h"
#include "AIPatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "AIPatrolController.h"


AAIPatrolController::AAIPatrolController()
{
	/* Initialize blackboard and behavior tree */
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	/* Initialze blackboard keys */
	PlayerKey = "Target";
	LocationToGoKey = "LocationToGo";

	CurrentPatrolPoint = 0;

}

void AAIPatrolController::SetPlayerCaught(APawn * Pawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(PlayerKey, Pawn);
	}

}

void AAIPatrolController::Possess(APawn * Pawn)
{
	Super::Possess(Pawn);

	/* Get reference to the character*/
	AAIPatrol* AICharacter;
	if (Pawn)
	{
		AICharacter = Cast<AAIPatrol>(Pawn);
	}
	else
	{
		AICharacter = nullptr;
	}

	/* Check if cast was succesful */
	if (AICharacter)
	{
		/* Check if blackboard is not null */
		if (AICharacter->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*(AICharacter->BehaviorTree->BlackboardAsset));
		}

		// Temp array that holds all patrol points
		TArray<AActor*> AllPatrolPoints;
		
		/* Populate patrol point array */
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIPatrolPoint::StaticClass(), AllPatrolPoints);

		// Only assign points that have the correct ID
		for (AActor* Actor : AllPatrolPoints)
		{
			AAIPatrolPoint* Point = Cast<AAIPatrolPoint>(Actor);
			if (Point->ID == AICharacter->PatrolPointID)
			{
				PatrolPoints.Add(Point);
			}
		}

		BehaviorComp->StartTree(*AICharacter->BehaviorTree);

	}

}




