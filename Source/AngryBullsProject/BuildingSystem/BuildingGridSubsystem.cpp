// Fill out your copyright notice in the Description page of Project Settings.
#include "BuildingSystem/BuildingGridSubsystem.h"



UBuildingGridSubsystem::UBuildingGridSubsystem()
{
	m_GridSpacing = 100.0f;
}

void UBuildingGridSubsystem::RegisterBuilding(ABuildingBase* Building)
{
	ensureMsgf(!m_Buildings.Contains(Building), TEXT("Registering already registered building!"));

	m_Buildings.Add(Building);
}

FVector UBuildingGridSubsystem::ClampLocationToGrid(const FVector& Location) const
{
	const FVector ClampedLocation(
		Location.X - FMath::Fmod(Location.X, m_GridSpacing),
		Location.Y - FMath::Fmod(Location.Y, m_GridSpacing),
		0.0f
	);

	return ClampedLocation;
}