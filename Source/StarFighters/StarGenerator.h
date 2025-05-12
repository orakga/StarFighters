// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StarGenerator.generated.h"

#define StarfieldAreaLeft -10000
#define StarfieldAreaRight 10000
#define StarfieldAreaFront -10000
#define StarfieldAreaBack 10000
#define StarfieldAreaBottom -10000
#define StarfieldAreaTop -500
#define StarfieldAreaQuantity 500


UCLASS()
class STARFIGHTERS_API AStarGenerator : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> StarModel;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
};
