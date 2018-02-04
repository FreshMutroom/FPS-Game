// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "BotStartLocation.h"



ABotStartLocation::ABotStartLocation()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->bGenerateOverlapEvents = false;
	Mesh->SetHiddenInGame(true);
	RootComponent = Mesh;
}




void ABotStartLocation::SpawnBot()
{
	UWorld * World = GetWorld();
	if (World)
	{
		switch (ID)
		{
		case 0:
			if (BP_AIPatrol)
			{
				World->SpawnActor<AAIPatrol>(BP_AIPatrol, GetActorLocation(), GetActorRotation());
			}
			break;

		case 1:
			if (BP_Advanced_Bot)
			{
				World->SpawnActor<AAdvanced_Bot>(BP_Advanced_Bot, GetActorLocation(), GetActorRotation());
			}
			break;

		case 2:
			if (BP_Ver2_Bot)
			{
				World->SpawnActor<AVer2_Bot>(BP_Ver2_Bot, GetActorLocation(), GetActorRotation());
			}
			break;

		case 3:
			if (BP_Ver2_RangeBot)
			{
				World->SpawnActor<AVer2_Bot>(BP_Ver2_RangeBot, GetActorLocation(), GetActorRotation());
			}
			break;

		default:
			break;

		}
	}

}
