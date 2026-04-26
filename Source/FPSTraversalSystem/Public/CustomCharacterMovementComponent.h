// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TraversalMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

/**
 * TODO
 */
UCLASS()
class FPSTRAVERSALSYSTEM_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	ETraversalMode CurrentTraversalMode = ETraversalMode::Walk;
	ETraversalMode NextTraversalMode = ETraversalMode::Walk;

public:
	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	ETraversalMode GetCurrentTraversalMode() const;
	
	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	void SetCurrentTraversalMode(ETraversalMode NewTraversalMode);
	
};
