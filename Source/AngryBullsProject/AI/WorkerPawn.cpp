// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/WorkerPawn.h"
#include "Interaction/InteractionSphereComponent.h"
#include "Selection/SelectionComponent.h"
#include "Player/PlayerPawn.h"
#include "ColorScheme/ColorSchemeSettings.h"
#include "AI/Orders/OrderMoveToActor.h"



// Sets default values
AWorkerPawn::AWorkerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	WorkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorkerMesh"));
	WorkerMesh->SetupAttachment(RootComponent);
	WorkerMesh->SetCanEverAffectNavigation(false);

	InteractionSphere = CreateDefaultSubobject<UInteractionSphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->OnSelect.BindUObject(this, &AWorkerPawn::OnSelect);
	InteractionSphere->OnDeselect.BindUObject(this, &AWorkerPawn::OnDeselect);
	InteractionSphere->OnInteract.BindUObject(this, &AWorkerPawn::OnInteract);

	Selection = CreateDefaultSubobject<USelectionComponent>(TEXT("Selection"));
	Selection->OnSelectionStateChanged.BindUObject(this, &AWorkerPawn::OnSelectionStateChanged);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	m_bDancing = false;
}

void AWorkerPawn::BeginPlay()
{
	Super::BeginPlay();

	m_WorkerMeshMat = WorkerMesh->CreateDynamicMaterialInstance(0);
	ensureMsgf(IsValid(m_WorkerMeshMat), TEXT("Failed to create dynamic material instance! | %s"), *GetName());

	RefreshMeshColor();
}

void AWorkerPawn::OnSelect(APlayerPawn* PlayerPawn)
{
	if (IsValid(PlayerPawn))
	{
		PlayerPawn->AddToSelection(Selection);
	}
}

void AWorkerPawn::OnDeselect(APlayerPawn* PlayerPawn)
{
	if (IsValid(PlayerPawn))
	{
		PlayerPawn->RemoveFromSelection(Selection);
	}
}

void AWorkerPawn::OnInteract(APlayerPawn* PlayerPawn)
{
	PlayerPawn->SendOrder(UOrderMoveToActor::Create(PlayerPawn, this));
}

void AWorkerPawn::OnSelectionStateChanged()
{
	RefreshMeshColor();
}

void AWorkerPawn::RefreshMeshColor()
{
	FLinearColor FinalColor;
	if (m_bDancing)
	{
		FinalColor.R = FMath::FRandRange(0.1f, 1.0f);
		FinalColor.G = FMath::FRandRange(0.1f, 1.0f);
		FinalColor.B = FMath::FRandRange(0.1f, 1.0f);
		FinalColor.A = 1.0f;
	}
	else if (USelectionColorScheme* ColorScheme = UColorSchemeSettings::GetColorScheme())
	{
		FinalColor = (Selection->IsSelected()) ? ColorScheme->SelectedUnit : ColorScheme->DeselectedUnit;
	}

	m_WorkerMeshMat->SetVectorParameterValue(FName("Color"), FinalColor);
}

void AWorkerPawn::SetDancing(bool bDancing)
{
	m_bDancing = bDancing;

	RefreshMeshColor();

	if (m_bDancing)
	{
		GetWorldTimerManager().SetTimer(m_DanceTimer, this, &AWorkerPawn::RefreshMeshColor, 0.25f, true);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(m_DanceTimer);
	}
}