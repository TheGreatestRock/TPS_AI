#include "BTTask_SetBooleanBBKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetBooleanBBKey::UBTTask_SetBooleanBBKey()
{
	// Set the Node Name
	NodeName = "Set Boolean Blackboard Key";
}

EBTNodeResult::Type UBTTask_SetBooleanBBKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Check if the Blackboard Component exists
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// Set the Boolean value in the Blackboard
	BlackboardComp->SetValueAsBool(BooleanKey.SelectedKeyName, bValueToSet);

	return EBTNodeResult::Succeeded;
}
