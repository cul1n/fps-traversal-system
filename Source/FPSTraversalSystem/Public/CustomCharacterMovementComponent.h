// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TraversalMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

/**
 * Custom Traversal System built on top of the Character Movement Component
 *
 * To add a custom movement mode:
 * 1. Update the ETraversalMode enum
 * 2. Handle input in the Game Character class.
 * 3. Handle logic in the UpdateTraversalMode method for interactions between movement modes
 * 4. For complex movements, implement two new methods for entry and update (e.g. EnterSlide, UpdateMantle)
 */
UCLASS()
class FPSTRAVERSALSYSTEM_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	ETraversalMode CurrentTraversalMode = ETraversalMode::Walk;
	ETraversalMode NextTraversalMode = ETraversalMode::Walk;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement")
	TMap<ETraversalMode, FTraversalParams> TraversalParams;

	// Stamina properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float CurrentStamina = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRate = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaThreshold = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaFatigueRate = 10.0f;

	// Slide properties
	float SlideElapsedTime = 0.f;
	FVector SlideDirection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slide")
	float SlideDeceleration = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slide")
	float MinSlideSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slide")
	float MaxSlideDuration = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slide")
	float MinSlideStartSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slide")
	float MaxSlideStartSpeed = 800.f;
	
	// Mantle properties
	FMantleData CurrentMantle;
	float MantleElapsedTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mantle")
	float MantleMaxDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mantle")
	float MantleReachHeight = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mantle")
	float MantleMinWallSteepnessAngle = 75.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mantle")
	float MantleMaxSurfaceAngle = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mantle")
	float MantleMaxAlignmentAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mantle")
	float MantleArc = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mantle")
	float MantleDuration = 2.9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mantle")
	float VaultDuration = 1.6f;

public:
	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	ETraversalMode GetCurrentTraversalMode() const;

	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	void SetCurrentTraversalMode(ETraversalMode NewTraversalMode);

	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	void SetNextTraversalMode(ETraversalMode NewTraversalMode);

	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	bool IsSliding() const;

	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	bool IsMantling() const;

	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	bool IsVaulting() const;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetStaminaNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetStaminaThresholdNormalized() const;

	void UpdateTraversalMode();

	bool ApplyTraversalParams(ETraversalMode TraversalMode);

	void UpdateStamina(float DeltaTime);

	virtual void InitializeComponent() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RequestCrouch();
	void EnterSlide();
	void UpdateSlide(float DeltaTime);

	bool TryMantle();
	bool CanMantle();
	void EnterMantle();
	void UpdateMantle(float DeltaTime);

	float CapR() const;
	float CapHH() const;
};
