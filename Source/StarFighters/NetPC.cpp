// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPC.h"
#include "NetGameMode.h"
#include "NetPawn.h"
#include "GameCamera.h"
#include "GameFramework/PlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "DrawDebugHelpers.h"


ANetPC::ANetPC()
{
	bAutoManageActiveCameraTarget = false;
}


void ANetPC::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	theWorld = GetWorld();
	theGameMode = Cast<ANetGameMode>(UGameplayStatics::GetGameMode(this->GetWorld()));

	if (PlayerState)
	{
		UE_LOG(LogTemp, Display, TEXT("ANetPC::BeginPlay() | PC Name: %s | PS Name: %s | PID: %i"), *GetName(), *PlayerState->GetPlayerName(), PlayerState->GetPlayerId());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetPC::BeginPlay() | PC Name: %s | NO PlayerState"), *GetName());
	}

	SetInputMappingContext(IMC_Spectating, "SPECTATING");

	if (!HasAuthority())
	{
		myCamera = this->GetWorld()->SpawnActor<AGameCamera>(camera_Template, FixedCameraLocation, FixedCameraRotation, FActorSpawnParameters());
		SetViewTarget(myCamera);
	}

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

	if (!HasAuthority() && myShip.IsValid() )
	{
		DebugDisplay();

		timeLeftToSendInput -= DeltaTime;

		if (timeLeftToSendInput <= 0)
		{
			Server_UpdatePlayerInput(playerInputState);

			timeLeftToSendInput += timeBetweenInputUpdates;
		}
	}
}


void ANetPC::MessageToClient_Implementation(const FString& message)
{
	UE_LOG(LogTemp, Error, TEXT("ANetPC::MessageToClient() | %s"), *message);
}

void ANetPC::PostLoginInitialization_Implementation(int32 newPlayerID, const FString& newPlayerName, const FString& newServerName)
{
	UE_LOG(LogTemp, Warning, TEXT("ANetPC::PostLoginInitialization() | ID: %i | Player Name: %s | Server Name: %s"), newPlayerID, *newPlayerName, *newServerName);

	playerID = newPlayerID;
	playerName = newPlayerName;
	serverName = newServerName;

	// InitializeInGameOverlay(newPlayerID, newPlayerName, newServerName);
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

	UE_LOG(LogTemp, Warning, TEXT("ANetPC::SetupInputComponent() EnhancedInputComponent SECURED | %s"), *GetName());

	SFInputComponent->BindAction(IA_SpawnShip, ETriggerEvent::Triggered, this, &ANetPC::SpawnShip);
	SFInputComponent->BindAction(IA_ReturnToMenu, ETriggerEvent::Triggered, this, &ANetPC::ReturnToMenu);
	SFInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ANetPC::Move);
	SFInputComponent->BindAction(IA_Aim, ETriggerEvent::Triggered, this, &ANetPC::Aim);
	SFInputComponent->BindAction(IA_MouseAim, ETriggerEvent::Triggered, this, &ANetPC::MouseAim);
	SFInputComponent->BindAction(IA_Shoot, ETriggerEvent::Triggered, this, &ANetPC::Shoot);
}

void ANetPC::SetInputMappingContext(class UInputMappingContext* newIMC, FString newIMCmessage)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Display, TEXT("ANetPC::SetInputMappingContext() This is Authority! | %s"), *GetName());
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* SFInputSystem = Cast<ULocalPlayer>(Player)->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!newIMC)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::SetInputMappingContext() newIMC is NULL | %s"), *GetName());
		return;
	}

	int32 inputMappingPriority = 1;
	SFInputSystem->ClearAllMappings();
	SFInputSystem->AddMappingContext(newIMC, inputMappingPriority);
	UE_LOG(LogTemp, Warning, TEXT("ANetPC::SetInputMappingContext() IMC: %s | %s"), *newIMCmessage, *GetName());

}

void ANetPC::SpawnShip()
{
	UE_LOG(LogTemp, Error, TEXT("ANetPC::SpawnShip() | %s"), *GetName());
	Server_SpawnAndPossess();
}

void ANetPC::Server_SpawnAndPossess_Implementation()
{
	UE_LOG(LogTemp, Display, TEXT("ANetPC::Server_SpawnAndPossess() SPAWNING | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());

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

	if (!myShip.IsValid())
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

void ANetPC::Server_UpdatePlayerInput_Implementation(FPlayerInputState newPlayerInputState)
{
	// UE_LOG(LogTemp, Display, TEXT("ANetPC::Server_UpdateUserInput() Move: %.2f / %.2f | Aim: %.2f / %.2f | %s (PID: %d)"), moveInput.X, moveInput.Y, aimInput.X, aimInput.Y, *GetName(), PlayerState->GetPlayerId());

	if (!myShip.IsValid())
	{
		// UE_LOG(LogTemp, Error, TEXT("ANetPC::Server_UpdateUserInput() myShip is NOT VALID | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
		return;
	}

	myShip->SetPlayerInput(newPlayerInputState);
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

	if (moveInputVector.Size() < MoveInputDeadzone)
	{
		playerInputState.isMoveInputActive = false;
		// moveInputVector = FVector2D(0, 0);
	}
	else
	{
		playerInputState.isMoveInputActive = true;

		// CALCULATE HEADING
		float headingRadians = FMath::Atan2(moveInputVector.X, moveInputVector.Y);
		playerInputState.moveHeading = SFLibrary::BoundHeadingAngle( FMath::RadiansToDegrees(headingRadians) );
	}

	// UE_LOG(LogTemp, Error, TEXT("ANetPC::Move() X: %.2f / Y: %.2f | Heading: %.1f (PID: %d)"), moveInputVector.X, moveInputVector.Y, playerInputState.moveHeading, PlayerState->GetPlayerId());
}


void ANetPC::Aim(const struct FInputActionInstance& Instance)
{
	struct FInputActionValue inputValue = Instance.GetValue();
	aimInputVector = inputValue.Get<FVector2D>();

	if (aimInputVector.Size() < AimInputDeadzone)
	{
		playerInputState.isAimInputActive = false;
		// aimInputVector = FVector2D(0, 0);
	}
	else
	{
		playerInputState.isAimInputActive = true;

		// CALCULATE HEADING
		float headingRadians = FMath::Atan2(aimInputVector.X, aimInputVector.Y);
		playerInputState.aimHeading = SFLibrary::BoundHeadingAngle(FMath::RadiansToDegrees(headingRadians));
	}

	// UE_LOG(LogTemp, Warning, TEXT("ANetPC::Aim() X: %.2f / Y: %.2f | Heading: %.1f (PID: %d)"), aimInputVector.X, aimInputVector.Y, playerInputState.aimHeading, PlayerState->GetPlayerId());
}

void ANetPC::MouseAim(const struct FInputActionInstance& Instance)
{
	struct FInputActionValue inputValue = Instance.GetValue();
	isMousePressed = inputValue.Get<bool>();

	FHitResult hitResult;
	GetHitResultUnderCursor(MouseAimTraceChannel, false, hitResult);

	if (myShip.IsValid() && isMousePressed && hitResult.bBlockingHit)
	{
		playerInputState.isAimInputActive = true;
	}
	else
	{
		playerInputState.isAimInputActive = false;
		return;
	}

	mouseHitLocation = hitResult.Location;
	FVector shipLocation = myShip->GetActorLocation();
	FVector mouseAimDirection = mouseHitLocation - shipLocation;

	// CALCULATE HEADING
	float headingRadians = FMath::Atan2(mouseAimDirection.Y, mouseAimDirection.X);
	playerInputState.aimHeading = SFLibrary::BoundHeadingAngle(FMath::RadiansToDegrees(headingRadians));
}
	

void ANetPC::Shoot()
{
	// UE_LOG(LogTemp, Error, TEXT("ANetPC::Shoot() xxxxxxxxxxxxxxxxxxxxxxx | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());

	Server_Shoot();
}


void ANetPC::Server_Shoot_Implementation()
{
	if (!myShip.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::Server_Shoot() myShip is NOT VALID | Player Name: %s (PID: %d)"), *PlayerState->GetPlayerName(), PlayerState->GetPlayerId());
		return;
	}

	myShip->Shoot();
}



void ANetPC::AssignShipToPlayer()
{
	if (!GetPawn())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::AssignShipToPlayer() Cannot GET Pawn! | Player Name: %s (%s)"), *PlayerState->GetPlayerName(), *GetName());
		return;
	}

	myShip = Cast<ANetPawn>(GetPawn());

	if (!myShip.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::AssignShipToPlayer() FAILED TO GET SHIP | Player Name: %s (%s)"), *PlayerState->GetPlayerName(), *GetName());
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("ANetPC::AssignShipToPlayer() GOT my ship | Player Name: %s (%s)"), *PlayerState->GetPlayerName(), *GetName());

	myCamera->SetTarget(myShip.Get());
	SetInputMappingContext(IMC_Playing, "PLAYING");
}

void ANetPC::DestroyShip()
{
	UE_LOG(LogTemp, Warning, TEXT("ANetPC::DestroyShip() Player Name: %s (%s)"), *PlayerState->GetPlayerName(), *GetName());

	StartSpectating();
	UnPossess();
	myShip->HandleDeath();
	myShip.Reset();
}

void ANetPC::StartSpectating_Implementation()
{
	myShip.Reset();
	SetInputMappingContext(IMC_Spectating, TEXT("SPECTATING"));
	myCamera->SpectatorMode();
	UE_LOG(LogTemp, Warning, TEXT("ANetPC::StartSpectating() Returning to SPECTATOR MODE | %s (%s)"), *PlayerState->GetPlayerName(), *GetName());
}

void ANetPC::DebugDisplay()
{
	FVector shipLocation = myShip->GetActorLocation();

	// Draw MOVE debugs ==============================================
	if (playerInputState.isMoveInputActive)
	{
		float moveHeadingRad = FMath::DegreesToRadians(playerInputState.moveHeading);
		FVector moveDir = FVector(FMath::Cos(moveHeadingRad), FMath::Sin(moveHeadingRad), 0);

		// DRAW LINE ------------------------------
		DrawDebugLine(
			theWorld, // const UWorld * InWorld,
			shipLocation, // const FVector & LineStart,
			shipLocation + moveDir * 300, // const FVector & LineEnd,
			FColor::Green, // const FColor & Color,
			false, // bool bPersistentLines = false,
			0.f, // float LifeTime = -1.f,
			0, // uint8 DepthPriority = 0,
			3 // float Thickness = 0.f
		);


		// DRAW TEXT ------------------------------
		/*
		DrawDebugString(
			theWorld, // const UWorld * InWorld,
			shipLocation + FVector(0, -100, 0), // const FVector & TextLocation,
			FString::Printf(TEXT("%.0f"), playerInputState.moveHeading), // const FString & Text,
			nullptr, // AActor * TestBaseActor = nullptr,
			FColor::Green, // const FColor & TextColor = FColor::White,
			0, // float Duration = -1.0f,
			true, // bool bDrawShadow = false,
			1 // float FontScale = 1.f
		);
		*/

	}


	// Draw AIM debugs ==============================================
	if (playerInputState.isAimInputActive)
	{
		float aimHeadingRad = FMath::DegreesToRadians(playerInputState.aimHeading);
		FVector aimDir = FVector(FMath::Cos(aimHeadingRad), FMath::Sin(aimHeadingRad), 0);

		// DRAW LINE ------------------------------
		DrawDebugLine(
			theWorld, // const UWorld * InWorld,
			shipLocation, // const FVector & LineStart,
			shipLocation + aimDir * 300, // const FVector & LineEnd,
			FColor::Red, // const FColor & Color,
			false, // bool bPersistentLines = false,
			0.f, // float LifeTime = -1.f,
			0, // uint8 DepthPriority = 0,
			3 // float Thickness = 0.f
		);


		// DRAW TEXT ------------------------------
		/*
		DrawDebugString(
			theWorld, // const UWorld * InWorld,
			shipLocation + FVector(0, +100, 0), // const FVector & TextLocation,
			FString::Printf(TEXT("%.0f"), playerInputState.aimHeading), // const FString & Text,
			nullptr, // AActor * TestBaseActor = nullptr,
			FColor::Red, // const FColor & TextColor = FColor::White,
			0, // float Duration = -1.0f,
			true, // bool bDrawShadow = false,
			1 // float FontScale = 1.f
		);
		*/

		if (isMousePressed)
		{
			FVector crossLeft = mouseHitLocation + FVector(-30, 0, 0);
			FVector crossRight = mouseHitLocation + FVector(30, 0, 0);
			FVector crossTop = mouseHitLocation + FVector(0, 30, 0);
			FVector crossBottom = mouseHitLocation + FVector(0, -30, 0);

			DrawDebugLine(theWorld, crossLeft, crossRight, FColor::Yellow, false, 0.f, 0, 5);
			DrawDebugLine(theWorld, crossTop, crossBottom, FColor::Yellow, false, 0.f, 0, 5);
		}

	}
}