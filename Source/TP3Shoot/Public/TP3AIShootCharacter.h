// TP3AIShootCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "../TP3ShootCharacter.h"
#include "TP3AIShootCharacter.generated.h"

class UBehaviorTree;

UCLASS()
class TP3SHOOT_API ATP3AIShootCharacter : public ATP3ShootCharacter
{
	GENERATED_BODY()

public:
	// Constructor
	ATP3AIShootCharacter();

	// AI-specific Behavior Tree
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Function to initialize AI with the Behavior Tree
	UBehaviorTree* GetBehaviorTree() const;

};
