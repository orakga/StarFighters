// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetWeapon.h"
#include "NetWeaponBlank.generated.h"

/**
 * 
 */
UCLASS()
class STARFIGHTERS_API ANetWeaponBlank : public ANetWeapon
{
	GENERATED_BODY()
	
	public:

		virtual void Tick( float DeltaTime ) override;

	protected:
		
		virtual void Shoot() override;	
	
};
