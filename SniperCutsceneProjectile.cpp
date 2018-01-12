// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "SniperCutsceneProjectile.h"


// Sets default values
ASniperCutsceneProjectile::ASniperCutsceneProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SniperTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail  Particles"));

	InitialLifeSpan = 20.f;

	StartTrace = FVector(-50311.f, 18842.f, -1896.f);

}

// Called when the game starts or when spawned
void ASniperCutsceneProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (SniperTrail)
	{
		SniperTrail->SetBeamSourcePoint(0, StartTrace, 0);
		SniperTrail->SetBeamTargetPoint(0, EndTrace, 0);
	}

}

// Called every frame
void ASniperCutsceneProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

