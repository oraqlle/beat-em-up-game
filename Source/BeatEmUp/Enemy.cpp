// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "BrainComponent.h"
#include "EnemyBTController.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<ABeatEmUpCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);

	if (DynamicSpotlightClass)
    {
	    const FVector SpawnLocation = GetActorLocation();
		Spotlight = Cast<ADynamicSpotlight>(GetWorld()->SpawnActor(DynamicSpotlightClass, &SpawnLocation));
		Spotlight->TargetActor = this;
    }
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::DealDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0, MaxHealth);
	
	UDamageTextUI* SpawnedDamage = Cast<UDamageTextUI>(CreateWidget(GetGameInstance(), DamageTextClass));
    UGameplayStatics::ProjectWorldToScreen(
    	GetWorld()->GetFirstPlayerController(),
    	GetMesh()->GetComponentLocation(),
    	SpawnedDamage->CurrentLocation);
    SpawnedDamage->DamageText->SetText(FText::FromString(FString::SanitizeFloat(Damage)));
    SpawnedDamage->TargetLocation += SpawnedDamage->CurrentLocation;
    SpawnedDamage->AddToViewport();
}

void AEnemy::OnHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector Normal,
	const FHitResult& Hit)
{
	if (OtherActor == Player)
    {
		Player->DealDamage(DamageToPlayer);
		Destroy();
    }
}

void AEnemy::OnOverlapBegin(
	UPrimitiveComponent* OverlapComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Overlap collision channel used when Player is dashing to
	// get different interactions
	Ragdoll();
	FVector LaunchDirection = GetActorLocation() - Player->GetActorLocation();
	LaunchDirection.Normalize();
	LaunchDirection *= 3;
	LaunchDirection += FVector::UpVector;
	GetMesh()->AddImpulse(LaunchDirection * Player->ThrusterKnockBackScalar);
	DealDamage(Player->ThrusterChargeDamage);
}

void AEnemy::Ragdoll()
{
	Cast<AEnemyBTController>(GetController())->BrainComponent->PauseLogic("Ragdolling!");
	GetMesh()->SetCollisionProfileName("Ragdoll");
    GetMesh()->SetSimulatePhysics(true);
    GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
    GetWorld()->GetTimerManager().SetTimer(
    	RagdollTimerHandle,
    	this,
    	&AEnemy::StopRagdoll,
    	RagdollTime,
    	false);
}

void AEnemy::StopRagdoll()
{
	if (CurrentHealth <= 0) {
		Player->AddEXP(EXPAmount);

		if (DeathParticleClass) {
			UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				DeathParticleClass,
				GetMesh()->GetComponentLocation());
			
			SpawnedEffect->SetColorParameter(FName("User.SpawnColour"), FLinearColor::MakeRandomColor());
        }

		if (HealthPackClass) {
			FVector SpawnLocation = GetMesh()->GetComponentLocation() + FVector(0,0,50);
			GetWorld()->SpawnActor(HealthPackClass, &SpawnLocation);
		}
		
		Spotlight->Destroy();
		
		Destroy();
		return;
    }
	
    GetMesh()->SetSimulatePhysics(false);
    GetMesh()->SetCollisionProfileName("CharacterMesh");
    GetCapsuleComponent()->SetWorldLocation(GetMesh()->GetSocketLocation("pelvis"));
    GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
    GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90), FRotator(0,-90,0));
    GetCapsuleComponent()->SetCollisionProfileName("Pawn");
	
	Cast<AEnemyBTController>(GetController())->BrainComponent->ResumeLogic("Moving Again!");
	
	GetCharacterMovement()->GravityScale = 1;
}

void AEnemy::Shoot(FVector Direction)
{
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100;
    FRotator SpawnRotation = Direction.Rotation();
    ABullet* SpawnedBullet = Cast<ABullet>(GetWorld()->SpawnActor(BulletClass, &SpawnLocation, &SpawnRotation));
    Direction.Normalize();
    SpawnedBullet->MovementComponent->Velocity = Direction * SpawnedBullet->MovementSpeed;
}

