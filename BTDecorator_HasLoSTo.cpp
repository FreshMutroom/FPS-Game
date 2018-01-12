// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "BTDecorator_HasLoSTo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Advanced_Bot.h"
#include "Advanced_BotController.h"


UBTDecorator_HasLoSTo::UBTDecorator_HasLoSTo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Has LoS To";
	// accept only actors and vectors	
	EnemyKey.AddObjectFilter(this, *NodeName, AActor::StaticClass());
	EnemyKey.AddVectorFilter(this, *NodeName);
}

bool UBTDecorator_HasLoSTo::CalculateRawConditionValue(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) const
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	AAIController* MyController = OwnerComp.GetAIOwner();
	bool HasLOS = false;

	if (MyController && MyBlackboard)
	{
		auto MyID = MyBlackboard->GetKeyID(EnemyKey.SelectedKeyName);
		auto TargetKeyType = MyBlackboard->GetKeyType(MyID);

		FVector TargetLocation;
		bool bGotTarget = false;
		AActor* EnemyActor = NULL;
		if (TargetKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(MyID);
			EnemyActor = Cast<AActor>(KeyValue);
			if (EnemyActor)
			{
				TargetLocation = EnemyActor->GetActorLocation();
				bGotTarget = true;
			}
		}
		else if (TargetKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(MyID);
			bGotTarget = true;
		}

		if (bGotTarget == true)
		{
			if (LOSTrace(OwnerComp.GetOwner(), EnemyActor, TargetLocation) == true)
			{
				HasLOS = true;
			}
		}
	}

	return HasLOS;

}

bool UBTDecorator_HasLoSTo::LOSTrace(AActor * InActor, AActor * InEnemyActor, const FVector & EndLocation) const
{
	static FName LosTag = FName(TEXT("AILosTrace"));
	AAdvanced_BotController* MyController = Cast<AAdvanced_BotController>(InActor);
	AAdvanced_Bot* MyBot = MyController ? Cast<AAdvanced_Bot>(MyController->GetPawn()) : NULL;

	bool bHasLOS = false;
	{
		if (MyBot != NULL)
		{
			// Perform trace to retrieve hit info
			FCollisionQueryParams TraceParams(LosTag, true, InActor);
			TraceParams.bTraceAsyncScene = true;

			TraceParams.bReturnPhysicalMaterial = true;
			TraceParams.AddIgnoredActor(MyBot);
			const FVector StartLocation = MyBot->GetActorLocation();
			FHitResult Hit(ForceInit);
			GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, TraceParams);	// param4 changed from COLLISION_WEAPON
			if (Hit.bBlockingHit == true)
			{
				// We hit something. If we have an actor supplied, just check if the hit actor is an enemy. If it is consider that 'has LOS'
				AActor* HitActor = Hit.GetActor();
				if (Hit.GetActor() != NULL)
				{
					// If the hit is our target actor consider it LOS
					if (HitActor == InActor)
					{
						bHasLOS = true;
					}
					else
					{
						// Check the team of us against the team of the actor we hit if we are able. If they dont match good to go.
						ACharacter* HitChar = Cast<ACharacter>(HitActor);
						if ((HitChar != NULL)
							&& (MyController->PlayerState != NULL) && (HitChar->PlayerState != NULL))
						{
							bHasLOS = true;
								
						}
					}
				}
				else //we didnt hit an actor
				{
					if (InEnemyActor == NULL)
					{
						// We were not given an actor - so check of the distance between what we hit and the target. If what we hit is further away than the target we should be able to hit our target.
						FVector HitDelta = Hit.ImpactPoint - StartLocation;
						FVector TargetDelta = EndLocation - StartLocation;
						if (TargetDelta.SizeSquared() < HitDelta.SizeSquared())
						{
							bHasLOS = true;
						}
					}
				}
			}
		}
	}

	return bHasLOS;
}
