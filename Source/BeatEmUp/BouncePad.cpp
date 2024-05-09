// Fill out your copyright notice in the Description page of Project Settings.


#include "BouncePad.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
ABouncePad::ABouncePad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
    Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
    Pad = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pad"));
    Spring = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Spring"));
	SpringReleaseTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Spring Release Trigger"));
	
    Base->SetupAttachment(RootComponent);
    Pad->SetupAttachment(RootComponent);
    Spring->SetupAttachment(RootComponent);
	SpringReleaseTrigger->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABouncePad::BeginPlay()
{
	Super::BeginPlay();

	SpringReleaseTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABouncePad::OnPadOverlapBegin);

	Pad->SetSimulatePhysics(true);

	// SpringReset();
}

// Called every frame
void ABouncePad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABouncePad::SpringReset()
{
	// if (GEngine) {
	// 	GEngine->AddOnScreenDebugMessage(
	// 		1,
	// 		100.0f,
	// 		FColor::Yellow,
	// 		FString::Printf(
	// 			TEXT("BaseHeight: %.3f\nPadZPos: %.3f\nZScale: %.3f"),
	// 			BaseHeight,
	// 			PadZPos,
	// 			ZScale));
	// }
	
	// Spring->SetLinearPositionTarget(FVector(0.f, 0.f, CompressedTargetZPosition));
	// Pad->WakeRigidBody();
}

void ABouncePad::OnPadOverlapBegin(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult)
{

	if (OtherComp == Pad) { return; }

	if (ACharacter* Character = Cast<ACharacter>(OtherActor); Character != nullptr) {
		const FVector LaunchVelocity = Character->GetActorUpVector() * (BounceForce * 0.75f);
		Character->LaunchCharacter(LaunchVelocity,true, true);
	} else {
		const FVector LaunchVelocity = OtherComp->GetUpVector() * BounceForce;
		OtherComp->SetSimulatePhysics(true);
		OtherComp->AddImpulse(LaunchVelocity);
		OtherComp->SetSimulatePhysics(false);
	}

	// GetWorld()->GetTimerManager().SetTimer(
	// 	SpringResetTimerHandle,
	// 	this,
	// 	&ABouncePad::SpringReset,
	// 	SpringResetTime,
	// 	false);
}

