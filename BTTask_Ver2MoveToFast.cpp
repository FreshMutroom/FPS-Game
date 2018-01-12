// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "BTTask_Ver2MoveToFast.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Ver2_Controller.h"
#include "Ver2_Bot.h"


EBTNodeResult::Type UBTTask_Ver2MoveToFast::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AVer2_Controller* Controller = Cast<AVer2_Controller>(OwnerComp.GetAIOwner());
	
	if (Controller)
	{
		UBlackboardComponent* BlackboardComp = Controller->GetBlackboardComp();

		if (BlackboardComp)
		{
			FVector Location = BlackboardComp->GetValueAsVector("CoverLocation");

			// Change movement speed, move to location, then change movement speed back
			if (Controller->OwningBot)
			{
				UMovementComponent* MovementComp = Controller->OwningBot->GetMovementComponent();
			
				if (MovementComp)
				{
					UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(MovementComp);

					if (CharMoveComp)
					{
						CharMoveComp->MaxWalkSpeed = Controller->OwningBot->RunSpeed;

						Controller->MoveToLocation(Location, 5.f);

						CharMoveComp->MaxWalkSpeed = Controller->OwningBot->CrouchMovementSpeed;

					}

				}

			}

		}

	}

	return EBTNodeResult::Succeeded;	// to compile
}

