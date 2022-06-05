// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ARCharacter.generated.h"
class UCameraComponent;
class USpringArmComponent;
class UARInteractionComponent;
class UARAttributeComponent;
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

	UPROPERTY(VisibleAnywhere)
		UARInteractionComponent* InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UARAttributeComponent* AttributeComponent;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//input functions
	void MoveForward(float value);
	void Strafe(float value);
	/// <summary>
	///		This function sets a new target zoom for the player's camera, clamped between a minimum and maximum value
	/// </summary>
	void ZoomCamera(float value);
	bool bZoomCamera;
	float TargetZoom;
	void Interact();
	UPROPERTY(BlueprintReadOnly)
	bool bCanInteract;

	//combat functions
	void Primary();
	//Timer used to track when the primary attack should be fired
	FTimerHandle PrimaryAttackTimer;
	//Timer used to give primary attack a cooldown
	float attackTime;
	UFUNCTION()
	void PrimaryAttack();
	

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere)
	UAnimMontage* PrimaryFireAnim;

	//interpolation functions
	void InterpCameraZoom(float deltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
