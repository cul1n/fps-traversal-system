// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"

ETraversalMode UCustomCharacterMovementComponent::GetCurrentTraversalMode() const
{
	return CurrentTraversalMode;
}

void UCustomCharacterMovementComponent::SetCurrentTraversalMode(ETraversalMode NewTraversalMode)
{
	CurrentTraversalMode = NewTraversalMode;
}
