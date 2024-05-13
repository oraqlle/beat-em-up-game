// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicSpotlight.h"

#include "Enemy.h"

// Sets default values
ADynamicSpotlight::ADynamicSpotlight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Spotlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
    RootComponent = Spotlight;
}

// Called when the game starts or when spawned
void ADynamicSpotlight::BeginPlay()
{
	Super::BeginPlay();

	if (bStartingSpotlight) {
		TargetActor = GetWorld()->GetFirstPlayerController()->GetPawn();
    }
}

// Called every frame
void ADynamicSpotlight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetActor) {
		FVector TargetDirection = TargetActor->GetActorLocation() - GetActorLocation();
		SetActorRotation(TargetDirection.Rotation());
		
		if (!bStartingSpotlight) {
			if (const AEnemy* Enemy = Cast<AEnemy>(TargetActor)) {
				SetActorLocation(Enemy->GetMesh()->GetComponentLocation() + ActorOffset);
				TargetDirection = Enemy->GetMesh()->GetComponentLocation() - GetActorLocation();
				SetActorRotation(TargetDirection.Rotation());
				Spotlight->SetLightColor(FLinearColor::LerpUsingHSV(
					HealthyColour,
					DeadColour,
					(1 - Enemy->CurrentHealth / Enemy->MaxHealth)));
			} else {
				SetActorLocation(TargetActor->GetActorLocation() + ActorOffset);
			}
		} else {
			if (const ABeatEmUpCharacter* Character = Cast<ABeatEmUpCharacter>(TargetActor)) {
				Spotlight->SetLightColor(FLinearColor::LerpUsingHSV(
					HealthyColour,
					DeadColour,
					(1 - static_cast<float>(Character->CurrentHealth) / Character->MaxHealth)));
			}
		}
    }
}

