// Fill out your copyright notice in the Description page of Project Settings.


#include "ARInteractionComponent.h"
#include "ARGameplayInterface.h"

// Sets default values for this component's properties
UARInteractionComponent::UARInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UARInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	MyOwner = Cast<APawn>(GetOwner());
	// ...
	
}


// Called every frame
void UARInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AActor* UARInteractionComponent::GetInteractableObject()
{
	if (MyOwner)
	{
		//FHitResult hit;
		FVector start;
		FVector end;
		FCollisionObjectQueryParams queryParams;
		queryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		FRotator lookingDirection;
		FVector eyeLocation;
		MyOwner->GetActorEyesViewPoint(eyeLocation, lookingDirection);
		end = eyeLocation + (lookingDirection.Vector() * 1000.f);
		//GetWorld()->LineTraceSingleByObjectType(hit, eyeLocation, end, queryParams);
		TArray<FHitResult> hits;
		FCollisionShape Shape;
		Shape.SetSphere(30.0f);
		GetWorld()->SweepMultiByObjectType(hits, eyeLocation, end, FQuat::Identity, queryParams, Shape);
		for (FHitResult hit : hits)
		{
			AActor* HitActor = hit.GetActor();
			if (HitActor)
			{
				if (HitActor->Implements<UARGameplayInterface>())
				{
					return HitActor;
				}
			}
			else
			{
				//UE_LOG(LogTemp, Log, TEXT("Nothing to interact with"));
				return nullptr;
			}
		}
		return nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Tried to use interaction component on a class not derived from Pawn"));
		return nullptr;
	}
}

void UARInteractionComponent::PrimaryInteract()
{
	AActor* HitActor = GetInteractableObject();
	if (HitActor)
	{
		IARGameplayInterface::Execute_Interact(HitActor,MyOwner);
	}
}

