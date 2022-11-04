// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SelectionColorScheme.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBULLSPROJECT_API USelectionColorScheme : public UDataAsset
{
	GENERATED_BODY()
	

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ColorScheme")
		FLinearColor SelectedUnit = FLinearColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ColorScheme")
		FLinearColor DeselectedUnit = FLinearColor::Blue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ColorScheme")
		FLinearColor SelectedBuilding = FLinearColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ColorScheme")
		FLinearColor DeselectedBuilding = FLinearColor::Blue;
};