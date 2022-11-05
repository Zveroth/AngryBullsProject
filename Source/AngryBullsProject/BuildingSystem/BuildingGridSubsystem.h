// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BuildingSystem/BuildingBase.h"
#include "BuildingGridSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBULLSPROJECT_API UBuildingGridSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	

public:

	UBuildingGridSubsystem();

	void RegisterBuilding(ABuildingBase* Building);

	FVector ClampLocationToGrid(const FVector& Location) const;

private:

	UPROPERTY()
		TArray<ABuildingBase*> m_Buildings;

	float m_GridSpacing;
};