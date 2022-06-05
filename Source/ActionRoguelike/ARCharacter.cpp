// Fill out your copyright notice in the Description page of Project Settings.


#include "ARCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ARInteractionComponent.h"
#include "ARAttributeComponent.h"
// Sets default values
AARCharacter::AARCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ThirdPersonCameraArm = CreateDefaultSubobject<USpringArmComponent>("ThirdPersonSpringArm");
	ThirdPersonCameraArm->bUsePawnControlRotation = true;
	ThirdPersonCameraArm->TargetArmLength = 300.f;
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>("ThirdPersonCamera");
	ThirdPersonCameraArm->SetupAttachment(RootComponent);
	ThirdPersonCamera->SetupAttachment(ThirdPersonCameraArm);

	InteractionComponent = CreateDefaultSubobject<UARInteractionComponent>("InteractionComponent");

	AttributeComponent = CreateDefaultSubobject<UARAttributeComponent>("AttributeComponent");

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bCanInteract = false;
}

// Called when the game starts or when spawned
void AARCharacter::BeginPlay()
{
	Super::BeginPlay();
	TargetZoom = ThirdPersonCameraArm->TargetArmLength;
	bCanInteract = false;
	bZoomCamera = false;
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

void AARCharacter::ZoomCamera(float value)
{
	//axis constantly provides input, only set variable if value is significant
	if (value != 0.0f)
	{
	//multiply value by -1 to make it inverse
		TargetZoom = UKismetMathLibrary::FClamp(ThirdPersonCameraArm->TargetArmLength + (-1*value * 100.f),150.f,800.f);
		bZoomCamera = true;
		UE_LOG(LogTemp, Log, TEXT("Setting camera zoom, target zoom is %f"), TargetZoom);
	}
}

void AARCharacter::Interact()
{
	if (InteractionComponent)
	{
		InteractionComponent->PrimaryInteract();
	}
}

void AARCharacter::Primary()
{
	float timerLength = 0.2f;
	float timeSinceLastShot = GetWorld()->TimeSeconds - attackTime;
	
	if (!GetWorldTimerManager().IsTimerActive(PrimaryAttackTimer) && timeSinceLastShot > (timerLength*3))
	{
		UE_LOG(LogTemp, Log, TEXT("Time since last used primary: %f"), timeSinceLastShot);
		PlayAnimMontage(PrimaryFireAnim);
		attackTime = GetWorld()->TimeSeconds;
		GetWorldTimerManager().SetTimer(PrimaryAttackTimer, this, &AARCharacter::PrimaryAttack, timerLength);
	}
	
}

void AARCharacter::PrimaryAttack()
{
	if (ensure(ProjectileClass))
	{
		FVector TargetVector;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FHitResult Hit;
		FVector End;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		FCollisionObjectQueryParams queryParams;
		queryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		queryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		queryParams.AddObjectTypesToQuery(ECC_Pawn);
		FVector Start = ThirdPersonCamera->GetComponentLocation() + (ThirdPersonCamera->GetForwardVector() * 150.f);

		End = Start + (GetControlRotation().Vector() * 7500.f);
		FCollisionShape Shape;
		Shape.SetSphere(20.f);

		if (GetWorld()->SweepSingleByObjectType(Hit, Start, End, FQuat::Identity, queryParams, Shape, Params))
		{
			TargetVector = Hit.ImpactPoint;

		}
		else
		{
			TargetVector = End;
		}
		DrawDebugSphere(GetWorld(), TargetVector, 20.f, 12, FColor::White, false, 2.f);
		FVector SpawnLocation = GetMesh()->GetSocketLocation("Muzzle_01");
		FRotator SpawnRotation = FRotationMatrix::MakeFromX(TargetVector - SpawnLocation).Rotator();
		FTransform ProjectileSpawnTransform = FTransform(SpawnRotation, SpawnLocation);

		AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, ProjectileSpawnTransform, SpawnParams);
		Projectile->SetOwner(this);
		Projectile->SetInstigator(this);
		GetWorldTimerManager().ClearTimer(PrimaryAttackTimer);
		float timerLength = 0.2f;
	}
	
}

void AARCharacter::InterpCameraZoom(float DeltaTime)
{
	ThirdPersonCameraArm->TargetArmLength = UKismetMathLibrary::FInterpTo(ThirdPersonCameraArm->TargetArmLength, TargetZoom, DeltaTime, 5.f);
	UE_LOG(LogTemp, Log, TEXT("Interping camera zoom, target zoom is %f"), TargetZoom);
	if (ThirdPersonCameraArm->TargetArmLength == TargetZoom)
	{
		bZoomCamera = false;
	}
}

// Called every frame
void AARCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//we should interp the camera to its target position
	if (bZoomCamera)
	{
		InterpCameraZoom(DeltaTime);
	}
	//check if we have an object we can interact with
	if (InteractionComponent)
	{
		AActor* InteractableObject = InteractionComponent->GetInteractableObject();
		if (InteractableObject && !bCanInteract)
		{
			//UE_LOG(LogTemp, Log, TEXT("There is a interactable object nearby."));
			bCanInteract = true;
		}
		else if (bCanInteract)
		{
			bCanInteract = false;
		}
	}
}

// Called to bind functionality to input
void AARCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AARCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Pitch", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Strafe", this, &AARCharacter::Strafe);
	PlayerInputComponent->BindAxis("Zoom", this, &AARCharacter::ZoomCamera);

	PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &AARCharacter::Primary);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AARCharacter::Interact);
}



