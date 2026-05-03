// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TraversalMode.generated.h"

/**
 * TODO: add description for adding new movement modes
 */

UENUM(BlueprintType)
enum class ETraversalMode : uint8
{
	Walk,
	Sprint,
	Crouch,
	Slide
};

USTRUCT(BlueprintType)
struct FTraversalParams
{
	GENERATED_BODY()

public:
	// TODO: define real params
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxWalkSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAcceleration = 2048.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float GroundFriction = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float BrakingDecelerationWalking = 2048.f;
};
