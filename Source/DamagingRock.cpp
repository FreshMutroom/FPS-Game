// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "DamagingRock.h"
#include "Runtime/Engine/Classes/PhysicsEngine/DestructibleActor.h"
#include "Projectile.h"
#include "Item.h"


// Sets default values
ADamagingRock::ADamagingRock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SM_DamagingRock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SM_DamagingRock->SetSimulatePhysics(true);
	SM_DamagingRock->SetEnableGravity(true);
	SM_DamagingRock->WakeRigidBody();
	SM_DamagingRock->OnComponentHit.AddDynamic(this, &ADamagingRock::OnHit);
	RootComponent = SM_DamagingRock;

	Health = 0.1f;
	Damage = 30.0f;
	bIsCollidingWithPlayer = false;
	DamagingSpeed = 950000.f;

}

// Called when the game starts or when spawned
void ADamagingRock::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADamagingRock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TicksSinceDMSpawn > -1)
	{
		TicksSinceDMSpawn--;
		if (TicksSinceDMSpawn == 0)
		{
			// Disable collision
			//SM_DamagingRock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//TicksSinceDMSpawn = -1;

			// Destroy a better option I think
			Destroy();

		}
	}

}

void ADamagingRock::Explode(FVector NormalImpulse)
{
	// Swap the static mesh to a destructible mesh and make it fracture

	// First make static mesh invisible
	if (SM_DamagingRock)
	{
		SM_DamagingRock->SetVisibility(false);
	}

	
	// Spawn destructible mesh in its place
	UWorld* const world = GetWorld();
	if (world != NULL)
	{
		ADestructibleActor* Destructible = world->SpawnActor<ADestructibleActor>(DM_DamagingRock, SM_DamagingRock->GetComponentLocation(), SM_DamagingRock->GetComponentRotation());

		// Fracture the destructible mesh
		// Parameters 2, 3 and 4 have dummy values in them
		if (Destructible)
		{
			Destructible->GetDestructibleComponent()->ApplyDamage(100.f, FVector(0.f, 0.f, 0.f), FVector(0.f, 0.f, 0.f), 1.f);
			TicksSinceDMSpawn = 10;
		}

		// Now that the destructible mesh has spawned we consider this actor destroyed
		// meaning the chunks from the destructible mesh won't damage the player
		bHasBeenDestroyed = true;

	}

}

void ADamagingRock::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{

	if (bHasBeenDestroyed)
	{
		return;
	}

	bIsCollidingWithPlayer = true;

	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		if (SM_DamagingRock->GetPhysicsLinearVelocity().SizeSquared() > DamagingSpeed)
		{
			// Cause damage to player
			AFPS_ProjectCharacter* Character = Cast<AFPS_ProjectCharacter>(OtherActor);
			if (Character)
			{
				float RandomDamage = FMath::RandRange(0.8f, 1.2f) * Damage;
				Character->ReceiveDamage(RandomDamage);
			}

			// Make the mesh fracture
			Explode(NormalImpulse);

		}

		
	}
	else if (OtherActor->IsA(AProjectile::StaticClass()))
	{
		Explode(NormalImpulse);
	}
}

