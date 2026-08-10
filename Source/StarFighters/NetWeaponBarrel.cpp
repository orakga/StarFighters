// Fill out your copyright notice in the Description page of Project Settings.


#include "NetWeaponBarrel.h"


// Sets default values
ANetWeaponBarrel::ANetWeaponBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

}


USceneComponent* ANetWeaponBarrel::GetMuzzleComponent()
{
	if (muzzleFound && muzzleComponent)
	{
		return muzzleComponent;
	}
	
	// Find Component with Corresponding TAG ================================================
	TArray<UActorComponent*> myComponents;
	this->GetComponents<UActorComponent>(myComponents);

	for (UActorComponent* Comp : myComponents)
	{
		// Find Barrel: SOLO =================
		if (Comp->ComponentHasTag(TEXT("Muzzle")))
		{
			UE_LOG(LogTemp, Warning, TEXT("ANetWeaponBarrel::GetMuzzleComponent() FOUND: Muzzle | %s"), *GetDebugName(this));
			muzzleComponent = (USceneComponent*)Comp;
			muzzleFound = true;
			return muzzleComponent;
		}
	}

	if (!muzzleFound)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetWeaponBarrel::GetMuzzleComponent() MUZZLE NOT FOUND | %s"), *GetDebugName(this));
	}

	return nullptr;
}