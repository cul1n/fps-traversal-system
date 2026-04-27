// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacter.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "CustomCharacterMovementComponent.h"

AGameCharacter::AGameCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	CustomMovementComponent = Cast<UCustomCharacterMovementComponent>(GetCharacterMovement());
}

// Called when the game starts or when spawned
void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameCharacter::HandleMoveInput);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameCharacter::HandleLookInput);
	}

}

void AGameCharacter::HandleMoveInput(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	
	if (!Controller)
	{
		return;
	}
	
	const FRotator Rotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Forward = Rotation.RotateVector(FVector::ForwardVector);
	const FVector Right = Rotation.RotateVector(FVector::RightVector);
	
	AddMovementInput(Forward, Input.Y);
	AddMovementInput(Right, Input.X);
}

void AGameCharacter::HandleLookInput(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();

	if (!Controller)
	{
		return;
	}

	AddControllerYawInput(Input.X);
	AddControllerPitchInput(Input.Y);
}
