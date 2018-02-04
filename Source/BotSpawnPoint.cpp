// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "BotSpawnPoint.h"
#include "AIPatrol.h"
#include "Advanced_Bot.h"
#include "Ver2_Bot.h"


void ABotSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	Index = 0;

}

void ABotSpawnPoint::Start()
{
	BeforeSpawnBot();

}

void ABotSpawnPoint::BeforeSpawnBot()
{
	if (TimeBetweenSpawns < 2.f)
	{
		return;
	}
	
	if (SpawnQueue.IsValidIndex(Index))
	{
		SpawnID = SpawnQueue[Index];
		Index++;
		SpawnBot();

		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ABotSpawnPoint::BeforeSpawnBot, TimeBetweenSpawns, false);

		}

	}

}

void ABotSpawnPoint::SpawnBot()
{
	switch (SpawnID)
	{
	case 0:
		SpawnAIPatrol();
		break;

	case 1: 
		SpawnAdvanced_Bot();
		break;

	case 2:
		SpawnVer2_Bot();
		break;

	default:
		break;

	}

}

void ABotSpawnPoint::SpawnAIPatrol()
{
	UWorld* World = GetWorld();
	if (World && BP_AIPatrol)
	{
		World->SpawnActor<AAIPatrol>(BP_AIPatrol, GetActorLocation(), GetActorRotation());
	}

}

void ABotSpawnPoint::SpawnAdvanced_Bot()
{
	UWorld* World = GetWorld();
	if (World && BP_Advanced_Bot)
	{
		World->SpawnActor<AAdvanced_Bot>(BP_Advanced_Bot, GetActorLocation(), GetActorRotation());
	}
}

void ABotSpawnPoint::SpawnVer2_Bot()
{
	UWorld* World = GetWorld();
	if (World && BP_Ver2_Bot)
	{
		World->SpawnActor<AVer2_Bot>(BP_Ver2_Bot, GetActorLocation(), GetActorRotation());
	}
}
