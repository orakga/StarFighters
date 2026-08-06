// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFLibrary.h"
#include "NetWeapon.generated.h"

UCLASS()
class STARFIGHTERS_API ANetWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetWeaponParameters(int32 incomingID);

	void Shoot();

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ANetProjectile> projectileTemplate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPrimitiveComponent* rootComp;
	UWorld* theWorld;

	int32 shooterID = -1;

};
