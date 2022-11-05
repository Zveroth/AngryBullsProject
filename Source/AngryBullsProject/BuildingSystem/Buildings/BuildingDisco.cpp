// Fill out your copyright notice in the Description page of Project Settings.
#include "BuildingSystem/Buildings/BuildingDisco.h"
#include "Interaction/InteractionSphereComponent.h"
#include "Player/PlayerPawn.h"
#include "AI/Orders/OrderMoveToActor.h"
#include "AI/Orders/OrderDance.h"



ABuildingDisco::ABuildingDisco()
{
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
	BuildingMesh->SetupAttachment(RootComponent);
	BuildingMesh->SetCollisionProfileName(FName("BlockAll"));
	BuildingMesh->SetCanEverAffectNavigation(true);

	InteractionSphere = CreateDefaultSubobject<UInteractionSphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->OnInteract.BindUObject(this, &ABuildingDisco::OnInteract);
}

void ABuildingDisco::OnInteract(APlayerPawn* PlayerPawn)
{
	UOrderMoveToActor* MoveOrder = UOrderMoveToActor::Create(PlayerPawn, this);
	MoveOrder->SetRetryUntilSuccess();

	UOrderDance* DanceOrder = UOrderDance::Create(PlayerPawn, 5.0f);
	DanceOrder->SetNeedsSuccessToStart(true);

	PlayerPawn->SendOrders({ MoveOrder, DanceOrder });
}