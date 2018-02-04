// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Ver2_PatrolPoint.h"
#include "Ver2_Controller.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AVer2_Controller : public AAIController
{
	GENERATED_UCLASS_BODY()
	
public:

	/* Reference to blackboard */
	UPROPERTY(transient)
	UBlackboardComponent* BlackboardComp;

	/* Reference to behavior tree */
	UPROPERTY(transient)
	UBehaviorTreeComponent* BehaviorComp;
	
	/* Reference to owning pawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AVer2_Bot* OwningBot;

	// Begin AController interface
	virtual void Possess(class APawn* InPawn) override;
	virtual void UnPossess() override;
	virtual void BeginInactiveState() override;
	// End APlayerController interface

	// Begin AAIController interface
	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	// End AAIController interface


	// Array of patrol points
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AVer2_PatrolPoint*> PatrolPoints;

	// Current patrol point to move to
	int32 CurrentPatrolPoint = 0;

	// Starts bot behavior
	void StartBehavior();

	/** Returns BlackboardComp subobject **/
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }
	/* Returns array of patrol points */
	FORCEINLINE TArray<AVer2_PatrolPoint*> GetPatrolPoints() const { return PatrolPoints; }

};
