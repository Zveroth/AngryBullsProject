// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingSystem/BuildingBase.h"
#include "BuildingDisco.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBULLSPROJECT_API ABuildingDisco : public ABuildingBase
{
	GENERATED_BODY()
	

public:

	ABuildingDisco();

private:

	void OnInteract(class APlayerPawn* PlayerPawn);

private:

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* BuildingMesh;

	UPROPERTY(VisibleAnywhere)
		class UInteractionSphereComponent* InteractionSphere;
};