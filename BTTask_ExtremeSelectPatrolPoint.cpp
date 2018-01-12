// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "BTTask_ExtremeSelectPatrolPoint.h"
#include "Extreme_BotPatrolPoint.h"
#include "Extreme_BotController.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UBTTask_ExtremeSelectPatrolPoint::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AExtreme_BotController* AICon = Cast<AExtreme_BotController>(OwnerComp.GetAIOwner());

	if (AICon)
	{
		/* Get BB component*/
		UBlackboardComponent* BlackboardComp = AICon->GetBlackboardComp();

		AExtreme_BotPatrolPoint* CurrentPoint = Cast<AExtreme_BotPatrolPoint>(BlackboardComp->GetValueAsObject("LocationToGo"));

		TArray<AExtreme_BotPatrolPoint*> AvailablePatrolPoints = AICon->GetPatrolPoints();

		AExtreme_BotPatrolPoint* NextPatrolPoint = nullptr;

		if (AICon->CurrentPatrolPoint != AvailablePatrolPoints.Num() - 1)
		{
			NextPatrolPoint = AvailablePatrolPoints[++AICon->CurrentPatrolPoint];
		}
		else // if there is not any more points to go to
		{
			NextPatrolPoint = AvailablePatrolPoints[0];
			AICon->CurrentPatrolPoint = 0;
		}

		BlackboardComp->SetValueAsObject("LocationToGo", NextPatrolPoint);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;

}
