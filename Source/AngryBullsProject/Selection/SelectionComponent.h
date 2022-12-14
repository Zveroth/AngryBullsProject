// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SelectionComponent.generated.h"



DECLARE_DELEGATE(FOnSelectionStateChanged);

//Component used distinguish actors that can be selected.
UCLASS( ClassGroup=(Selection), meta=(BlueprintSpawnableComponent) )
class ANGRYBULLSPROJECT_API USelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USelectionComponent();

	void SetSelectionState(bool SelectionState);
	bool IsSelected() const { return m_bSelectionState; }

public:

	FOnSelectionStateChanged OnSelectionStateChanged;

private:

	bool m_bSelectionState;
};