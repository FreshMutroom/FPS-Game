// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "BTTask_Ver2StartCoverRoutine.h"
#include "Ver2_Bot.h"


EBTNodeResult::Type UBTTask_Ver2StartCoverRoutine::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	
	AController* Controller = Cast<AController>(OwnerComp.GetAIOwner());
	
	if (Controller)
	{
		APawn* OwningPawn = Controller->GetPawn();

		if (OwningPawn)
		{
			AVer2_Bot* Bot = Cast<AVer2_Bot>(OwningPawn);

			if (Bot)
			{
				UCharacterMovementComponent* CharMoveComp = Bot->GetCharacterMovement();

				if (CharMoveComp)
				{
					
					Bot->BeginCoverRoutine();

					return EBTNodeResult::Succeeded;

				}
				
			}

		}

	}

	return EBTNodeResult::Failed;

}
