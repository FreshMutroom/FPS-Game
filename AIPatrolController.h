// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"	// added by me
#include "AIPatrolController.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AAIPatrolController : public AAIController
{
	GENERATED_BODY()

	/* Behavior tree component */
	UBehaviorTreeComponent* BehaviorComp;

	/* Our blackboard component */
	UBlackboardComponent* BlackboardComp;

	/* Blackboard keys */
	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName LocationToGoKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName PlayerKey;

	TArray<AActor*> PatrolPoints;

	virtual void Possess(APawn* Pawn) override;

public:
	AAIPatrolController();
	
	int32 CurrentPatrolPoint = 0;

	void SetPlayerCaught(APawn* Pawn);

	/* Inline getter functions */

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp;  }
	FORCEINLINE TArray<AActor*> GetPatrolPoints() const { return PatrolPoints;  }
};
