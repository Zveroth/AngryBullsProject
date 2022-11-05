// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/Orders/OrderDance.h"
#include "AI/OrderQueueAIController.h"
#include "AI/WorkerPawn.h"



UOrderDance* UOrderDance::Create(UObject* Outer, float Duration)
{
	UOrderDance* Order = NewObject<UOrderDance>(Outer);
	Order->m_Duration = Duration;
	return Order;
}

void UOrderDance::Execute_Internal()
{
	if (AWorkerPawn* WorkerPawn = GetOwningController()->GetPawn<AWorkerPawn>())
	{
		WorkerPawn->SetDancing(true);

		GetWorld()->GetTimerManager().SetTimer(m_DanceTimer, this, &UOrderDance::TimerTimeout, m_Duration);
	}
	else
	{
		Finish(false);
	}
}

void UOrderDance::Abort_Internal()
{
	if (AWorkerPawn* WorkerPawn = GetOwningController()->GetPawn<AWorkerPawn>())
	{
		WorkerPawn->SetDancing(false);
	}

	GetWorld()->GetTimerManager().ClearTimer(m_DanceTimer);
}

UOrderBase* UOrderDance::CreateCopy(UObject* Outer) const
{
	return InitializeCopy(UOrderDance::Create(Outer, m_Duration));
}

void UOrderDance::TimerTimeout()
{
	if (AWorkerPawn* WorkerPawn = GetOwningController()->GetPawn<AWorkerPawn>())
	{
		WorkerPawn->SetDancing(false);
	}

	Finish(true);
}