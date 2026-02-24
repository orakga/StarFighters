// Fill out your copyright notice in the Description page of Project Settings.


#include "NetProjectile.h"


// Sets default values
ANetProjectile::ANetProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

// Called when the game starts or when spawned
void ANetProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	rootComp = (UPrimitiveComponent*)this->GetRootComponent();

	SetActorTickInterval(5);

	if (!rootComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetProjectile::BeginPlay() rootComp is NOT VALID | %s"), *GetName());
		return;
	}

	FVector currentDirection = this->GetActorForwardVector();
	rootComp->SetAllPhysicsLinearVelocity(currentDirection * projectileSpeed);

}

// Called every frame
void ANetProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

