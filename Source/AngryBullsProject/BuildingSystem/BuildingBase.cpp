// Fill out your copyright notice in the Description page of Project Settings.
#include "BuildingSystem/BuildingBase.h"
#include "BuildingSystem/BuildingGridSubsystem.h"



// Sets default values
ABuildingBase::ABuildingBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

#if WITH_EDITOR
void ABuildingBase::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	if (bFinished)
	{
		UBuildingGridSubsystem* BuildingGrid = GetWorld()->GetSubsystem<UBuildingGridSubsystem>();
		if (IsValid(BuildingGrid))
		{
			SetActorLocation(BuildingGrid->ClampLocationToGrid(GetActorLocation()));
		}
	}
}
#endif // WITH_EDITOR

// Called when the game starts or when spawned
void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();
	
	UBuildingGridSubsystem* BuildingGrid = GetWorld()->GetSubsystem<UBuildingGridSubsystem>();
	BuildingGrid->RegisterBuilding(this);
}