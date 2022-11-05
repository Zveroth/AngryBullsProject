// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/OrderBase.h"
#include "OrderDance.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBULLSPROJECT_API UOrderDance : public UOrderBase
{
	GENERATED_BODY()
	

public:

	static UOrderDance* Create(UObject* Outer, float Duration);

	virtual UOrderBase* CreateCopy(UObject* Outer) const override;

protected:

	virtual void Execute_Internal() override;
	virtual void Abort_Internal() override;

private:

	void TimerTimeout();

private:

	float m_Duration;
	FTimerHandle m_DanceTimer;
};