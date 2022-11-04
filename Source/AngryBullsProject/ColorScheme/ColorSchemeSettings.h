// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ColorScheme/SelectionColorScheme.h"
#include "ColorSchemeSettings.generated.h"

//We can use GameUserSettings if we want to make this player configurable
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Color Scheme Settings"))
class ANGRYBULLSPROJECT_API UColorSchemeSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	static USelectionColorScheme* GetColorScheme();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Color Scheme")
		TSoftObjectPtr<USelectionColorScheme> SelectionColorScheme;
};