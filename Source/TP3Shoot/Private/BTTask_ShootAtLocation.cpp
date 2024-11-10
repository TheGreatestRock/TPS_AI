// BTTTask_ShootAtLocation.cpp

#include "BTTask_ShootAtLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../TP3ShootCharacter.h"
#include "Kismet/GameplayStatics.h"

UBTTask_ShootAtLocation::UBTTask_ShootAtLocation()
{
	NodeName = "Shoot At Location";
}

EBTNodeResult::Type UBTTask_ShootAtLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get the AI controller and controlled character
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ATP3ShootCharacter* Character = Cast<ATP3ShootCharacter>(AIController->GetPawn());
	if (!Character) return EBTNodeResult::Failed;

	// Retrieve the target location from the blackboard
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	FVector TargetLocation = BlackboardComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);

	// Perform the shooting action
	Character->ShootAtLocation(TargetLocation);

	return EBTNodeResult::Succeeded;
}
