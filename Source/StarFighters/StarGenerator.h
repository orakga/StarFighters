// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFLibrary.h"
#include "GameFramework/Actor.h"
#include "StarGenerator.generated.h"


UCLASS()
class STARFIGHTERS_API AStarGenerator : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> StarModel;

	UPROPERTY(EditAnywhere)
		float areaLeft= StarfieldAreaLeft;

	UPROPERTY(EditAnywhere)
		float areaRight = StarfieldAreaRight;

	UPROPERTY(EditAnywhere)
		float areaFront = StarfieldAreaFront;

	UPROPERTY(EditAnywhere)
		float areaBack = StarfieldAreaBack;

	UPROPERTY(EditAnywhere)
		float areaBottom = StarfieldAreaBottom;

	UPROPERTY(EditAnywhere)
		float areaTop = StarfieldAreaTop;

	UPROPERTY(EditAnywhere)
		int32 areaQuantity = StarfieldAreaQuantity;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
};
