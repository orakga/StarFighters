// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StarGenerator.generated.h"


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
