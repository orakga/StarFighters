// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFGameplayAttributes.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARFIGHTERS_API USFGameplayAttributes : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USFGameplayAttributes();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeAttributes(int32 newPlayerID, int32 initialHealth, int32 initialMaxHealth);
	void ProcessDamage(int32 damage, int32 shooterID, AActor* damageSourceActor);

private:
	
	TWeakObjectPtr<class ANetPawn> myPawnPtr;

	int32 health = 0;
	int32 maxHealth = 0;
	int32 playerID = 0;
	bool isAlive = true;
	
};
