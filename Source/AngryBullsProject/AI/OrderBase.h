// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OrderBase.generated.h"



enum class EOrderState
{
	Pending,
	Executing,
	Success,
	Failure,
	Aborted,
	Skipped
};

DECLARE_DELEGATE_TwoParams(FOrderFinished, UOrderBase* Order, bool bSuccess);

UCLASS()
class ANGRYBULLSPROJECT_API UOrderBase : public UObject
{
	GENERATED_BODY()
	

public:

	UOrderBase();

	void Execute();
	void Abort(bool bBroadcast = true);
	void Skip();

	EOrderState GetState() const { return OrderState; }
	bool IsInState(EOrderState State) const { return OrderState == State; }

	bool NeedsSuccessToStart() const { return m_bNeedsSuccessToStart; }

	class AOrderQueueAIController* GetOwningController() const;

	virtual UOrderBase* CreateCopy(UObject* Outer) const PURE_VIRTUAL(UORderBase::CreateCopy, return nullptr; );

	void SetNeedsSuccessToStart(bool bNeedsSuccessToStart);
	void SetRetryCount(int8 RetryCount);
	void SetRetryUntilSuccess();

protected:

	//Call finish only from Execute_Internal
	void Finish(bool bSuccess);

	virtual void Execute_Internal() PURE_VIRTUAL(UOrderBase::Execute_Internal, );
	virtual void Abort_Internal() PURE_VIRTUAL(UOrderBase::Abort_Internal, );

	UOrderBase* InitializeCopy(UOrderBase* Order) const;

public:

	FOrderFinished OnOrderFinished;

	int32 Priority;

protected:

	bool m_bNeedsSuccessToStart;
	//RetryCount == -1 will execute an order until it succeeds
	int8 m_RetryCount;

private:

	EOrderState OrderState;
};