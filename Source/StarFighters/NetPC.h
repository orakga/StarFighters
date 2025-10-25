// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetPC.generated.h"

/**
 * 
 */
UCLASS()
class STARFIGHTERS_API ANetPC : public APlayerController
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;

public:

	UFUNCTION(reliable, client)
		void MessageToClient(const FString& message);
	
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_Spectating;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_SpawnShip;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_ReturnToMenu;

private:

	void SpawnShip();
	void ReturnToMenu();

};
