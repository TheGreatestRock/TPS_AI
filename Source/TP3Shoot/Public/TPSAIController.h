// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TPSAIController.generated.h"

/**
 * AI Controller class for handling AI character behavior.
 */
UCLASS()
class TP3SHOOT_API ATPSAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets the rotation of the controlled character to face a specified target location
	void RotateToFaceTarget(const FVector& TargetLocation);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
