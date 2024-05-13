// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatEmUpCharacter.h"
#include "Bullet.h"
#include "DamageTextUI.h"
#include "DynamicSpotlight.h"
#include "HealthPack.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class BEATEMUP_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY()
	ABeatEmUpCharacter* Player;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100;

	UPROPERTY(EditAnywhere)
	float CurrentHealth = MaxHealth;
	
	UPROPERTY(EditAnywhere)
    float DamageToPlayer = 10;
	
    UPROPERTY(EditAnywhere)
    float RagdollTime = 5;
	
    FTimerHandle RagdollTimerHandle;

	UPROPERTY(EditAnywhere)
    TSubclassOf<UDamageTextUI> DamageTextClass;
	
	UPROPERTY(EditAnywhere)
    TSubclassOf<ABullet> BulletClass;

	UPROPERTY(EditAnywhere)
    int EXPAmount = 20;

	UPROPERTY(EditAnywhere)
    UNiagaraSystem* DeathParticleClass;

	UPROPERTY(EditAnywhere)
    TSubclassOf<AHealthPack> HealthPackClass;

	UPROPERTY(EditAnywhere)
    TSubclassOf<ADynamicSpotlight> DynamicSpotlightClass;
	
    UPROPERTY()
    ADynamicSpotlight* Spotlight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void DealDamage(float Damage);

	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector Normal,
		const FHitResult& Hit);
	
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlapComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void Ragdoll();
	
    UFUNCTION()
    void StopRagdoll();

	void Shoot(FVector Direction);
};
