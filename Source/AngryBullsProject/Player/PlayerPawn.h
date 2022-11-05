// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"



class USelectionComponent;
class UOrderBase;
class AOrderQueueAIController;

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

	void SendOrder(UOrderBase* OrderTemplate);
	void SendOrders(const TArray<UOrderBase*>& OrderTemplates);

	//Widget callback for priority confirmation
	UFUNCTION(BlueprintCallable, Category = "Order Handling")
		void ConfirmPriority(int32 Priority);

private:

	void Select();
	void Interact();

	void IncreaseCameraDesiredZoom();
	void DecreaseCameraDesiredZoom();

	void SetMovementInputX(float Direction);
	void SetMovementInputY(float Direction);

	void ProcessMovement(float DeltaTime);

	bool IsAnyShiftDown(APlayerController* PlayerController = nullptr) const;
	bool IsAnyControlDown(APlayerController* PlayerController = nullptr) const;

	AOrderQueueAIController* GetControllerFromSelection(uint32 Index) const;
	AOrderQueueAIController* GetControllerFromSelectionComponent(USelectionComponent* SelectionComp) const;

	void DisplayPriorityWidgetForOrders(const TArray<UOrderBase*>& Orders);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float ScrollSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Interface")
		TSubclassOf<UUserWidget> PrioritySelectionWidgetClass;

private:

	UPROPERTY(VisibleAnywhere)
		class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* Camera;

	UPROPERTY()
		TArray<USelectionComponent*> m_Selection;

	FVector2D m_MovementInput;

	UPROPERTY()
		TArray<UOrderBase*> m_PriorityPendingOrders;
	bool m_bSelectingPriority;
};
