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
	if (CurrentStamina == 0.0f && CurrentTraversalMode == ETraversalMode::Sprint)
	{
		NextTraversalMode = ETraversalMode::Walk;
	}

	if (CurrentTraversalMode == ETraversalMode::Walk && NextTraversalMode == ETraversalMode::Sprint &&
		CurrentStamina <= StaminaThreshold)
	{
		NextTraversalMode = ETraversalMode::Walk;
	} 

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
	else if (CurrentTraversalMode == ETraversalMode::Walk)
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
	
	// TODO remove debug message, add stamina in UI
	if (CurrentStamina < StaminaThreshold)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Red,
			FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina));
	} else
	{
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green,
			FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina));
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
	UpdateTraversalMode();
	UpdateStamina(DeltaTime);
}
