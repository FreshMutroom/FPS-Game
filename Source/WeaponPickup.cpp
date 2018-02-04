// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "WeaponPickup.h"


// Sets default values
AWeaponPickup::AWeaponPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	WeaponBox->bGenerateOverlapEvents = true;
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeaponPickup::TriggerEnter);
	WeaponBox->OnComponentEndOverlap.AddDynamic(this, &AWeaponPickup::TriggerExit);

	RootComponent = WeaponBox;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box Mesh"));
	WeaponMesh->AttachTo(RootComponent);

	bWeaponIsWithinRange = false;

}

// Called when the game starts or when spawned
void AWeaponPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyPlayerController != NULL)	// can crash game without this
	{
		if (MyPlayerController->bIsPickingUpWeapon && bWeaponIsWithinRange)
		{
			PickupWeapon();
		}
	}

}

void AWeaponPickup::PickupWeapon()
{
	MyPlayerController->EquippedWeapon = WeaponType;
	Destroy();

}

void AWeaponPickup::GetPlayer(AActor * Player)
{
	MyPlayerController = Cast<AFPS_ProjectCharacter>(Player);

}

void AWeaponPickup::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	bWeaponIsWithinRange = true;
	GetPlayer(OtherActor);

}

void AWeaponPickup::TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	bWeaponIsWithinRange = false;

}

