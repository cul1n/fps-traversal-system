// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"

#include "GameFramework/Character.h"

ETraversalMode UCustomCharacterMovementComponent::GetCurrentTraversalMode() const
{
	return CurrentTraversalMode;
}

bool UCustomCharacterMovementComponent::IsSliding() const
{
	return CurrentTraversalMode == ETraversalMode::Slide;
}

void UCustomCharacterMovementComponent::RequestCrouch()
{
	if (!IsMovingOnGround())
	{
		return;
	}

	if (CurrentTraversalMode == ETraversalMode::Sprint)
	{
		NextTraversalMode = ETraversalMode::Slide;
	}
	else
	{
		NextTraversalMode = ETraversalMode::Crouch;
	}
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

void UCustomCharacterMovementComponent::UpdateTraversalMode()
{
	// If no change in Traversal mode, no need to apply new params
	if (!CharacterOwner || NextTraversalMode == CurrentTraversalMode)
	{
		return;
	}

	switch (CurrentTraversalMode)
	{
		case ETraversalMode::Walk:
			if (NextTraversalMode == ETraversalMode::Sprint && CurrentStamina <= StaminaThreshold)
			{
				return;
			} 
			if (NextTraversalMode == ETraversalMode::Crouch && IsMovingOnGround())
			{
				CharacterOwner->Crouch();
			}
			break;
		case ETraversalMode::Sprint:
			if (NextTraversalMode == ETraversalMode::Slide && IsMovingOnGround())
			{
				CharacterOwner->Crouch();
				EnterSlide();
			}
			break;
		case ETraversalMode::Crouch:
			if (NextTraversalMode != ETraversalMode::Slide)
			{
				CharacterOwner->UnCrouch();
			}
			break;
		case ETraversalMode::Slide:
			// While in the Slide mode, the character can't switch modes till it finishes the slide by crouching
			if (NextTraversalMode != ETraversalMode::Crouch)
			{
				return;	
			}
			break;
		default:
			break;
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
	GroundFriction = Params->GroundFriction;
	BrakingDecelerationWalking = Params->BrakingDecelerationWalking;

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
	else
	{
		if (CurrentStamina <= StaminaThreshold)
		{
			CurrentStamina += DeltaTime * StaminaFatigueRate;	
		}
		else
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

	// TODO consider removing stamina drain on tick
	UpdateTraversalMode();
	UpdateStamina(DeltaTime);
	
	if (CurrentTraversalMode == ETraversalMode::Slide)
	{
		UpdateSlide(DeltaTime);
	}
}

void UCustomCharacterMovementComponent::EnterSlide()
{
	SlideElapsedTime = 0.f;
	SlideDirection = Velocity.GetSafeNormal();

	if (SlideDirection.IsNearlyZero())
	{
		SlideDirection = CharacterOwner->GetActorForwardVector().GetSafeNormal2D();
	}

	// TODO: consider moving bool in params?
	CharacterOwner->bUseControllerRotationYaw = false;

	const float SlideStartSpeed = FMath::Clamp(Velocity.Size2D(), MinSlideStartSpeed, MaxSlideStartSpeed);
	Velocity = SlideDirection * SlideStartSpeed;
}

void UCustomCharacterMovementComponent::UpdateSlide(float DeltaTime)
{
	SlideElapsedTime += DeltaTime;

	float const NewSlideSpeed = FMath::Max(Velocity.Size2D() - SlideDeceleration * DeltaTime, 0.f);
	Velocity = SlideDirection * NewSlideSpeed;
	
	if (NewSlideSpeed < MinSlideSpeed || SlideElapsedTime > MaxSlideDuration)
	{
		CharacterOwner->bUseControllerRotationYaw = true;
		SetNextTraversalMode(ETraversalMode::Crouch);
	}
}
