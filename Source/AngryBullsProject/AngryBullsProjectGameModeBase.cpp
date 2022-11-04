// Copyright Epic Games, Inc. All Rights Reserved.
#include "AngryBullsProjectGameModeBase.h"
#include "Player/PlayerPawn.h"
#include "Player/PlayerControllerBase.h"



AAngryBullsProjectGameModeBase::AAngryBullsProjectGameModeBase()
{
	DefaultPawnClass = APlayerPawn::StaticClass();
	PlayerControllerClass = APlayerController::StaticClass();
}