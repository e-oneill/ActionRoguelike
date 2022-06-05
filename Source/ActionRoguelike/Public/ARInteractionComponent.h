// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARInteractionComponent.generated.h"

class AActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UARInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UARInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	APawn* MyOwner;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// This function is used to Get an interactable object using a line trace in a 30cm sphere sweep
	/// </summary>
	UFUNCTION()
	AActor* GetInteractableObject();


	void PrimaryInteract();
};
