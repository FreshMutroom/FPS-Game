// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Extreme_BotController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"


/* This kind of constructor is needed if using GENERATED_UCLASS_BODY() in the .h file
instead of GENERATED_BODY() */
AExtreme_BotController::AExtreme_BotController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));
	
	//BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));

	// BrainComponent: member of AIController.h
	BrainComponent = BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));

	// member of AIController.h
	bWantsPlayerState = true;

	LocationToGoKey = "LocationToGo";
	PlayerKey = "Target";
	WaitTime = "WaitTime";
	//CoverLocation = "CoverLocation";


}

void AExtreme_BotController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);

	OwningBot = Cast<AExtreme_Bot>(InPawn);

	// start behavior
	if (OwningBot && OwningBot->BotBehavior)
	{
		if (OwningBot->BotBehavior->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*OwningBot->BotBehavior->BlackboardAsset);
			BlackboardComp->SetValueAsFloat(WaitTime, 10.0f);
		}

		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		NeedRestID = BlackboardComp->GetKeyID("NeedAmmo");

		// Temp array that holds all patrol points
		TArray<AActor*> AllPatrolPoints;

		/* Populate patrol point array */
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AExtreme_BotPatrolPoint::StaticClass(), AllPatrolPoints);

		// Only assign points that have the correct ID
		for (AActor* Actor : AllPatrolPoints)
		{
			AExtreme_BotPatrolPoint* Point = Cast<AExtreme_BotPatrolPoint>(Actor);
			if (Point->ID == OwningBot->PatrolPointID)
			{
				PatrolPoints.Add(Point);
			}
		}

		BehaviorComp->StartTree(*(OwningBot->BotBehavior));

	}

}

void AExtreme_BotController::UnPossess()
{
	Super::UnPossess();

	BehaviorComp->StopTree();

}

void AExtreme_BotController::BeginInactiveState()
{
	Super::BeginInactiveState();

}

AFPS_ProjectCharacter * AExtreme_BotController::GetEnemy() const
{
	if (BlackboardComp)
	{
		return Cast<AFPS_ProjectCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(EnemyKeyID));
	}

	return NULL;
}

void AExtreme_BotController::SetEnemy(APawn * InPawn)
{
	
	if (BlackboardComp)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, InPawn);

		// function implemented in AIController.h
		SetFocus(InPawn);
	}

}

bool AExtreme_BotController::FindClosestEnemyWithLOS(AFPS_ProjectCharacter * ExcludeEnemy)
{
	return false;
}

bool AExtreme_BotController::HasWeaponLOSToEnemy(AActor * InEnemyActor, const bool bAnyEnemy) const
{
	return false;
}

void AExtreme_BotController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	// Look toward focus
	FVector FocalPoint = GetFocalPoint();
	if (!FocalPoint.IsZero() && GetPawn())
	{
		FVector Direction = FocalPoint - GetPawn()->GetActorLocation();
		FRotator NewControlRotation = Direction.Rotation();

		NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);

		SetControlRotation(NewControlRotation);

		if (OwningBot && bUpdatePawn)
		{
			OwningBot->FaceRotation(NewControlRotation, DeltaTime);
		}

	}

}

bool AExtreme_BotController::LOSTrace(AFPS_ProjectCharacter * InEnemyChar) const
{
	return false;
}

void AExtreme_BotController::SetPlayerCaught(APawn * Pawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(PlayerKey, Pawn);

		// Stop all move commands. The MoveTo for the patrol points is the one this is for.
		//StopMovement();

	}

}

void AExtreme_BotController::SprintForCover(FVector & Location)
{
}
