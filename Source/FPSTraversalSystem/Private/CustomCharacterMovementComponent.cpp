// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"

#include "GameFramework/Character.h"

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

float UCustomCharacterMovementComponent::GetStaminaNormalized() const
{
	if (!MaxStamina)
	{
		return 0.0f;
	}

	return CurrentStamina / MaxStamina;
}

float UCustomCharacterMovementComponent::GetStaminaThresholdNormalized() const
{
	if (!MaxStamina)
	{
		return 0.0f;
	}

	return StaminaThreshold / MaxStamina;
}

// TODO: consider refactoring this method
void UCustomCharacterMovementComponent::UpdateTraversalMode()
{
	if (CurrentStamina == 0.0f && CurrentTraversalMode == ETraversalMode::Sprint)
	{
		NextTraversalMode = ETraversalMode::Walk;
	}

	if (CurrentTraversalMode == ETraversalMode::Walk && NextTraversalMode == ETraversalMode::Sprint &&
		CurrentStamina <= StaminaThreshold)
	{
		NextTraversalMode = ETraversalMode::Walk;
	}

	if (NextTraversalMode == CurrentTraversalMode)
	{
		return;
	}

	if (CharacterOwner)
	{
		if (NextTraversalMode == ETraversalMode::Crouch && IsMovingOnGround())
		{
			CharacterOwner->Crouch();
		} else if (CurrentTraversalMode == ETraversalMode::Crouch)
		{
			CharacterOwner->UnCrouch();
		}
	}

	CurrentTraversalMode = NextTraversalMode;
	ApplyTraversalParams(CurrentTraversalMode);
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

void UCustomCharacterMovementComponent::UpdateStamina(float DeltaTime)
{
	if (CurrentTraversalMode == ETraversalMode::Sprint)
	{
		CurrentStamina -= DeltaTime * StaminaRate;

		if (CurrentStamina <= 0.f)
		{
			CurrentStamina = 0.f;
			NextTraversalMode = ETraversalMode::Walk;
		}
	}
	else if (CurrentTraversalMode == ETraversalMode::Walk || CurrentTraversalMode == ETraversalMode::Crouch)
	{
		if (CurrentStamina <= StaminaThreshold)
		{
			CurrentStamina += DeltaTime * StaminaFatigueRate;	
		} else
		{
			CurrentStamina += DeltaTime * StaminaRate;
		}

		if (CurrentStamina >= MaxStamina)
		{
			CurrentStamina = MaxStamina;
		}
	}
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

	// TODO consider removing stamina drain on tick?
	UpdateTraversalMode();
	UpdateStamina(DeltaTime);
}
