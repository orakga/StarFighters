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

	void CheckForOutOfBounds();

	UPrimitiveComponent* rootComp;
	UPrimitiveComponent* colliderComp;

	float projectileSpeed = 300;

	int32 myShooterID = -1;
	bool projectileInitialized = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OverlapDetected(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(reliable, NetMulticast)
		void BroadcastDamage();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	int32 GetMyShooterID() { return myShooterID; }
	void SetProjectileParams(int32 shooterID);
	bool IsInitalized() { return projectileInitialized;}
	
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AActor> hitFX_template;
};
