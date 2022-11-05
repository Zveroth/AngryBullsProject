// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/Orders/OrderMoveToLocation.h"



UOrderMoveToLocation* UOrderMoveToLocation::Create(UObject* Outer, const FVector& TargetLocation)
{
	UOrderMoveToLocation* Order = NewObject<UOrderMoveToLocation>(Outer);
	Order->m_TargetLocation = TargetLocation;

	return Order;
}

void UOrderMoveToLocation::Execute_Internal()
{
	AOrderQueueAIController* OwningController = GetOwningController();
	OwningController->ReceiveMoveCompleted.AddDynamic(this, &UOrderMoveToLocation::OnMoveCompleted);
	OwningController->MoveToLocation(m_TargetLocation);
}

void UOrderMoveToLocation::Abort_Internal()
{
	AOrderQueueAIController* OwningController = GetOwningController();
	OwningController->ReceiveMoveCompleted.RemoveDynamic(this, &UOrderMoveToLocation::OnMoveCompleted);
	OwningController->StopMovement();
}

void UOrderMoveToLocation::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	GetOwningController()->ReceiveMoveCompleted.RemoveDynamic(this, &UOrderMoveToLocation::OnMoveCompleted);
	Finish(Result == EPathFollowingResult::Success);
}

UOrderBase* UOrderMoveToLocation::CreateCopy(UObject* Outer) const
{
	return InitializeCopy(UOrderMoveToLocation::Create(Outer, m_TargetLocation));
}