// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingBase.generated.h"

UCLASS()
class ANGRYBULLSPROJECT_API ABuildingBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingBase();

#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished);
#endif // WITH_EDITOR

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
