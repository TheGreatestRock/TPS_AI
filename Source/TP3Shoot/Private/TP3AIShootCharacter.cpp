// TP3AIShootCharacter.cpp

#include "TP3AIShootCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AIController.h"

ATP3AIShootCharacter::ATP3AIShootCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // No need to tick; behavior tree handles AI logic
}

void ATP3AIShootCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get the AI controller and start the behavior tree if it's available
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (BehaviorTreeAsset)
		{
			AIController->RunBehaviorTree(BehaviorTreeAsset);
		}
	}
}

UBehaviorTree* ATP3AIShootCharacter::GetBehaviorTree() const
{
	return BehaviorTreeAsset;
}
