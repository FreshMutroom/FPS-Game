// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "PhysicsEngine/DestructibleActor.h"	// added by me
#include "Bullet.h"


// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));

	RootComponent = RootComp;

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult HitResult;

	FVector StartTrace = this->GetActorLocation();
	FVector EndTrace = (Velocity * DeltaTime) + StartTrace;
	EndTrace.Z += this->GetActorRotation().Pitch;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// if the line trace hits something it will return true, otherwise false
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Destructible, CollisionParams))
	{
		// hit something
		if (HitResult.GetActor())
		{
			// param5: keep box around forever or make it disappear after some time?
			DrawDebugSolidBox(GetWorld(), HitResult.ImpactPoint, FVector(10.0f), FColor::Blue, true);
			ADestructibleActor* Mesh = Cast<ADestructibleActor>(HitResult.GetActor());

			if (Mesh)
			{
				Mesh->GetDestructibleComponent()->ApplyRadiusDamage(10.0f, HitResult.ImpactPoint, 32.0f, 10.0f, false);
			}
			
		}
		Destroy();

	}
	else
	{
		// did not hit something
		BulletExpiry += DeltaTime;

		// param5: true to keep line drawn forever, false and yo ucan specify a time to keep it drawn
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(0.0f, -BulletExpiry * 80.0f, 100.0f), true);

		// Move bullet to the end of the line trace
		SetActorLocation(EndTrace);

		Velocity += FVector(0.0f, 0.0f, -200.0f) * DeltaTime;

	}

	if (BulletExpiry > 3)
	{
		Destroy();
	}

}

