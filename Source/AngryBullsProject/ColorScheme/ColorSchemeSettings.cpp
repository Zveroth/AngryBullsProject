// Fill out your copyright notice in the Description page of Project Settings.
#include "ColorScheme/ColorSchemeSettings.h"



USelectionColorScheme* UColorSchemeSettings::GetColorScheme()
{
	UColorSchemeSettings* Settings = (UColorSchemeSettings*)UColorSchemeSettings::StaticClass()->GetDefaultObject();
	if (IsValid(Settings))
	{
		return Settings->SelectionColorScheme.LoadSynchronous();
	}

	return nullptr;
}