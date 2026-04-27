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

void UCustomCharacterMovementComponent::SetNextTraversalMode(ETraversalMode NewTraversalMode)
{
	NextTraversalMode = NewTraversalMode;
}

void UCustomCharacterMovementComponent::UpdateTraversalMode()
{
	if (NextTraversalMode != CurrentTraversalMode)
	{
		CurrentTraversalMode = NextTraversalMode;
		ApplyTraversalParams(CurrentTraversalMode);
	}
}

bool UCustomCharacterMovementComponent::ApplyTraversalParams(ETraversalMode TraversalMode)
{
	const FTraversalParams* Params = TraversalParams.Find(TraversalMode);
	
	if (!Params)
	{
		return false;
	}
	
	MaxWalkSpeed = Params->MaxWalkSpeed;
	MaxAcceleration = Params->MaxAcceleration;
	
	return true;
}

void UCustomCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	ApplyTraversalParams(CurrentTraversalMode);
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateTraversalMode();
}
