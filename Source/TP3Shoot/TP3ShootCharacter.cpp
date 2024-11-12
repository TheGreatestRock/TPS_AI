// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP3ShootCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include <Perception/AISense_Sight.h>
#include <BehaviorTree/BlackboardComponent.h>


//////////////////////////////////////////////////////////////////////////
// ATP3ShootCharacter

ATP3ShootCharacter::ATP3ShootCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create SK_Gun
	SK_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	SK_Gun->SetupAttachment(GetMesh());
	// Set parent socket
	SK_Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("GripPoint"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	CurrentHealth = MaxHealth; // Initialize health


}

//////////////////////////////////////////////////////////////////////////
// Input

void ATP3ShootCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ATP3ShootCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ATP3ShootCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ATP3ShootCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ATP3ShootCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATP3ShootCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATP3ShootCharacter::TouchStopped);

	// Aiming 
	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &ATP3ShootCharacter::Aim);
	PlayerInputComponent->BindAction("Aiming", IE_Released, this, &ATP3ShootCharacter::StopAiming);

	// Fire
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATP3ShootCharacter::Fire);

	// Boost Speed
	PlayerInputComponent->BindAction("BoostSpeed", IE_Pressed, this, &ATP3ShootCharacter::BoostSpeed);
	PlayerInputComponent->BindAction("BoostSpeed", IE_Released, this, &ATP3ShootCharacter::RemoveSpeedBoost);
}

void ATP3ShootCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ATP3ShootCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ATP3ShootCharacter::Aim()
{
	IsAiming = true;
}

void ATP3ShootCharacter::StopAiming()
{
	IsAiming = false;
}

void ATP3ShootCharacter::Fire()
{
	if (!IsAiming) return;

	// Start the trace from the gun's muzzle
	FVector MuzzleLocation = SK_Gun->GetSocketLocation("MuzzleFlash");
	FVector CameraLocation = FollowCamera->GetComponentLocation();
	FVector ForwardVector = FollowCamera->GetForwardVector();

	// Calculate an endpoint far away in the direction the camera is facing
	FVector CenterScreenTraceEnd = CameraLocation + (ForwardVector * 10000);

	// Perform a line trace from the camera to the center of the screen
	FHitResult CameraHitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// Perform the first line trace from the camera
	bool bHitCamera = GetWorld()->LineTraceSingleByChannel(
		CameraHitResult,
		CameraLocation,
		CenterScreenTraceEnd,
		ECC_Visibility,
		Params
	);

	// Log details if the first trace hit something
	if (bHitCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("Camera Hit!"));
		UE_LOG(LogTemp, Warning, TEXT("Hit Location: %s"), *CameraHitResult.Location.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *CameraHitResult.GetActor()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *CameraHitResult.GetComponent()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Impact Normal: %s"), *CameraHitResult.ImpactNormal.ToString());
		//if the camera hit that can be cast to a TP3ShootCharacter
		if (ATP3ShootCharacter* HitCharacter = Cast<ATP3ShootCharacter>(CameraHitResult.GetActor()))
		{
			// Call TakeDamage function
			HitCharacter->TakeDamage(10);
		}
	}

	FVector TargetPoint = bHitCamera ? CameraHitResult.Location : CenterScreenTraceEnd;

	// Perform a second line trace from the muzzle to the target point
	FHitResult MuzzleHitResult;
	bool bHitMuzzle = GetWorld()->LineTraceSingleByChannel(
		MuzzleHitResult,
		MuzzleLocation,
		TargetPoint,
		ECC_Visibility,
		Params
	);

	// Log details if the second trace hit something
	if (bHitMuzzle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Muzzle Hit!"));
		UE_LOG(LogTemp, Warning, TEXT("Hit Location: %s"), *MuzzleHitResult.Location.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *MuzzleHitResult.GetActor()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *MuzzleHitResult.GetComponent()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Impact Normal: %s"), *MuzzleHitResult.ImpactNormal.ToString());
	}

	// Determine the final end point of the trace
	FVector LineTraceEnd = bHitMuzzle ? MuzzleHitResult.Location : TargetPoint;

	// Draw a debug line from the muzzle to the calculated end point
	DrawDebugLine(
		GetWorld(),
		MuzzleLocation,
		LineTraceEnd,
		FColor::Red,       // Line color
		false,             // Persistent (will stay) or not
		1.0f,              // Duration the line will be drawn
		0,                 // Depth priority
		1.0f               // Line thickness
	);

	UE_LOG(LogTemp, Warning, TEXT("Fire!"));
	PlayFireSound();
}





void ATP3ShootCharacter::BoostSpeed()
{
	// Set Max walking speed to 800
	GetCharacterMovement()->MaxWalkSpeed = 800.f;

	GetWorld()->GetTimerManager().SetTimer(BoostSpeedTimer, [&]()
		{
			// Set Max walking speed to 500
			GetCharacterMovement()->MaxWalkSpeed = 500.f;
			
			// Clear existing timer boost speed
			GetWorldTimerManager().ClearTimer(BoostSpeedTimer);

		}, 4, false);
}

void ATP3ShootCharacter::RemoveSpeedBoost()
{
	// Set Max walking speed to 500
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}


void ATP3ShootCharacter::FireParticle(FVector Start, FVector Impact)
{
	if (!ParticleStart || !ParticleImpact) return;

	FTransform ParticleT;

	ParticleT.SetLocation(Start);

	ParticleT.SetScale3D(FVector(0.25, 0.25, 0.25));

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleStart, ParticleT, true);

	// Spawn particle at impact point
	ParticleT.SetLocation(Impact);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleImpact, ParticleT, true);

}

void ATP3ShootCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ATP3ShootCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ATP3ShootCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATP3ShootCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ATP3ShootCharacter::TakeDamage(float DamageAmount, ATP3ShootCharacter* Attacker)
{
	if (DamageAmount <= 0) return;
	LastAttacker = Attacker;
	CurrentHealth -= DamageAmount;
	if (CurrentHealth <= 0)
	if (CurrentHealth <= 0)
	{
		// Handle death
		Respawn(SpawnLocation);
	}
}


void ATP3ShootCharacter::Respawn(FVector RespawnLocation)
{
	SetActorLocation(RespawnLocation); // Move to spawn location
	CurrentHealth = MaxHealth; // Reset health
}

void ATP3ShootCharacter::ShootAtLocation(FVector TargetLocation)
{
	
	FVector DirectionToTarget = (TargetLocation - GetActorLocation()).GetSafeNormal();
	FRotator TargetRotation = DirectionToTarget.Rotation();
	TargetRotation.Pitch = 0;

	if (Controller)
	{
		Controller->SetControlRotation(TargetRotation);
	}
	else
	{
		SetActorRotation(TargetRotation);
	}

	FireAtTarget(TargetLocation);
}


void ATP3ShootCharacter::FireAtTarget(FVector TargetLocation)
{
	// Muzzle location for spawning particles and determining shot direction
	FVector MuzzleLocation = SK_Gun->GetSocketLocation("MuzzleFlash");

	// Trace from muzzle to target location
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, this);

	// Perform line trace from the muzzle to the target location
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		MuzzleLocation,
		TargetLocation,
		ECC_Visibility,
		TraceParams
	);

	// Step 5: Process hit results and spawn visual effects
	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Hit Location: %s"), *HitResult.Location.ToString());

		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			// Check if the hit actor is of type ATP3ShootCharacter
			if (ATP3ShootCharacter* HitCharacter = Cast<ATP3ShootCharacter>(HitActor))
			{
				HitCharacter->TakeDamage(10, this); // Apply damage if the cast is successful
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit actor is not a TP3ShootCharacter: %s"), *HitActor->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HitResult.GetActor() returned null."));
		}

		// Play hit particle at impact location
		FireParticle(MuzzleLocation, HitResult.Location);
	}
	else
	{
		// If no hit, trace endpoint for visual effect
		FireParticle(MuzzleLocation, TargetLocation);
	}

	// Step 6: Draw debug line for visual feedback (optional)
	DrawDebugLine(
		GetWorld(),
		MuzzleLocation,
		bHit ? HitResult.Location : TargetLocation,
		FColor::Red,
		false,
		1.0f,
		0,
		1.0f
	);

	// Log firing event
	UE_LOG(LogTemp, Warning, TEXT("Shot fired at target location!"));

	PlayFireSound();
}

void ATP3ShootCharacter::PlayFireSound()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}