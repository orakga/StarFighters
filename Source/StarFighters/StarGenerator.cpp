// Fill out your copyright notice in the Description page of Project Settings.


#include "StarGenerator.h"
#include "SFLibrary.h"

// Called when the game starts or when spawned
void AStarGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < StarfieldAreaQuantity; i++)
	{
		// ## SPAWN ONE STAR AT A TIME ##
		GetWorld()->SpawnActor<AActor>(
			StarModel,
			FVector( 
				FMath::RandRange(StarfieldAreaLeft, StarfieldAreaRight),
				FMath::RandRange(StarfieldAreaFront, StarfieldAreaBack),
				FMath::RandRange(StarfieldAreaBottom, StarfieldAreaTop)
				),
			FRotator(),
			FActorSpawnParameters()
		);
	}
}
