#include "UBTTask_IdentifyEnemy.h"
#include "AIController.h"
#include "../TP3ShootCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h" // Optional: For debugging the ray traces

UBTTask_IdentifyEnemy::UBTTask_IdentifyEnemy()
{
    NodeName = TEXT("Identify Enemy");
}

EBTNodeResult::Type UBTTask_IdentifyEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Get the AI controller and the controlled character
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn)
    {
        return EBTNodeResult::Failed;
    }

    // Cast to your specific character class to access teammates
    ATP3ShootCharacter* AICharacter = Cast<ATP3ShootCharacter>(ControlledPawn);
    if (!AICharacter)
    {
        return EBTNodeResult::Failed;
    }

    // Get the teammates from the AI character
    const TArray<ATP3ShootCharacter*>& Teammates = AICharacter->Teammates; // Ensure this is accessible

    // Set up cone parameters
    FVector ConeOrigin = ControlledPawn->GetActorLocation();
    FVector ForwardVector = ControlledPawn->GetActorForwardVector();
    float ConeLength = 1000.0f; // Length of the cone
    float ConeAngle = 45.0f;     // Angle of the cone (in degrees)
    int32 RayCount = 10;         // Number of rays to cast

    bool bEnemyDetected = false;

    // Perform multiple ray traces to simulate a cone
    for (int32 i = 0; i < RayCount; i++)
    {
        // Calculate the angle for this ray relative to the forward vector
        float Alpha = (i / static_cast<float>(RayCount - 1)) * ConeAngle - (ConeAngle / 2.0f);

        // Create a rotation based on the alpha angle around the up vector (Z)
        FQuat Rotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(Alpha));
        FVector RayDirection = Rotation.RotateVector(ForwardVector); // Rotate forward vector to create the cone effect

        FVector Start = ConeOrigin; // Start point of the ray
        FVector End = Start + (RayDirection * ConeLength); // End point of the ray

        // Create collision parameters
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(ControlledPawn); // Ignore the controlled pawn itself

        // Prepare hit result
        FHitResult HitResult;

        // Perform the ray trace
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            Start,
            End,
            ECC_Visibility, // Use the appropriate channel for your game
            CollisionParams
        );

        if (bHit)
        {
            ATP3ShootCharacter* HitCharacter = Cast<ATP3ShootCharacter>(HitResult.GetActor());
            if (HitCharacter && !Teammates.Contains(HitCharacter))
            {
                // If an enemy is identified, update the blackboard
                OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetLocationKey.SelectedKeyName, HitResult.Location); // Set enemy position

                // Optional: Draw debug line to visualize the ray trace
                DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
                bEnemyDetected = true;
                break; // Exit loop on first detection
            }
        }

        // Optional: Draw debug lines for all rays (can be commented out for performance)
        DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 1.0f);
    }

    return EBTNodeResult::Succeeded;
}
