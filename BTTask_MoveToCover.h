// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToCover.generated.h"

/**
* Currently not used
*/
UCLASS()
class FPS_PROJECT_API UBTTask_MoveToCover : public UBTTask_MoveTo
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


};