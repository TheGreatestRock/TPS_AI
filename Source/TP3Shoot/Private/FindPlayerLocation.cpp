#include "FindPlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UFindPlayerLocation::UFindPlayerLocation()
{
    NodeName = "Find Player Location";
}

EBTNodeResult::Type UFindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Get the player pawn
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerComp.GetWorld(), 0);
    if (PlayerPawn)
    {
        // Get the enemy character from the blackboard
        AActor* EnemyActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(EnemyCharacter.SelectedKeyName));

        // Check if the player pawn is the same as the enemy character
        if (EnemyActor && EnemyActor == PlayerPawn)
        {
            // Get the player's location
            FVector PlayerLocation = PlayerPawn->GetActorLocation();

            // Set the location in the blackboard using TargetLocationKey
            OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetLocationKey.SelectedKeyName, PlayerLocation);

            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}
