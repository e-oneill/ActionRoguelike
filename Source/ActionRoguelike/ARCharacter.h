// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ARCharacter.generated.h"
class UCameraComponent;
class USpringArmComponent;
UCLASS()
class ACTIONROGUELIKE_API AARCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AARCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* ThirdPersonCameraArm;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//input functions
	void MoveForward(float value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
