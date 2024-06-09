// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));

	MovementComponent->bShouldBounce= true;
	MovementComponent->bSweepCollision = true;
	MovementComponent->InitialSpeed = MovementSpeed;
	MovementComponent->UpdatedComponent = Mesh;
	MovementComponent->ProjectileGravityScale = 0.5f;
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABomb::Explode()
{
	if (ExplosionEffectClass) {
		[[maybe_unused]] UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffectClass,
			GetActorLocation(),
			GetActorRotation());
	}
	
	Destroy();
}

