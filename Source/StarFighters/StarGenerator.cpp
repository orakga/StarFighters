// Fill out your copyright notice in the Description page of Project Settings.


#include "StarGenerator.h"


// Called when the game starts or when spawned
void AStarGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < StarGenerationQuantity; i++)
	{
		GetWorld()->SpawnActor<AActor>(
			StarModel,
			FVector( FMath::RandRange(StarGenerationAreaLeft, StarGenerationAreaRight),
					FMath::RandRange(StarGenerationAreaFront, StarGenerationAreaBack),
					FMath::RandRange(StarGenerationAreaBottom, StarGenerationAreaTop)),
			FRotator(),
			FActorSpawnParameters()
		);
	}
}
