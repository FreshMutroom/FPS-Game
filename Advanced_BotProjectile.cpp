// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Advanced_BotProjectile.h"
#include "FPS_ProjectCharacter.h"


// Sets default values
AAdvanced_BotProjectile::AAdvanced_BotProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Projectile Particles"));
	ProjectileParticles->AttachTo(RootComponent);

}

// Called when the game starts or when spawned
void AAdvanced_BotProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	AFPS_ProjectCharacter* Player = Cast<AFPS_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	const FVector StartTrace = GetActorLocation();
	FVector EndTrace = GetActorForwardVector() + StartTrace;

	ProjectileParticles->SetBeamSourcePoint(1, StartTrace, 0);
	ProjectileParticles->SetBeamTargetPoint(1, EndTrace, 0);

}

// Called every frame
void AAdvanced_BotProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

