// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPC.h"
#include "NetGameMode.h"
#include "NetPawn.h"
#include "GameFramework/PlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"


void ANetPC::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	theGameMode = Cast<ANetGameMode>(UGameplayStatics::GetGameMode(this->GetWorld()));

	if (PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetPC::BeginPlay() | PC Name: %s | PS Name: %s | PID: %i"), *GetName(), *PlayerState->GetPlayerName(), PlayerState->GetPlayerId());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetPC::BeginPlay() | PC Name: %s | NO PlayerState"), *GetName());
	}

	SetInputMappingContext(IMC_Spectating, "SPECTATING");

}


void ANetPC::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::OnRep_PlayerState() | PC Name: %s | PS Name: %s | PID: %i"), *GetName(), *PlayerState->GetPlayerName(), PlayerState->GetPlayerId());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::OnRep_PlayerState() | PC Name: %s | NO PlayerState"), *GetName());
	}

}


void ANetPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TECH DEBT note: Below routine should only run when the player is controlling the ship

	if (!HasAuthority())
	{
		timeLeftToSendInput -= DeltaTime;

		if (timeLeftToSendInput <= 0)
		{
			Server_UpdateUserInput(moveInputVector, aimInputVector);

			timeLeftToSendInput += timeBetweenInputUpdates;
		}
	}
}


void ANetPC::MessageToClient_Implementation(const FString& message)
{
	UE_LOG(LogTemp, Error, TEXT("ANetPC::MessageToClient() | %s"), *message);
}

void ANetPC::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* SFInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!SFInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::SetupInputComponent() Cannot GET EnhancedInputComponent | %s"), *GetName());
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("ANetPC::SetupInputComponent() EnhancedInputComponent SECURED | %s"), *GetName());

	SFInputComponent->BindAction(IA_SpawnShip, ETriggerEvent::Triggered, this, &ANetPC::SpawnShip);
	SFInputComponent->BindAction(IA_ReturnToMenu, ETriggerEvent::Triggered, this, &ANetPC::ReturnToMenu);
	SFInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ANetPC::Move);
	SFInputComponent->BindAction(IA_Aim, ETriggerEvent::Triggered, this, &ANetPC::Aim);
	SFInputComponent->BindAction(IA_Shoot, ETriggerEvent::Triggered, this, &ANetPC::Shoot);
}

void ANetPC::SetInputMappingContext(class UInputMappingContext* newIMC, FString newIMCmessage)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::SetInputMappingContext() This is Authority! | %s"), *GetName());
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* SFInputSystem = Cast<ULocalPlayer>(Player)->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!newIMC)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::SetInputMappingContext() newIMC is NULL | %s"), *GetName());
		return;
	}

	int32 inputMappingPriority = 1;
	SFInputSystem->AddMappingContext(newIMC, inputMappingPriority);
	UE_LOG(LogTemp, Error, TEXT("ANetPC::SetInputMappingContext() IMC: %s | %s"), *newIMCmessage, *GetName());

}

void ANetPC::SpawnShip()
{
	UE_LOG(LogTemp, Error, TEXT("ANetPC::SpawnShip() | %s"), *GetName());
	Server_SpawnAndPossess();
}

void ANetPC::Server_SpawnAndPossess_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ANetPC::Server_SpawnAndPossess() SPAWNING | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());

	if (GetPawn())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::Server_SpawnAndPossess() Pawn already exists | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
		return;
	}

	FVector spawnLocation = FVector(FMath::RandRange(-300, 500), FMath::RandRange(-1000,1000), 0);
	FTransform spawnTransform = FTransform(FRotator(), spawnLocation);
	APawn* newShip = theGameMode->SpawnDefaultPawnAtTransform(this, spawnTransform);

	if (!newShip)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::Server_SpawnAndPossess() NO NEW SHIP! | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ANetPC::Server_SpawnAndPossess() NEW SHIP SPAWNED! | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());

	Possess(newShip);

	myShip = Cast<ANetPawn>(newShip);

	if (!myShip)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::Server_SpawnAndPossess() CASTING to NetPawn FAILED | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
		return;
	}

	myShip->InitializeShip();

	if (GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetPC::Server_SpawnAndPossess() I HAVE A SHIP! OOOOOOOOOOOOO | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::Server_SpawnAndPossess() NO SHIP! XXXXXXXXXXXXX | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
	}
}

void ANetPC::Server_UpdateUserInput_Implementation(FVector2D moveInput, FVector2D aimInput)
{
	UE_LOG(LogTemp, Warning, TEXT("ANetPC::Server_UpdateUserInput() Move: %.2f / %.2f | Aim: %.2f / %.2f | %s (PID: %d)"), moveInput.X, moveInput.Y, aimInput.X, aimInput.Y, *GetName(), PlayerState->GetPlayerId());
}

void ANetPC::ReturnToMenu()
{
	UE_LOG(LogTemp, Error, TEXT("ANetPC::ReturnToMenu() | %s"), *GetName());
	ClientTravel("MainMenu", ETravelType::TRAVEL_Absolute);
}

void ANetPC::Move(const struct FInputActionInstance& Instance)
{
	struct FInputActionValue inputValue = Instance.GetValue();
	moveInputVector = inputValue.Get<FVector2D>();

	UE_LOG(LogTemp, Display, TEXT("ANetPC::Move() X: %.2f / Y: %.2f (PID: %d)"), moveInputVector.X, moveInputVector.Y, PlayerState->GetPlayerId());
}


void ANetPC::Aim(const struct FInputActionInstance& Instance)
{
	struct FInputActionValue inputValue = Instance.GetValue();
	aimInputVector = inputValue.Get<FVector2D>();

	UE_LOG(LogTemp, Warning, TEXT("ANetPC::Aim() X: %.2f / Y: %.2f (PID: %d)"), aimInputVector.X, aimInputVector.Y, PlayerState->GetPlayerId());
}

void ANetPC::Shoot()
{
	UE_LOG(LogTemp, Error, TEXT("ANetPC::Shoot() xxxxxxxxxxxxxxxxxxxxxxx | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
}


void ANetPC::AssignShipToPlayer()
{
	if (!GetPawn())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::AssignShipToPlayer() Cannot GET Pawn! | Player Name: %s (%s)"), *PlayerState->GetPlayerName(), *GetName());
		return;
	}

	myShip = Cast<ANetPawn>(GetPawn());
	UE_LOG(LogTemp, Warning, TEXT("ANetPC::AssignShipToPlayer() GOT my ship | Player Name: %s (%s)"), *PlayerState->GetPlayerName(), *GetName());

	SetInputMappingContext(IMC_Playing, "PLAYING");
}