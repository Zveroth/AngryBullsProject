// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/WorkerPawn.h"
#include "Interaction/InteractionSphereComponent.h"
#include "Selection/SelectionComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Player/PlayerPawn.h"
#include "ColorScheme/ColorSchemeSettings.h"



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

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovement"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
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

}

void AWorkerPawn::OnSelectionStateChanged()
{
	RefreshMeshColor();
}

void AWorkerPawn::RefreshMeshColor()
{
	if (USelectionColorScheme* ColorScheme = UColorSchemeSettings::GetColorScheme())
	{
		m_WorkerMeshMat->SetVectorParameterValue(FName("Color"), ((Selection->IsSelected()) ? ColorScheme->SelectedUnit : ColorScheme->DeselectedUnit));
	}
}