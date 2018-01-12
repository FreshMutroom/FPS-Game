// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "FPS_ProjectCharacter.h"
#include "RotatingText.h"


// Sets default values
ARotatingText::ARotatingText()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("RotatingText"));
	MyText->SetWorldSize(70.0f);
	MyText->SetTextRenderColor(FColor::Red);
	
	// Set horizontal alignment
	MyText->SetHorizontalAlignment(EHTA_Center);



}

// Called when the game starts or when spawned
void ARotatingText::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARotatingText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsStatic)
	{
		MakeTextFacePlayer();
	}

}

void ARotatingText::MakeTextFacePlayer()
{
	AFPS_ProjectCharacter* Character = Cast<AFPS_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	FRotator NewRotation = Character->GetActorRotation();
	
	NewRotation.Yaw += 180.f;

	MyText->SetRelativeRotation(NewRotation);
}

