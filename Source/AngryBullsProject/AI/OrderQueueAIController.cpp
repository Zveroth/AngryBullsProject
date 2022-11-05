// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/OrderQueueAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "AI/OrderBase.h"



AOrderQueueAIController::AOrderQueueAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}

void AOrderQueueAIController::AddOrder(UOrderBase* Order, bool bClearQueue)
{
	if (Order->GetOuter() != this)
	{
		Order->Rename(nullptr, this);
	}

	if (bClearQueue)
	{
		ClearQueue();
	}

	if (IsValid(Order))
	{
		Order->OnOrderFinished.BindUObject(this, &AOrderQueueAIController::OnOrderFinished);

		int32 At = 0;
		for (int32 I = m_OrderQueue.Num() - 1; I >= 0; --I)
		{
			if (m_OrderQueue[I]->Priority >= Order->Priority)
			{
				At = I + 1;
				break;
			}
		}
		m_OrderQueue.Insert(Order, At);


		if (!IsExecuting())
		{
			ExecuteNextOrder(true);
		}
	}
}

void AOrderQueueAIController::AddOrderWaiting(UOrderBase* Order)
{
	if (Order->GetOuter() != this)
	{
		Order->Rename(nullptr, this);
	}

	if (IsValid(Order))
	{
		Order->OnOrderFinished.BindUObject(this, &AOrderQueueAIController::OnOrderFinished);

		int32 At = 0;
		for (int32 I = m_OrderQueue.Num() - 1; I >= 0; --I)
		{
			if (m_OrderQueue[I]->Priority >= Order->Priority)
			{
				At = I + 1;
				break;
			}
		}
		m_OrderQueue.Insert(Order, At);
	}
}

void AOrderQueueAIController::FlushOrders()
{
	if (!m_OrderQueue.IsValidIndex(0) || IsExecuting())
	{
		return;
	}

	ExecuteNextOrder(true);
}

void AOrderQueueAIController::OnOrderFinished(UOrderBase* Order, bool bSuccess)
{
	ensureMsgf(m_ExecutedOrder == Order, TEXT("Outside order finished!"));
	
	m_ExecutedOrder = nullptr;

	ExecuteNextOrder(bSuccess);
}

void AOrderQueueAIController::ExecuteNextOrder(bool bWasSuccess)
{
	if (m_OrderQueue.IsValidIndex(0))
	{
		m_ExecutedOrder = m_OrderQueue[0];
		m_OrderQueue.RemoveAt(0);

		if (m_ExecutedOrder->NeedsSuccessToStart() && !bWasSuccess)
		{
			m_ExecutedOrder->Skip();
			return;
		}

		m_ExecutedOrder->Execute();
	}
}

void AOrderQueueAIController::ClearQueue()
{
	if (IsExecuting())
	{
		m_ExecutedOrder->Abort();
		m_ExecutedOrder = nullptr;
	}

	m_OrderQueue.Empty();
}

bool AOrderQueueAIController::IsExecuting() const
{
	return IsValid(m_ExecutedOrder);
}