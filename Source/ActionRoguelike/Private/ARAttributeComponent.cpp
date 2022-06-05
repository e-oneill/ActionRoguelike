// Fill out your copyright notice in the Description page of Project Settings.


#include "ARAttributeComponent.h"
#include "Kismet/KismetMathLibrary.h"
// Sets default values for this component's properties
UARAttributeComponent::UARAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	maxHealth = 100.f;
	// ...
}


// Called when the game starts
void UARAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	if (GetOwnerRole() == ROLE_Authority)
	{
		currentHealth = maxHealth;
		AActor* MyOwner = Cast<AActor>(GetOwner());
		if (ensure(MyOwner))
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UARAttributeComponent::handleTakeDamage);
		}
	}
}

void UARAttributeComponent::handleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage)
	{
		currentHealth = UKismetMathLibrary::FClamp(currentHealth - Damage, 0.0f, maxHealth);
		OnHealthChanged.Broadcast(this, DamageCauser, currentHealth, Damage);
		if (currentHealth <= 0.f)
		{
			//we are dead, give the attacker experience points and destroy me
			UARAttributeComponent* attackerAttributes = Cast<UARAttributeComponent>(DamageCauser->GetComponentByClass(UARAttributeComponent::StaticClass()));
			if (attackerAttributes)
			{
				attackerAttributes->experience += experienceValue;
			}
			GetOwner()->Destroy();
		}
		
	}
}

// Called every frame
void UARAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UARAttributeComponent::ChangeHealth(float delta)
{
	currentHealth = UKismetMathLibrary::FClamp(currentHealth + delta, 0.0f, maxHealth);
	UE_LOG(LogTemp, Log, TEXT("Taking %f damage"), delta);
	return true;
}

