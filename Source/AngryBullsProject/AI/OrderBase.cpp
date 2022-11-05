// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/OrderBase.h"
#include "AI/OrderQueueAIController.h"



UOrderBase::UOrderBase()
{
	OrderState = EOrderState::Pending;
	m_bNeedsSuccessToStart = false;
	m_RetryCount = 0;
	Priority = 0;
}

void UOrderBase::Execute()
{
	if (IsInState(EOrderState::Pending))
	{
		OrderState = EOrderState::Executing;

		Execute_Internal();
	}
}

void UOrderBase::Abort(bool bBroadcast)
{
	if (IsInState(EOrderState::Executing))
	{
		OrderState = EOrderState::Aborted;

		Abort_Internal();

		if (bBroadcast)
		{
			OnOrderFinished.ExecuteIfBound(this, false);
		}
	}
}

void UOrderBase::Skip()
{
	ensureMsgf(IsInState(EOrderState::Pending), TEXT("Tried to skip order that has already begun/finished!"));

	OrderState = EOrderState::Skipped;

	OnOrderFinished.ExecuteIfBound(this, false);
}

void UOrderBase::Finish(bool bSuccess)
{
	ensureMsgf(IsInState(EOrderState::Executing), TEXT("Finish called on an order that wasn't executing!"));

	if (!bSuccess)
	{
		switch (m_RetryCount)
		{
		default: --m_RetryCount;
		case -1: Execute_Internal();
			return;
		case 0: 
			break;
		}
	}

	OrderState = (bSuccess) ? EOrderState::Success : EOrderState::Failure;
	OnOrderFinished.ExecuteIfBound(this, bSuccess);
}

AOrderQueueAIController* UOrderBase::GetOwningController() const
{
	return Cast<AOrderQueueAIController>(GetOuter());
}

UOrderBase* UOrderBase::InitializeCopy(UOrderBase* Order) const
{
	Order->m_bNeedsSuccessToStart = m_bNeedsSuccessToStart;
	Order->m_RetryCount = m_RetryCount;
	Order->Priority = Priority;
	return Order;
}

void UOrderBase::SetNeedsSuccessToStart(bool bNeedsSuccessToStart)
{
	ensureMsgf(IsInState(EOrderState::Pending), TEXT("Tried setting NeedsSuccessToStart after an order has already started/finished!"));

	m_bNeedsSuccessToStart = bNeedsSuccessToStart;
}

void UOrderBase::SetRetryCount(int8 RetryCount)
{
	ensureMsgf(IsInState(EOrderState::Pending), TEXT("Tried setting SetRestryCount after an order has already started/finished!"));

	m_RetryCount = RetryCount;
}

void UOrderBase::SetRetryUntilSuccess()
{
	ensureMsgf(IsInState(EOrderState::Pending), TEXT("Tried setting SetRetryUntilSuccess after an order has already started/finished!"));

	m_RetryCount = -1;
}