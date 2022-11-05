// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/OrderBase.h"
#include "AI/OrderQueueAIController.h"
#include "OrderMoveToLocation.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBULLSPROJECT_API UOrderMoveToLocation : public UOrderBase
{
	GENERATED_BODY()


public:

	static UOrderMoveToLocation* Create(UObject* Outer, const FVector& TargetLocation);

	virtual UOrderBase* CreateCopy(UObject* Outer) const override;

protected:

	virtual void Execute_Internal() override;
	virtual void Abort_Internal() override;

private:

	UFUNCTION()
		void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

private:

	FVector m_TargetLocation;
};
