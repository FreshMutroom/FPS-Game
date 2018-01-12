// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "BTTask_Ver2StartStrafingRoutine.h"
#include "Ver2_Bot.h"


EBTNodeResult::Type UBTTask_Ver2StartStrafingRoutine::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
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
				Bot->StartStandAndStrafeRoutine();

				return EBTNodeResult::Succeeded;

			}

		}

	}

	return EBTNodeResult::Failed;

}

