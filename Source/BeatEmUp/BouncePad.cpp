// Fill out your copyright notice in the Description page of Project Settings.


#include "BouncePad.h"

#include "Components/BoxComponent.h"
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

	SpringReleaseTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABouncePad::OnPadCollision);

	Pad->SetSimulatePhysics(true);
	
	const float ZScale = Base->GetComponentScale().Z;
	const float BaseHeight = Base->GetStaticMesh()->GetBounds().GetBox().GetExtent().Z * ZScale;
	const float PadZPos = Pad->GetComponentLocation().Component(2);
	CompressedTargetZPosition = PadZPos - BaseHeight;

	SpringReset();
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
	
	Spring->SetLinearPositionTarget(FVector(0.f, 0.f, CompressedTargetZPosition));
	Pad->WakeRigidBody();
}

void ABouncePad::OnPadCollision(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult)
{	
	Spring->SetLinearPositionTarget(FVector(0.f, 0.f, 0.f));
	Pad->WakeRigidBody();

	FVector Velocity = OtherComp->GetComponentVelocity();
	Velocity.Normalize();
	Velocity.Z = -Velocity.Z;
	OtherComp->AddForce(Velocity);

	GetWorld()->GetTimerManager().SetTimer(
		SpringResetTimerHandle,
		this,
		&ABouncePad::SpringReset,
		SpringResetTime,
		false);
}

