// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "BTTask_MoveToCover.h"
#include "Extreme_BotController.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UBTTask_MoveToCover::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AExtreme_BotController* AICon = Cast<AExtreme_BotController>(OwnerComp.GetAIOwner());
	

	if (AICon)
	{
		AExtreme_Bot* Bot = Cast<AExtreme_Bot>(AICon->GetPawn());
		
		/* Get BB component*/
		UBlackboardComponent* BlackboardComp = AICon->GetBlackboardComp();

		if (Bot && BlackboardComp)
		{

			
			// This will make Bot->CoverLocation the correct location but it might run in parallel
			// to the rest of this code, so the location might not be ready by the time the move 
			// command is executed below
			//Bot->RunFindCoverEQS();
			
			

			// With this if the cover target location changes in the blackboard then the bot will 
			// instantly start moving towards the new location
			bObserveBlackboardValue = true;

			// Set bot run speed to fast
			UMovementComponent* MovementComp = Bot->GetMovementComponent();
			UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(MovementComp);
			CharMovement->MaxWalkSpeed = 600.f;
			
			FAIMoveRequest MoveRequest;
			
			// This location may be set correctly
			//MoveRequest.SetGoalLocation(Bot->CoverLocation);
			


			//BlackboardComp->SetValueAsObject("LocationToGo", NextPatrolPoint);

			return EBTNodeResult::Succeeded;

		}

		

		
	}

	return EBTNodeResult::Failed;

}






