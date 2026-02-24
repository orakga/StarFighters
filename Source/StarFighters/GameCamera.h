// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameCamera.generated.h"

/**
 * 
 */
UCLASS()
class STARFIGHTERS_API AGameCamera : public ACameraActor
{
	GENERATED_BODY()
	
private:

	AGameCamera();
	virtual void BeginPlay() override;
	virtual void Tick(float DetaTime) override;
	
	AActor* myTarget;
	
	UPrimitiveComponent* targetRootComp;
	UPrimitiveComponent* myRootComp;

public:

	void SetTarget(class AActor* newTarget);

};
