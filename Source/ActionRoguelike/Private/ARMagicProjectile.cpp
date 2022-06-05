// Fill out your copyright notice in the Description page of Project Settings.


#include "ARMagicProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AARMagicProjectile::AARMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionProfileName("Projectile");
	SphereComponent->OnComponentHit.AddDynamic(this, &AARMagicProjectile::OnHit);
	RootComponent = SphereComponent;
	
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("ParticleSystem");
	ParticleSystem->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	MovementComponent->InitialSpeed = 1000.0f;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bInitialVelocityInLocalSpace = true;

	ForceComponent = CreateDefaultSubobject<URadialForceComponent>("ForceComponent");
	BaseDamage = 10.f;
	BlastRadius = 50.f;
	bBounces = false;
	ForceComponent->ForceStrength = BaseDamage * 10.f;
	ForceComponent->ImpulseStrength = BaseDamage * 10.f;
	ForceComponent->Radius = BlastRadius;
	ForceComponent->bIgnoreOwningActor = true;
	ForceComponent->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AARMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	CreateTime = GetWorld()->TimeSeconds;
}

void AARMagicProjectile::ExplodeProjectile()
{
	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), GetActorRotation());
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
	TArray<AActor*> IgnoreActors;
	DrawDebugSphere(GetWorld(), GetActorLocation(), BlastRadius, 12, FColor::Red, false, 2.0f);
	FString DebugString = FString::Printf(TEXT("%.2f"),BaseDamage);
	DrawDebugString(GetWorld(), GetActorLocation(), DebugString, nullptr, FColor::Green, 2.0f, true);
	float InnerRadius = BlastRadius/2.f;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), BaseDamage, 1.f, GetActorLocation(), InnerRadius, BlastRadius, 1.f, DamageType, IgnoreActors, GetOwner());
	ForceComponent->FireImpulse();
	MakeNoise(1.f, this->GetInstigator());
	Destroy();
}

// Called every frame
void AARMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BlastDelay > 0.0f && GetWorld()->TimeSeconds - CreateTime >= BlastDelay)
	{
		ExplodeProjectile();
	}

}

void AARMagicProjectile::OnHit(UPrimitiveComponent* hitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{

	if (bBounces)
	{
		Bounces++;
	}
	if ((!bBounces || Bounces >= NumberOfBounces) && OtherActor != GetOwner())
	{
		ExplodeProjectile();
	}
	if (OtherActor != NULL && OtherActor != this && hitComp != NULL && OtherActor != this->GetOwner() && hitComp->IsSimulatingPhysics())
	{
		
	}

}

