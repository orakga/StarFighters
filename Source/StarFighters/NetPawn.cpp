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

	rootComp = (UPrimitiveComponent*)(this->GetRootComponent());
	rootComp->SetLinearDamping(ShipLinearDamping);
	rootComp->SetAngularDamping(ShipAngularDamping);
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

	if (HasAuthority())
	{
		FVector accelVector = FVector(moveInputVector.Y, moveInputVector.X, 0);

		if (accelVector.Size() > 1)
		{
			accelVector.Normalize(); // limits Vector magnitude to 1.0
		}


		rootComp->AddForce(accelVector * DeltaTime * shipAcceleration, NAME_None, true);

		timeLeftToSendState -= DeltaTime;

		if (timeLeftToSendState <= 0)
		{
			Multicast_BroadcastState(rootComp->GetComponentLocation(), rootComp->GetPhysicsLinearVelocity(), rootComp->GetComponentRotation());
			
			timeLeftToSendState += timeBetweenStateUpdates;
		}

	}

	FVector currentLocation = rootComp->GetComponentLocation();
	// UE_LOG(LogTemp, Display, TEXT("ANetPawn::Tick() Position X: %.2f / Y: %.2f / Z: %.2f | %s | %d"), currentLocation.X, currentLocation.Y, currentLocation.Z, *myShipName, myShipID);
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

void ANetPawn::SetUserInput(FVector2D moveInput, FVector2D aimInput)
{
	moveInputVector = moveInput;
	aimInputVector = aimInput;

	UE_LOG(LogTemp, Display, TEXT("ANetPawn::SetUserInput() Move: %.2f / %.2f | Aim: %.2f / %.2f | %s (PID: %d)"), moveInput.X, moveInput.Y, aimInput.X, aimInput.Y, *GetName(), myShipID);
}

void ANetPawn::Multicast_BroadcastState_Implementation(FVector shipPosition, FVector shipVelocity, FRotator shipRotation)
{
	if (HasAuthority())
	{
		return;
	}

	if (!rootComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPawn::Multicast_BroadcastState() rootComp NOT initalized | ID: % i | % s"), myShipID, *GetDebugName(this));
		return;
	}

	rootComp->SetWorldLocationAndRotation(shipPosition, shipRotation);
	rootComp->SetPhysicsLinearVelocity(shipVelocity);
}