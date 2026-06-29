// Fill out your copyright notice in the Description page of Project Settings.


#include "StarGenerator.h"
#include "SFLibrary.h"

// Called when the game starts or when spawned
void AStarGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < areaQuantity; i++)
	{
		// ## SPAWN ONE STAR AT A TIME ##
		GetWorld()->SpawnActor<AActor>(
			StarModel,
			FVector( 
				FMath::RandRange(areaLeft, areaRight),
				FMath::RandRange(areaFront, areaBack),
				FMath::RandRange(areaBottom, areaTop)
				),
			FRotator(),
			FActorSpawnParameters()
		);
	}
}
