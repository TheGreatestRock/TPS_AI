#include "TPSAIController.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

void ATPSAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSAIController::RotateToFaceTarget(const FVector& TargetLocation)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	// Calculate direction to target
	FVector DirectionToTarget = (TargetLocation - ControlledPawn->GetActorLocation()).GetSafeNormal();
	FRotator TargetRotation = DirectionToTarget.Rotation();

	// Lock pitch to zero for horizontal aiming
	TargetRotation.Pitch = 0;

	// Apply the rotation to the AI controller's control rotation
	SetControlRotation(TargetRotation);

	// Debug log to verify rotation
	UE_LOG(LogTemp, Warning, TEXT("AI Controller rotated to face target location: %s"), *TargetRotation.ToString());
}
