// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StarGenerator.generated.h"

#define StarGenerationAreaLeft -10000
#define StarGenerationAreaRight 10000
#define StarGenerationAreaFront -10000
#define StarGenerationAreaBack 10000
#define StarGenerationAreaTop -500
#define StarGenerationAreaBottom -10000
#define StarGenerationQuantity 500


UCLASS()
class STARFIGHTERS_API AStarGenerator : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> StarModel;
	
	
};
