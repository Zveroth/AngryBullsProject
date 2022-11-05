// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OrderQueueAIController.generated.h"



class UOrderBase;

UCLASS()
class ANGRYBULLSPROJECT_API AOrderQueueAIController : public AAIController
{
	GENERATED_BODY()
	

public:

	AOrderQueueAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void AddOrder(UOrderBase* Order, bool bClearQueue);
	void AddOrderWaiting(UOrderBase* Order);
	void FlushOrders();

	void ClearQueue();

	bool IsExecuting() const;

private:

	void OnOrderFinished(UOrderBase* Order, bool bSuccess);

	void ExecuteNextOrder(bool bWasSuccess);

private:

	//Orders should be stored in clusters so they can be *stuck* together
	UPROPERTY()
		TArray<UOrderBase*> m_OrderQueue;

	UPROPERTY()
		UOrderBase* m_ExecutedOrder;
};
