// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WorkerPawn.generated.h"



class APlayerPawn;

UCLASS()
class ANGRYBULLSPROJECT_API AWorkerPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AWorkerPawn();

protected:

	virtual void BeginPlay() override;

private:

	void OnSelect(APlayerPawn* PlayerPawn);
	void OnDeselect(APlayerPawn* PlayerPawn);
	void OnInteract(APlayerPawn* PlayerPawn);

	void OnSelectionStateChanged();

	void RefreshMeshColor();

private:

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* WorkerMesh;

	UPROPERTY(VisibleAnywhere)
		class UInteractionSphereComponent* InteractionSphere;

	UPROPERTY(VisibleAnywhere)
		class USelectionComponent* Selection;

	UPROPERTY(VisibleAnywhere)
		class UFloatingPawnMovement* PawnMovement;

	UPROPERTY()
		UMaterialInstanceDynamic* m_WorkerMeshMat;
};