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

	UFUNCTION(reliable, client)
		void PostLoginInitialization(int32 newPlayerID, const FString& newPlayerName, const FString& newServerName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void InitializeInGameOverlay(int32 newPlayerID, const FString& newPlayerName, const FString& newServerName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void AddSystemMessage(const FString& newSysMsg);
	
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
	class UInputAction* IA_Shoot1;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Shoot2;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_MouseAim;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGameCamera> camera_Template;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ANetWeapon> weapon_template_1;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ANetWeapon> weapon_template_2;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ANetWeapon> weapon_template_3;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ANetWeapon> weapon_template_4;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ANetWeapon> weapon_template_5;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ANetWeapon> weapon_template_6;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ANetWeapon> weapon_template_BLANK;

	void AssignShipToPlayer();

	UFUNCTION(BlueprintCallable)
	void SetWeapon1( EWeaponType type );

	UFUNCTION(BlueprintCallable)
	void SetWeapon2( EWeaponType type );

	TSubclassOf<class ANetWeapon> ResolveWeaponClass(EWeaponType type);

	UFUNCTION(reliable, client)
		void RegisterWeapon1(class ANetWeapon* newWeapon);

	UFUNCTION(reliable, client)
		void RegisterWeapon2(class ANetWeapon* newWeapon);

	void DestroyShip();

private:

	void SpawnShip();

	UFUNCTION(reliable, Server)
		void Server_SpawnAndPossess(EWeaponType weapon1Type, EWeaponType weapon2Type);

	UFUNCTION(reliable, client)
		void StartSpectating();

	UFUNCTION(unreliable, server)
		void Server_UpdatePlayerInput(FPlayerInputState newPlayerInputState);

	void ReturnToMenu();
	void Move(const struct FInputActionInstance& Instance);
	void Aim(const struct FInputActionInstance& Instance);
	void MouseAim(const struct FInputActionInstance& Instance);
	// void Shoot();
	void WeaponTrigger1_On();
	void WeaponTrigger1_Off();
	void WeaponTrigger2_On();
	void WeaponTrigger2_Off();

	void DebugDisplay();

	UFUNCTION(reliable, server)
		void Server_WeaponTrigger1(bool isActive);

	UFUNCTION(reliable, server)
		void Server_WeaponTrigger2(bool isActive);

	UWorld* theWorld;
	class ANetGameMode* theGameMode;
	// class ANetPawn* myShip;
	TWeakObjectPtr<class ANetPawn> myShip;
	TWeakObjectPtr<class ANetWeapon> myWeapon1;
	TWeakObjectPtr<class ANetWeapon> myWeapon2;
	class AGameCamera* myCamera;

	EWeaponType selectedWeapon1 = EWeaponType::DefaultSolo;
	EWeaponType selectedWeapon2 = EWeaponType::None;

	float timeBetweenInputUpdates = (float) 1 / (float) UserInputUpdateFrequency;
	float timeLeftToSendInput = 0;

	FVector2D moveInputVector;
	FVector2D aimInputVector;
	FPlayerInputState playerInputState;
	bool isMousePressed = false;
	FVector mouseHitLocation;

protected:

	UPROPERTY(BlueprintReadOnly)
		int32 playerID;

	UPROPERTY(BlueprintReadOnly)
		FString playerName;

	UPROPERTY(BlueprintReadOnly)
		FString serverName;

};
