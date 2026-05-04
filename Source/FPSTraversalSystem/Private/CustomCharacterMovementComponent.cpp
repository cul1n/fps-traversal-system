// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"

#include "GameCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

ETraversalMode UCustomCharacterMovementComponent::GetCurrentTraversalMode() const
{
	return CurrentTraversalMode;
}

bool UCustomCharacterMovementComponent::IsSliding() const
{
	return CurrentTraversalMode == ETraversalMode::Slide;
}

bool UCustomCharacterMovementComponent::IsMantling() const
{
	return CurrentTraversalMode == ETraversalMode::Mantle;
}

bool UCustomCharacterMovementComponent::IsVaulting() const
{
	return CurrentTraversalMode == ETraversalMode::Mantle and CurrentMantle.bIsVault;
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
			else if (NextTraversalMode == ETraversalMode::Mantle)
			{
				EnterMantle();
			}
			break;
		case ETraversalMode::Sprint:
			if (NextTraversalMode == ETraversalMode::Slide && IsMovingOnGround())
			{
				CharacterOwner->Crouch();
				EnterSlide();
			}
			else if (NextTraversalMode == ETraversalMode::Mantle)
			{
				EnterMantle();
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
		case ETraversalMode::Mantle:
			// Wait for current mantle to be completed
			if (CurrentMantle.bInProgress)
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
	else if (CurrentTraversalMode == ETraversalMode::Mantle)
	{
		UpdateMantle(DeltaTime);
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
		SetNextTraversalMode(ETraversalMode::Crouch);
	}
}

float UCustomCharacterMovementComponent::CapR() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
}

float UCustomCharacterMovementComponent::CapHH() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

/**
 * Steps for testing if there are any objects to mantle:
 * 
 * 1. When the player tries to jump, trace forward to find any wall facing the player.
 * Both the speed of the player and the steepness of the wall are taken account of.
 * 2. In case a wall is found, it is checked if the top surface of the wall is higher than the maximum allowed height.
 * 3. It is checked if a capsule of the size of the player would overlap any objects in the landing 
 * position of the mantle.
 * 4. In the end, if the wall fits all criteria for mantling, the data is stored in CurrentMantle and the
 * next traversal mode is set to mantling.
 * 
 * Heavily based on this implementation: https://www.youtube.com/watch?v=3hk39el8dg8
 */
bool UCustomCharacterMovementComponent::TryMantle()
{
	if (CurrentTraversalMode == ETraversalMode::Crouch or CurrentTraversalMode == ETraversalMode::Slide)
	{
		return false;
	}

	FVector BaseLoc = UpdatedComponent->GetComponentLocation() + FVector::DownVector * CapHH();
	FVector Fwd = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
	auto Params = Cast<AGameCharacter>(CharacterOwner)->GetIgnoreCharacterParams();
	float MaxHeight = CapHH() * 2+ MantleReachHeight;
	float CosMMWSA = FMath::Cos(FMath::DegreesToRadians(MantleMinWallSteepnessAngle));
	float CosMMSA = FMath::Cos(FMath::DegreesToRadians(MantleMaxSurfaceAngle));
	float CosMMAA = FMath::Cos(FMath::DegreesToRadians(MantleMaxAlignmentAngle));
	
	// Ray trace for wall in front of the player
	FHitResult FrontHit;
	float CheckDistance = FMath::Clamp(Velocity | Fwd, CapR() + 30, MantleMaxDistance);
	FVector FrontStart = BaseLoc + FVector::UpVector * (MaxStepHeight - 1);

	for (int i = 0; i < 6; i++)
	{
		if (GetWorld()->LineTraceSingleByProfile(FrontHit, FrontStart, FrontStart + Fwd * CheckDistance,
			"BlockAll", Params))
		{
			break;
		}

		FrontStart += FVector::UpVector * (2.f * CapHH() - (MaxStepHeight - 1)) / 5;
	}

	if (!FrontHit.IsValidBlockingHit())
	{
		return false;
	}

	float CosWallSteepnessAngle = FrontHit.Normal | FVector::UpVector;
	if (FMath::Abs(CosWallSteepnessAngle) > CosMMWSA || (Fwd | -FrontHit.Normal) < CosMMAA)
	{
		return false;
	}

	// Check if the height of the wall that was found is suitable
	TArray<FHitResult> HeightHits;
	FHitResult SurfaceHit;
	FVector WallUp = FVector::VectorPlaneProject(FVector::UpVector, FrontHit.Normal).GetSafeNormal();
	float WallCos = FVector::UpVector | FrontHit.Normal;
	float WallSin = FMath::Sqrt(1 - WallCos * WallCos);
	FVector TraceStart = FrontHit.Location + Fwd + WallUp * (MaxHeight - (MaxStepHeight - 1)) / WallSin;

	if (!GetWorld()->LineTraceMultiByProfile(HeightHits, TraceStart, FrontHit.Location + Fwd,
		"BlockAll", Params))
	{
		return false;
	}

	for (const FHitResult& Hit : HeightHits)
	{
		if (Hit.IsValidBlockingHit())
		{
			SurfaceHit = Hit;
			break;
		}
	}

	if (!SurfaceHit.IsValidBlockingHit() || (SurfaceHit.Normal | FVector::UpVector) < CosMMSA)
	{
		return false;
	}

	float Height = (SurfaceHit.Location - BaseLoc) | FVector::UpVector;
	if (Height > MaxHeight)
	{
		return false;
	}

	// Check if there is enough space above the surface of the wall for the player capsule
	float SurfaceCos = FVector::UpVector | SurfaceHit.Normal;
	float SurfaceSin = FMath::Sqrt(1 - SurfaceCos * SurfaceCos);
	FVector ClearCapLoc = SurfaceHit.Location + Fwd * CapR() + FVector::UpVector * (CapHH() + 1 + CapR() * 2 * SurfaceSin);
	FCollisionShape CapShape = FCollisionShape::MakeCapsule(CapR(), CapHH());

	if (GetWorld()->OverlapAnyTestByProfile(ClearCapLoc, FQuat::Identity, "BlockAll", CapShape, Params))
	{
		return false;
	}

	// The player can mantle / vault the found wall based on it's height
	bool bMantle = false;
	if (Height > CapHH() * 2)
	{
		bMantle = true;
	}
	else if ((Velocity | FVector::UpVector) < 0)
	{
		if (!GetWorld()->OverlapAnyTestByProfile(ClearCapLoc, FQuat::Identity, "BlockAll",
			CapShape, Params))
		{
			bMantle = true;
		}
	}

	CurrentMantle.bIsVault = !bMantle;
	CurrentMantle.bInProgress = true;
	CurrentMantle.StartLocation = CharacterOwner->GetActorLocation();
	CurrentMantle.TargetLocation = ClearCapLoc;

	SetNextTraversalMode(ETraversalMode::Mantle);
	return true;
}

bool UCustomCharacterMovementComponent::CanMantle()
{
	if (CharacterOwner)
	{
		return TryMantle();
	}

	return false;
}

void UCustomCharacterMovementComponent::EnterMantle()
{
	MantleElapsedTime = 0.f;
	Velocity = FVector::ZeroVector;
	SetMovementMode(MOVE_Flying);
}

void UCustomCharacterMovementComponent::UpdateMantle(float DeltaTime)
{
	if (!CharacterOwner)
	{
		return;
	}

	MantleElapsedTime += DeltaTime;
	const float Duration = CurrentMantle.bIsVault ? VaultDuration : MantleDuration;
	const float Alpha = FMath::Clamp(MantleElapsedTime / Duration, 0.f, 1.f);

	FVector NewLocation = FMath::Lerp(CurrentMantle.StartLocation, CurrentMantle.TargetLocation, Alpha);
	NewLocation.Z += FMath::Sin(Alpha * PI) * MantleArc;

	CharacterOwner->SetActorLocation(NewLocation, false);

	if (Alpha >= 1.f)
	{
		SetMovementMode(MOVE_Walking);
		CurrentMantle = FMantleData();
		SetNextTraversalMode(ETraversalMode::Walk);
	}
}
