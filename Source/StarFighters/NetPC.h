// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SFLibrary.h"
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

	ANetPC();
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION(reliable, client)
		void MessageToClient(const FString& message);
	
	virtual void SetupInputComponent() override;

	void SetInputMappingContext(class UInputMappingContext* newIMC, FString newIMCmessage);

	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_Spectating;

	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_Playing;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_SpawnShip;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_ReturnToMenu;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Move;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Aim;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Shoot;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGameCamera> camera_Template;

	void AssignShipToPlayer();

private:

	void SpawnShip();

	UFUNCTION(reliable, Server)
		void Server_SpawnAndPossess();

	UFUNCTION(unreliable, server)
		void Server_UpdateUserInput(FVector2D moveInput, FVector2D aimInput);

	void ReturnToMenu();
	void Move(const struct FInputActionInstance& Instance);
	void Aim(const struct FInputActionInstance& Instance);
	void Shoot();

	UFUNCTION(reliable, server)
		void Server_Shoot();

	class ANetGameMode* theGameMode;
	// class ANetPawn* myShip;
	TWeakObjectPtr<class ANetPawn> myShip;
	class AGameCamera* myCamera;

	float timeBetweenInputUpdates = (float) 1 / (float) UserInputUpdateFrequency;
	float timeLeftToSendInput = 0;

	FVector2D moveInputVector;
	FVector2D aimInputVector;
};
