// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ShapeComponent.h"
#include "InteractionShapeComponent.generated.h"


class APlayerPawn;

DECLARE_DELEGATE_OneParam(FInteractionEvent, APlayerPawn* PlayerPawn);

UCLASS(ClassGroup = (Interaction))
class ANGRYBULLSPROJECT_API UInteractionShapeComponent : public UShapeComponent
{
	GENERATED_BODY()
	

public:

	UInteractionShapeComponent();
	UInteractionShapeComponent(const FObjectInitializer& ObjectInitializer);

	void Select(APlayerPawn* PlayerPawn);
	void Deselect(APlayerPawn* PlayerPawn);
	void Interact(APlayerPawn* PlayerPawn);

	bool CanSelect(APlayerPawn* PlayerPawn) { return m_bSelectionEnabled; }
	bool CanInteract(APlayerPawn* PlayerPawn) { return m_bInteractionEnabled; }

protected:

	void HandleSelect(APlayerPawn* PlayerPawn);
	void HandleInteract(APlayerPawn* PlayerPawn);

public:

	FInteractionEvent OnSelect;
	FInteractionEvent OnDeselect;
	FInteractionEvent OnInteract;

private:

	bool m_bSelectionEnabled;
	bool m_bInteractionEnabled;
};
