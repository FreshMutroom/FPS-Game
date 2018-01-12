// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "IdleGuard.h"
#include "Runtime/Engine/Classes/Animation/SkeletalMeshActor.h"


// Sets default values
AIdleGuard::AIdleGuard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	CharacterMesh->SetupAttachment(RootComponent);
	CharacterMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AIdleGuard::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("b_RightWeaponSocket"));
	WeaponMesh->SetHiddenInGame(false);
	WeaponMesh->SetVisibility(true);
	WeaponMesh->RelativeRotation = FRotator(0.f, 0.f, -90.f);

	// To make character fall to ground. Doesn't work like intended
	CharacterMesh->SetEnableGravity(true);

}

// Called every frame
void AIdleGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIdleGuard::HasBeenShot()
{
	if (!bHasBeenShot)
	{
		bHasBeenShot = true;

		if (CharacterMesh)
		{
			// Enable ragdoll
			CharacterMesh->SetAllBodiesSimulatePhysics(true);
			CharacterMesh->SetSimulatePhysics(true);
			CharacterMesh->WakeAllRigidBodies();

			UWorld* World = GetWorld();
			if (World)
			{
				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AIdleGuard::StopRagdoll, 30.f, false);

			}

		}

	}
	
}

void AIdleGuard::StopRagdoll()
{
	CharacterMesh->SetAllBodiesSimulatePhysics(false);
	CharacterMesh->SetSimulatePhysics(false);
	CharacterMesh->PutAllRigidBodiesToSleep();

	CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

