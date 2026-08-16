// Fill out your copyright notice in the Description page of Project Settings.


#include "NetWeaponBlank.h"


void ANetWeaponBlank::Tick(float DeltaTime)
{
	// DO NOTHING
}



void ANetWeaponBlank::Shoot()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetWeaponBlank::Shoot() must run on AUTHORITY | ID: %d | %s"), shooterID, *GetNameSafe(this));
	}

	UE_LOG(LogTemp, Display, TEXT("ANetWeaponBlank::Shoot() BLANK WEAPON! | ID: %d | %s"), shooterID, *GetNameSafe(this));
	// DO NOTHING
}


