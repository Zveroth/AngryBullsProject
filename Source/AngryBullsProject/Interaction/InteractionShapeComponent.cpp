// Fill out your copyright notice in the Description page of Project Settings.
#include "Interaction/InteractionShapeComponent.h"



UInteractionShapeComponent::UInteractionShapeComponent()
{
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	SetCanEverAffectNavigation(false);

	m_bSelectionEnabled = true;
	m_bInteractionEnabled = true;
}

UInteractionShapeComponent::UInteractionShapeComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	m_bSelectionEnabled = true;
	m_bInteractionEnabled = true;
}

void UInteractionShapeComponent::Select(APlayerPawn* PlayerPawn)
{
	if (CanSelect(PlayerPawn))
	{
		HandleSelect(PlayerPawn);
	}
}

void UInteractionShapeComponent::Deselect(APlayerPawn* PlayerPawn)
{
	OnDeselect.ExecuteIfBound(PlayerPawn);
}

void UInteractionShapeComponent::Interact(APlayerPawn* PlayerPawn)
{
	if (CanInteract(PlayerPawn))
	{
		HandleInteract(PlayerPawn);
	}
}

void UInteractionShapeComponent::HandleSelect(APlayerPawn* PlayerPawn)
{
	OnSelect.ExecuteIfBound(PlayerPawn);
}

void UInteractionShapeComponent::HandleInteract(APlayerPawn* PlayerPawn)
{
	OnInteract.ExecuteIfBound(PlayerPawn);
}