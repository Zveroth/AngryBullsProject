// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/Orders/OrderMoveToActor.h"



UOrderMoveToActor* UOrderMoveToActor::Create(UObject* Outer, AActor* TargetActor)
{
	UOrderMoveToActor* Order = NewObject<UOrderMoveToActor>(Outer);
	Order->m_TargetActor = TargetActor;

	return Order;
}

void UOrderMoveToActor::Execute_Internal()
{
	AOrderQueueAIController* OwningController = GetOwningController();
	OwningController->ReceiveMoveCompleted.AddDynamic(this, &UOrderMoveToActor::OnMoveCompleted);
	OwningController->MoveToActor(m_TargetActor);
}

void UOrderMoveToActor::Abort_Internal()
{
	AOrderQueueAIController* OwningController = GetOwningController();
	OwningController->ReceiveMoveCompleted.RemoveDynamic(this, &UOrderMoveToActor::OnMoveCompleted);
	OwningController->StopMovement();
}

void UOrderMoveToActor::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	GetOwningController()->ReceiveMoveCompleted.RemoveDynamic(this, &UOrderMoveToActor::OnMoveCompleted);
	Finish(Result == EPathFollowingResult::Success);
}

UOrderBase* UOrderMoveToActor::CreateCopy(UObject* Outer) const
{
	return InitializeCopy(UOrderMoveToActor::Create(Outer, m_TargetActor));
}