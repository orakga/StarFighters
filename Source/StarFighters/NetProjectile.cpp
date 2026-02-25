// Fill out your copyright notice in the Description page of Project Settings.


#include "NetProjectile.h"


// Sets default values
ANetProjectile::ANetProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

// Called when the game starts or when spawned
void ANetProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	rootComp = (UPrimitiveComponent*)this->GetRootComponent();

	SetActorTickInterval(5);

	if (!rootComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetProjectile::BeginPlay() rootComp is NOT VALID | %s"), *GetName());
		return;
	}

	FVector currentDirection = this->GetActorForwardVector();
	rootComp->SetAllPhysicsLinearVelocity(currentDirection * projectileSpeed);

	if (HasAuthority())
	{
		TArray<UActorComponent*> myComponents;
		this->GetComponents<UActorComponent>(myComponents);

		FName colliderTag = FName(*FString("Collider"));
		UActorComponent* componentToCheck = nullptr;
		colliderComp = nullptr;

		for (int i = 0; i < myComponents.Num(); i++)
		{
			componentToCheck = myComponents[i];

			if (componentToCheck->ComponentHasTag(colliderTag))
			{
				UE_LOG(LogTemp, Warning, TEXT("ANetProjectile::BeginPlay() Found a Collider | %s"), *GetName());
				colliderComp = (UPrimitiveComponent*)componentToCheck;
			}

		}

		// After all tags have been checked
		if (colliderComp) // A VALID collider has been found
		{
			// BINDING the DELEGATE to the Collider
			colliderComp->OnComponentBeginOverlap.AddDynamic(this, &ANetProjectile::OverlapDetected);
			// UE_LOG(LogTemp, Warning, TEXT("ANetProjectile::BeginPlay() Binding to ANetProjectile::OverlapDetected() | %s"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ANetProjectile::BeginPlay() NO COLLIDER FOUND! | %s"), *GetName());
		}

	}

}

// Called every frame
void ANetProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ANetProjectile::OverlapDetected(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("ANetProjectile::OverlapDetected() %s | Actor: %s | Comp: %s "), *GetName(), *OtherActor->GetName(), *OtherComp->GetName());
}

