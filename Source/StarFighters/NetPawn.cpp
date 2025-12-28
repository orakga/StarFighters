// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPawn.h"
#include "NetPC.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"

// Sets default values
ANetPawn::ANetPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ANetPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority())
	{
		SetLabel(myShipID, myShipName);
	}

}

void ANetPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ANetPawn, myShipID, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ANetPawn, myShipName, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ANetPawn, isShipInitialized, COND_InitialOnly);
}

void ANetPawn::OnRep_ShipID()
{
	UE_LOG(LogTemp, Warning, TEXT("ANetPawn::OnRep_ShipID() ShipID: %d"), myShipID);
}

void ANetPawn::OnRep_ShipName()
{
	UE_LOG(LogTemp, Warning, TEXT("ANetPawn::OnRep_ShipName() ShipName: %s"), *myShipName);
}

void ANetPawn::OnRep_ShipInitialized()
{
	UE_LOG(LogTemp, Warning, TEXT("ANetPawn::OnRep_ShipInitialized() myShipID = %d | myShipName = %s | %s"), myShipID, *myShipName, *GetDebugName(this));

	// Need to check for Controller to prevent OTHER clients from crashing
	if (!Controller)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPawn::OnRep_ShipInitialized() NOT CONTROLLED by this Client | %s"), *GetDebugName(this));
		return;
	}

	Cast<ANetPC>(Controller)->AssignShipToPlayer();
}


// Called every frame
void ANetPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANetPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANetPawn::InitializeShip()
{
	UE_LOG(LogTemp, Error, TEXT("ANetPawn::InitializeShip() | %s"), *GetDebugName(this));

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPawn::InitializeShip() MUST RUN ON SERVER | %s"), *GetDebugName(this));
		return;
	}

	if (!Controller)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPawn::InitializeShip() NO CONTROLLER | %s"), *GetDebugName(this));
		return;
	}

	myShipID = Controller->PlayerState->GetPlayerId();
	myShipName = Controller->PlayerState->GetPlayerName();
	isShipInitialized = true;

	UE_LOG(LogTemp, Warning, TEXT("ANetPawn::InitializeShip() myShipID = %d | myShipName = %s | %s"), myShipID, *myShipName, *GetDebugName(this));

}