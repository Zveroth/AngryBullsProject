// Fill out your copyright notice in the Description page of Project Settings.


#include "Selection/SelectionComponent.h"

// Sets default values for this component's properties
USelectionComponent::USelectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	m_bSelectionState = false;
}

void USelectionComponent::SetSelectionState(bool SelectionState)
{
	m_bSelectionState = SelectionState;
	OnSelectionStateChanged.ExecuteIfBound();
}