// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Extreme_Bot.h"
#include "Extreme_BotPatrolPoint.h"
#include "Extreme_BotController.generated.h"


/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AExtreme_BotController : public AAIController
{
	GENERATED_UCLASS_BODY()
	
private:

	UPROPERTY(transient)
	UBlackboardComponent* BlackboardComp;

	/* Cached BT component */
	UPROPERTY(transient)
	UBehaviorTreeComponent* BehaviorComp;
	
public:

	// Begin AController interface
	virtual void Possess(class APawn* InPawn) override;
	virtual void UnPossess() override;
	virtual void BeginInactiveState() override;
	// End APlayerController interface

	/* May not need these functions but will leave them here for now*/
	void SetEnemy(class APawn* InPawn);

	AFPS_ProjectCharacter * GetEnemy() const;

	UFUNCTION(BlueprintCallable, Category = Behavior)
	bool FindClosestEnemyWithLOS(AFPS_ProjectCharacter* ExcludeEnemy);

	bool HasWeaponLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const;

	// Begin AAIController interface
	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	// End AAIController interface

protected:
	// Check of we have LOS to a character
	bool LOSTrace(AFPS_ProjectCharacter* InEnemyChar) const;

	int32 EnemyKeyID;
	int32 NeedRestID;	// probably don't need this

	/** Things for patrolling */

	/* Blackboard keys for patrolling */
	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName LocationToGoKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName PlayerKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName CoverLocation;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName WaitTime;

	/* Array of patrol points */
	TArray<AExtreme_BotPatrolPoint*> PatrolPoints;

public:
	/** Returns BlackboardComp subobject **/
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }
	/* Returns array of patrol points */
	FORCEINLINE TArray<AExtreme_BotPatrolPoint*> GetPatrolPoints() const { return PatrolPoints; }

	// Added this myself. A reference to the bot using this controller
	UPROPERTY(BlueprintReadWrite)
	AExtreme_Bot* OwningBot;

	/** Fields for patrolling */
	int32 CurrentPatrolPoint = 0;

	void SetPlayerCaught(APawn* Pawn);

	// A fucntion that makes the bot sprint as fast as they can
	UFUNCTION(BlueprintCallable)
	void SprintForCover(FVector & Location);


























	
};
