// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"



class USelectionComponent;

UCLASS()
class ANGRYBULLSPROJECT_API APlayerPawn : public APawn
{
	GENERATED_BODY()


public:

	APlayerPawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	APlayerController* GetPlayerController() const;

	void AddToSelection(USelectionComponent* SelectionComp, bool bIgnoreModifierKeys = false);
	void RemoveFromSelection(USelectionComponent* SelectionComp);
	void ClearSelection();

private:

	void Select();
	void Interact();

	void IncreaseCameraDesiredZoom();
	void DecreaseCameraDesiredZoom();

	void SetMovementInputX(float Direction);
	void SetMovementInputY(float Direction);

	void ProcessMovement(float DeltaTime);

	void SendLocationOrder(const FVector& TargetLocation);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float ScrollSpeed;

private:

	UPROPERTY(VisibleAnywhere)
		class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* Camera;

	UPROPERTY()
		TArray<USelectionComponent*> m_Selection;

	FVector2D m_MovementInput;
};
