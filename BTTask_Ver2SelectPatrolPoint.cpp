// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "BTTask_Ver2SelectPatrolPoint.h"
#include "Ver2_PatrolPoint.h"
#include "Ver2_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UBTTask_Ver2SelectPatrolPoint::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AVer2_Controller* AICon = Cast<AVer2_Controller>(OwnerComp.GetAIOwner());

	if (AICon)
	{
		/* Get BB component*/
		UBlackboardComponent* BlackboardComp = AICon->GetBlackboardComp();

		AVer2_PatrolPoint* CurrentPoint = Cast<AVer2_PatrolPoint>(BlackboardComp->GetValueAsObject("PatrolPointLocation"));

		TArray<AVer2_PatrolPoint*> AvailablePatrolPoints = AICon->GetPatrolPoints();

		AVer2_PatrolPoint* NextPatrolPoint = nullptr;

		if (AICon->CurrentPatrolPoint != AvailablePatrolPoints.Num() - 1)
		{
			NextPatrolPoint = AvailablePatrolPoints[++AICon->CurrentPatrolPoint];
		}
		else // if there is not any more points to go to
		{
			NextPatrolPoint = AvailablePatrolPoints[0];
			AICon->CurrentPatrolPoint = 0;
		}

		BlackboardComp->SetValueAsObject("PatrolPointLocation", NextPatrolPoint);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;

}

