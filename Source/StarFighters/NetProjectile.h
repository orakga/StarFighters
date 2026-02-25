// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetProjectile.generated.h"

UCLASS()
class STARFIGHTERS_API ANetProjectile : public AActor
{
	GENERATED_BODY()
	
private:	
	// Sets default values for this actor's properties
	ANetProjectile();

	UPrimitiveComponent* rootComp;
	UPrimitiveComponent* colliderComp;

	float projectileSpeed = 50;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OverlapDetected(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
