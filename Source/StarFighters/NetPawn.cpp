// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPawn.h"


// Sets default values
ANetPawn::ANetPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ANetPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority())
	{
		SetLabel(999, GetDebugName(this));
	}

}

// Called every frame
void ANetPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANetPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

