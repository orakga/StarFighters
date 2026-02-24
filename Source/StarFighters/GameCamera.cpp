// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCamera.h"
#include "SFLibrary.h"


AGameCamera::AGameCamera()
{
	PrimaryActorTick.bCanEverTick = true; //Enables Tick() to run
}


void AGameCamera::BeginPlay()
{
	Super::BeginPlay();

	myRootComp = (UPrimitiveComponent*)this->GetRootComponent();
	myTarget = nullptr;
}


void AGameCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (myTarget)  // ### TECH DEBT? ### Better to use TWeakObjPtr / IsValid()??
	{
		if (myRootComp && targetRootComp)
		{
			myRootComp->SetWorldLocation(targetRootComp->GetComponentLocation() + FixedCameraLocation * 0.5f);
		}
	}

}


void AGameCamera::SetTarget(class AActor* newTarget)
{
	if (!newTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameCamera::SetTarget() FAILED: newTarget is NOT VALID!"));
		return;
	}

	targetRootComp = (UPrimitiveComponent*)newTarget->GetRootComponent();

	if (!targetRootComp)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameCamera::SetTarget() FAILED: Could not get RootComponent of newTarget!"));
		return;
	}

	myTarget = newTarget;
	UE_LOG(LogTemp, Warning, TEXT("AGameCamera::SetTarget() NEW TARGET: %s"), *myTarget->GetName());
}