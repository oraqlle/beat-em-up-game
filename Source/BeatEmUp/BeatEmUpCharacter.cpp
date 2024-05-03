// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpCharacter.h"

#include "Enemy.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Interactable.h"
#include "Kismet/GameplayStatics.h"

#define __FIT2096_BUILD_OPT__LAB_BASED_USE 0

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ABeatEmUpCharacter

ABeatEmUpCharacter::ABeatEmUpCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ABeatEmUpCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	InGameUI = Cast<UInGameUI>(CreateWidget(GetGameInstance(), InGameUIClass));
    InGameUI->Player = this;
    InGameUI->UpdateValues();
    InGameUI->AddToViewport();
}

void ABeatEmUpCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetWorld()->GetTimerManager().TimerExists(PunchCooldownTimerHandle))
    {
		InGameUI->UpdateValues();
    }

#if !__FIT2096_BUILD_OPT__LAB_BASED_USE
	FindUseTarget();
#endif
}

void ABeatEmUpCharacter::Punch()
{
	if(bPunchReady)
    {
		bPunchReady = false;
		GetWorld()->GetTimerManager().SetTimer(
			PunchCooldownTimerHandle,
			this,
			&ABeatEmUpCharacter::ResetPunch,
			PunchCooldown,
			false);

		TArray<FHitResult> HitResults;
        const FVector Start = GetActorLocation();
        const FVector End = Start + GetActorForwardVector() * PunchDistance;
        const FCollisionShape CubeShape = FCollisionShape::MakeBox(FVector(PunchDistance));
		[[maybe_unused]] const bool bSweep = GetWorld()->SweepMultiByChannel(HitResults,
        	End,
        	End,
        	GetActorQuat(),
        	ECC_WorldDynamic,
        	CubeShape);

		TArray<AActor*> HitThisPunch;
		for(FHitResult HitResult : HitResults)
		{
			if (HitResult.GetActor() != this && !HitThisPunch.Contains(HitResult.GetActor()))
			{
				HitThisPunch.Add(HitResult.GetActor());
				if (AEnemy* HitEnemy = Cast<AEnemy>(HitResult.GetActor()))
				{
					HitEnemy->Ragdoll();
					FVector LaunchDirection = HitEnemy->GetActorLocation() - GetActorLocation();
					LaunchDirection.Normalize();
					LaunchDirection *= 3;
					LaunchDirection += FVector::UpVector;
					HitEnemy->GetMesh()->AddImpulse(LaunchDirection * PunchForce);
					HitEnemy->DealDamage(PunchDamage);
				}
			}
		}
    }
}

void ABeatEmUpCharacter::ResetPunch()
{
	bPunchReady = true;
	InGameUI->UpdateValues();
}

void ABeatEmUpCharacter::DealDamage(const float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0, MaxHealth);
	
	InGameUI->UpdateValues();
	
    if (CurrentHealth == 0)
    {
		Cast<APlayerController>(GetController())->RestartLevel();
    }
}

void ABeatEmUpCharacter::Use()
{
#if __FIT2096_BUILD_OPT__LAB_BASED_USE
	FVector Start = GetActorLocation();
    FVector End = Start + GetFollowCamera()->GetForwardVector() * UseDistance;
	
    FHitResult HitData;
	
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
	
	FCollisionQueryParams TraceParams;
    TraceParams.bTraceComplex = true;
    TraceParams.bReturnPhysicalMaterial = false;
    TraceParams.AddIgnoredActors(ActorsToIgnore);
    TraceParams.TraceTag = FName("Use Trace Tag");
    GetWorld()->DebugDrawTraceTag = TraceParams.TraceTag;

	if (bool bSweep = GetWorld()->LineTraceSingleByChannel(HitData, Start, End, ECC_Visibility, TraceParams)) {
		if (HitData.GetActor()) {
			UE_LOG(LogTemp, Warning, TEXT("We hit: %s"), *HitData.GetActor()->GetName());
			
			if (HitData.GetActor()->GetClass()->ImplementsInterface(UInteractable::StaticClass())) {
				IInteractable::Execute_Interact(HitData.GetActor());
			} else {
				UE_LOG(LogTemp, Warning, TEXT("Actor not interactable!"));
			}
		} else {
			UE_LOG(LogTemp, Warning, TEXT("What happened!?"));
		}
    } else {
		UE_LOG(LogTemp, Warning, TEXT("Didn't hit an actor!"));
    }
#else
	if (UseTarget != nullptr) {
		if (FVector::Distance(UseTarget->GetActorLocation(), GetActorLocation()) <= UseDistance) {
			IInteractable::Execute_Interact(UseTarget);
		}
	}
#endif
}

void ABeatEmUpCharacter::ChargeDash(const FInputActionInstance& Instance)
{	
	if (!bDashReady) { return; }

	// Promote comparison to integer and multiply by scale which goes to zero
	// if condition fails, avoiding cond-branch
	DashChargeScalar = FMath::Clamp(
		(DashChargeRate * Instance.GetElapsedTime() * (+(DashChargeScalar < DashChargeLimit))) + DashChargeScalar,
		0.f,
		DashChargeLimit);

	InGameUI->UpdateValues();
}

void ABeatEmUpCharacter::ReleaseChargeDash()
{
	if (!bDashReady) { return; }

	bDashing = true;

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(
			2,
			2.f,
			FColor::Green,
			FString::Printf(TEXT("Dashing force: %.3f"), DashChargeScalar));
	}

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	
	FVector DashVel = GetActorForwardVector();
	DashVel.Normalize();
	DashVel *= DashChargeScalar;
	Cast<UCharacterMovementComponent>(GetMovementComponent())->AddForce(DashVel * 10000);
	// LaunchCharacter(DashVel, false, false);

	bDashReady = false;
	
	GetWorld()->GetTimerManager().SetTimer(
			DashCooldownTimerHandle,
			this,
			&ABeatEmUpCharacter::ResetDash,
			DashCooldown,
			false);
	
	GetWorld()->GetTimerManager().SetTimer(
			DashTimerHandle,
			this,
			&ABeatEmUpCharacter::IsDashingSwitch,
			GetActorLocation().Size() / DashVel.Size(),  // Find time dashing for
			false);
}

void ABeatEmUpCharacter::ResetDash()
{
	bDashReady = true;
}

void ABeatEmUpCharacter::IsDashingSwitch()
{
	bDashing = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);
	DashChargeScalar = 0.f;
	InGameUI->UpdateValues();
}

void ABeatEmUpCharacter::FindUseTarget()
{
	if (UsePrompt) {
		UsePrompt->RemoveFromParent();
		UsePrompt = nullptr;
		
		TArray<UStaticMeshComponent*> Components;
		UseTarget->GetComponents(Components);

		for (auto i = 0; i < Components.Num(); ++i) {
			Components[i]->SetRenderCustomDepth(false);
		}
	}
	
	FVector Start = GetActorLocation();
	FVector End = Start + GetFollowCamera()->GetForwardVector() * UseDistance;
	
	FHitResult HitData;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActors(ActorsToIgnore);
	TraceParams.TraceTag = FName("Use Find Trace Tag");
	// GetWorld()->DebugDrawTraceTag = TraceParams.TraceTag;

	if (bool bSweep = GetWorld()->LineTraceSingleByChannel(HitData, Start, End, ECC_Visibility, TraceParams)) {
		if (HitData.GetActor()) {
			UE_LOG(LogTemp, Warning, TEXT("We hit: %s"), *HitData.GetActor()->GetName());
			
			if (HitData.GetActor()->GetClass()->ImplementsInterface(UInteractable::StaticClass())) {

				// Add highlight/text for interaction
				UseTarget = HitData.GetActor();
				
				TArray<UStaticMeshComponent*> Components;
				UseTarget->GetComponents(Components);

				for (auto i = 0; i < Components.Num(); ++i) {
					Components[i]->SetRenderCustomDepth(true);
				}
				
				FVector2D TextLocation = FVector2D();
				UGameplayStatics::ProjectWorldToScreen(
					GetWorld()->GetFirstPlayerController(),
					UseTarget->GetActorLocation() + FVector(0.f, 0.f, 1.5f),
					TextLocation);

				UsePrompt = Cast<UUsePromptUI>(CreateWidget(GetGameInstance(), UsePromptClass));
				UsePrompt->SetPositionInViewport(TextLocation);
				UsePrompt->PromptText->SetText(FText::FromString("Click right-mouse to interact"));
				UsePrompt->AddToViewport();
				
			} else {
				UE_LOG(LogTemp, Warning, TEXT("Actor not interactable!"));
			}
		} else {
			UE_LOG(LogTemp, Warning, TEXT("What happened!?"));
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Didn't hit an actor!"));
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABeatEmUpCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABeatEmUpCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABeatEmUpCharacter::Look);

		// Punch
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &ABeatEmUpCharacter::Punch);

		// Interact/Use
		EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Started, this, &ABeatEmUpCharacter::Use);

		// Dash Attack		
		// Binding for ongoing to increment hold duration
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ABeatEmUpCharacter::ChargeDash);
		// Binding for when dash gets released
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &ABeatEmUpCharacter::ReleaseChargeDash);
		// Binding for if key is not pressed for long enough to trigger
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Canceled, this, &ABeatEmUpCharacter::ResetDash);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABeatEmUpCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABeatEmUpCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}