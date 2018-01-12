// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Kismet/KismetMathLibrary.h"	// added by me
#include "RandomActor.h"


// Sets default values
ARandomActor::ARandomActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
	RootComponent = MyMesh;

}

// Called when the game starts or when spawned
void ARandomActor::BeginPlay()
{
	Super::BeginPlay();
	
	MyMesh->SetRelativeRotation(UKismetMathLibrary::RandomRotator());


}

// Called every frame
void ARandomActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


