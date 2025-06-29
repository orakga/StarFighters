// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPC.generated.h"

/**
 * 
 */
UCLASS()
class STARFIGHTERS_API AMenuPC : public APlayerController
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	
	
};
