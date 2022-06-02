// Fill out your copyright notice in the Description page of Project Settings.


#include "ARCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
// Sets default values
AARCharacter::AARCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ThirdPersonCameraArm = CreateDefaultSubobject<USpringArmComponent>("ThirdPersonSpringArm");
	ThirdPersonCameraArm->bUsePawnControlRotation = true;
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>("ThirdPersonCamera");
	ThirdPersonCameraArm->SetupAttachment(RootComponent);
	ThirdPersonCamera->SetupAttachment(ThirdPersonCameraArm);
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void AARCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AARCharacter::MoveForward(float value)
{
	//we want the forward key to move the character in the direction the camera is pointing as we have split the actor rotation and camera rotation
	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0.0f;
	ControlRotation.Roll = 0.0f;
	AddMovementInput(ControlRotation.Vector(), value);
}

void AARCharacter::Strafe(float value)
{
	//to strafe we need to get the right vector of the controller rotation
	//the code I'm using is taken from KismetMathLibrary::GetRightVector
	FRotator ControlRotation = GetControlRotation();
	FVector ControlRightVector = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);
	AddMovementInput(ControlRightVector, value);
}

void AARCharacter::Primary()
{
	FTransform ProjectileSpawnTransform = FTransform(GetControlRotation(), GetMesh()->GetSocketLocation("Muzzle_01"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
	PlayAnimMontage(PrimaryFireAnim);
	GetWorld()->SpawnActor<AActor>(ProjectileClass, ProjectileSpawnTransform, SpawnParams);
}

// Called every frame
void AARCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AARCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	PlayerInputComponent->BindAxis("MoveForward", this, &AARCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Pitch", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Strafe", this, &AARCharacter::Strafe);

	PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &AARCharacter::Primary);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
}



