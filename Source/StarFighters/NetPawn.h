// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NetPawn.generated.h"

UCLASS()
class STARFIGHTERS_API ANetPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANetPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent)
		void SetLabel(int32 shipID, const FString& shipName);

	void InitializeShip();

private:

	UPROPERTY(ReplicatedUsing = OnRep_ShipID)
		int32 myShipID = -999;

	UFUNCTION()
		void OnRep_ShipID();
	
	UPROPERTY(ReplicatedUsing = OnRep_ShipName)
		FString myShipName = "Ship Name??";

	UFUNCTION()
		void OnRep_ShipName();

	UPROPERTY(ReplicatedUsing = OnRep_ShipInitialized)
		bool isShipInitialized = false;
	
	UFUNCTION()
		void OnRep_ShipInitialized();

};
