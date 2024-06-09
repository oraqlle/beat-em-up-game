// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"

#include "Enemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/StaticMeshActor.h"

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
	
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));
	PointLight->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
	
	PointLight->SetLightColor(FLinearColor(255.f, 0.f, 0.f));
	
	if (FlickeringLightMaterialClass) {
		FlickeringLightMaterialInstance = UMaterialInstanceDynamic::Create(FlickeringLightMaterialClass, this);
		PointLight->SetLightFunctionMaterial(FlickeringLightMaterialInstance);
	}
}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FlickeringLightMaterialInstance) {
		FlickeringLightMaterialInstance->SetScalarParameterValue(
			FName(TEXT("Flicker Rate")),
			GetWorld()->GetTimerManager().GetTimerElapsed(FuseTimerHandle) / 2.35f);
	}
}

void ABomb::Explode()
{
	
	TArray<FHitResult> HitResults;
	// const FVector Start = GetActorLocation();
	// const FVector End = Start + GetActorForwardVector() * ExplosionRadius;
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
	[[maybe_unused]] const bool bSweep = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation(),
		GetActorQuat(),
		ECC_WorldDynamic,
		SphereShape);

	TArray<AActor*> HitThisPunch;
	for(FHitResult HitResult : HitResults)
	{
		if (HitResult.GetActor() != this && !HitThisPunch.Contains(HitResult.GetActor()))
		{
			HitThisPunch.Add(HitResult.GetActor());
			
			if (AEnemy* HitEnemy = Cast<AEnemy>(HitResult.GetActor())) {
				HitEnemy->Ragdoll();
				FVector LaunchDirection = HitEnemy->GetActorLocation() - GetActorLocation();
				LaunchDirection.Normalize();
				LaunchDirection *= 3;
				LaunchDirection += FVector::UpVector;
				HitEnemy->GetMesh()->AddImpulse(LaunchDirection * KnockBackForce * 2);
				HitEnemy->DealDamage(Damage);
			} else if (ABeatEmUpCharacter* HitPlayer = Cast<ABeatEmUpCharacter>(HitResult.GetActor())) {
				HitPlayer->DealDamage(Damage);
			} else if (AActor* HitActor = HitResult.GetActor(); HitActor->GetRootComponent()->IsSimulatingPhysics()) {
				if (AStaticMeshActor* HitStaticMeshActor = Cast<AStaticMeshActor>(HitActor)) {
					FVector LaunchDirection = HitActor->GetActorLocation() - GetActorLocation();
					LaunchDirection.Normalize();
					LaunchDirection *= 3;
					LaunchDirection += FVector::UpVector;
					HitStaticMeshActor->GetStaticMeshComponent()->AddImpulse(LaunchDirection * KnockBackForce);
				}
			}
		}
	}
	
	if (ExplosionEffectClass) {
		[[maybe_unused]] UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffectClass,
			GetActorLocation(),
			GetActorRotation());

		SpawnedEffect->SetColorParameter("User.PrimaryColour", FLinearColor::MakeRandomColor());
		SpawnedEffect->SetColorParameter("User.SecondaryColour", FLinearColor::MakeRandomColor());
	}
	
	Destroy();
}
