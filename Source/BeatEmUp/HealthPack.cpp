// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPack.h"

#include "BeatEmUpCharacter.h"

// Sets default values
AHealthPack::AHealthPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));
	PointLight->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHealthPack::BeginPlay()
{
	Super::BeginPlay();

	Mesh->SetCollisionProfileName("OverlapAll");
    Mesh->SetVisibility(false);
    GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AHealthPack::Spawn, 5, false);
    TargetOffset += GetActorLocation();
    InitialPosition = GetActorLocation();
    PointLight->SetIntensity(StartingIntensity);
    PointLight->SetVisibility(false);

	
	if (HealthMaterialClass)
    {
		HealthMaterialInstance = UMaterialInstanceDynamic::Create(HealthMaterialClass, this);
		Mesh->SetMaterial(0, HealthMaterialInstance);
		FLinearColor PickupColour;
		TArray<FMaterialParameterInfo> MaterialParameters;
		TArray<FGuid> ParameterIDs;
		HealthMaterialInstance->GetAllVectorParameterInfo(MaterialParameters,ParameterIDs);
		HealthMaterialInstance->GetVectorParameterValue(MaterialParameters[0], PickupColour);
		PointLight->SetLightColor(PickupColour);
    }
}

// Called every frame
void AHealthPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Mesh->IsVisible()) {
		CurrentLifeTime -= DeltaTime;
		AddActorLocalRotation(FRotator(0,50 * DeltaTime,0));
		FVector NextLocation = FVector(InitialPosition.X, InitialPosition.Y, InitialPosition.Z + sin(GetGameTimeSinceCreation()) * 50);
		SetActorLocation(NextLocation);
		PointLight->SetIntensity(StartingIntensity * (CurrentLifeTime / MaxLifeTime));
		
		if (HealthMaterialInstance) {
			HealthMaterialInstance->SetScalarParameterValue("Emissive", StartingEmissive* (CurrentLifeTime / MaxLifeTime));
			HealthMaterialInstance->SetScalarParameterValue("Transparency", StartingTransparency* (CurrentLifeTime / MaxLifeTime));
		}
		
		if (CurrentLifeTime <= 0) {
			Destroy();
		}
    }
}

void AHealthPack::Spawn()
{
	CurrentLifeTime = MaxLifeTime;
    Mesh->SetVisibility(true);
    PointLight->SetVisibility(true);
    Mesh->OnComponentBeginOverlap.AddDynamic(this, &AHealthPack::OnMeshOverlap);
}

void AHealthPack::OnMeshOverlap(
	UPrimitiveComponent* OverlapComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int BodyIndex,
	bool bSweep,
	const FHitResult& Hit)
{
	if (Cast<ABeatEmUpCharacter>(OtherActor))
    {
		Cast<ABeatEmUpCharacter>(OtherActor)->DealDamage(-50);
		Destroy();
    }
}

