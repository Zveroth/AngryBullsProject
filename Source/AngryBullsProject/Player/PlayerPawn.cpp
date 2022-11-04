// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/PlayerPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Interaction/InteractionShapeComponent.h"
#include "Selection/SelectionComponent.h"
#include "AIController.h"



// Sets default values
APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	CameraArm->bEnableCameraLag = true;
	CameraArm->CameraLagSpeed = 30.0f;
	CameraArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	Camera->SetOrthoWidth(4096.0f);

	ScrollSpeed = 1000.0f;
	m_MovementInput = FVector2D::ZeroVector;
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ProcessMovement(DeltaTime);
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("CameraZoomIn"), EInputEvent::IE_Pressed, this, &APlayerPawn::IncreaseCameraDesiredZoom);
	PlayerInputComponent->BindAction(FName("CameraZoomOut"), EInputEvent::IE_Pressed, this, &APlayerPawn::DecreaseCameraDesiredZoom);

	PlayerInputComponent->BindAction(FName("Select"), EInputEvent::IE_Pressed, this, &APlayerPawn::Select);
	PlayerInputComponent->BindAction(FName("Interact"), EInputEvent::IE_Pressed, this, &APlayerPawn::Interact);

	PlayerInputComponent->BindAxis(FName("MoveX"), this, &APlayerPawn::SetMovementInputX);
	PlayerInputComponent->BindAxis(FName("MoveY"), this, &APlayerPawn::SetMovementInputY);
}

void APlayerPawn::IncreaseCameraDesiredZoom()
{
	
}

void APlayerPawn::DecreaseCameraDesiredZoom()
{
	
}

void APlayerPawn::SetMovementInputX(float Direction)
{
	m_MovementInput.X = Direction;
}

void APlayerPawn::SetMovementInputY(float Direction)
{
	m_MovementInput.Y = Direction;
}

void APlayerPawn::ProcessMovement(float DeltaTime)
{
	if (!m_MovementInput.IsZero())
	{
		const FVector MovementInputNormalized = FVector(m_MovementInput.GetSafeNormal(), 0.0f);
		const FVector FrameMovement = MovementInputNormalized * DeltaTime * ScrollSpeed;
		AddActorLocalOffset(FrameMovement);
	}
}

void APlayerPawn::Select()
{
	FHitResult HitResult;
	const APlayerController* PlayerController = GetPlayerController();
	PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, false, HitResult);

	if (UInteractionShapeComponent* InteractionComp = Cast<UInteractionShapeComponent>(HitResult.GetComponent()))
	{
		if (PlayerController->IsInputKeyDown(EKeys::LeftControl) || PlayerController->IsInputKeyDown(EKeys::RightControl))
		{
			InteractionComp->Deselect(this);
		}
		else
		{
			InteractionComp->Select(this);
		}
	}
	else if (!PlayerController->IsInputKeyDown(EKeys::LeftShift) && !PlayerController->IsInputKeyDown(EKeys::RightShift))
	{
		ClearSelection();
	}
}

void APlayerPawn::Interact()
{
	FHitResult HitResult;
	const APlayerController* PlayerController = GetPlayerController();
	PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, false, HitResult);

	if (UInteractionShapeComponent* InteractionComp = Cast<UInteractionShapeComponent>(HitResult.GetComponent()))
	{
		InteractionComp->Interact(this);
	}
	else if(HitResult.bBlockingHit)
	{
		SendLocationOrder(HitResult.Location);
	}
}

APlayerController* APlayerPawn::GetPlayerController() const
{
	return Cast<APlayerController>(GetController());
}

void APlayerPawn::AddToSelection(USelectionComponent* SelectionComp, bool bIgnoreModifierKeys)
{
	ensureMsgf(IsValid(SelectionComp), TEXT("Invalid selection component added to selection!"));

	const APlayerController* PlayerController = GetPlayerController();
	const bool bClearSelection = bIgnoreModifierKeys || (!PlayerController->IsInputKeyDown(EKeys::LeftShift) && !PlayerController->IsInputKeyDown(EKeys::RightShift));

	if (!m_Selection.Contains(SelectionComp))
	{
		if (bClearSelection)
		{
			ClearSelection();
		}

		m_Selection.Add(SelectionComp);
		SelectionComp->SetSelectionState(true);
	}
	else if (bClearSelection && m_Selection.Num() > 1)
	{
		m_Selection.RemoveSingle(SelectionComp);

		ClearSelection();

		m_Selection.Add(SelectionComp);
	}
}

void APlayerPawn::RemoveFromSelection(USelectionComponent* SelectionComp)
{
	if (m_Selection.RemoveSingle(SelectionComp) != 0)
	{
		if (IsValid(SelectionComp))
		{
			SelectionComp->SetSelectionState(false);
		}
	}
}

void APlayerPawn::ClearSelection()
{
	for (USelectionComponent* SelectionComp : m_Selection)
	{
		if (IsValid(SelectionComp))
		{
			SelectionComp->SetSelectionState(false);
		}
	}

	m_Selection.Empty();
}

void APlayerPawn::SendLocationOrder(const FVector& TargetLocation)
{
	for (USelectionComponent* SelectionComp : m_Selection)
	{
		if (IsValid(SelectionComp))
		{
			APawn* SelectedPawn = SelectionComp->GetOwner<APawn>();
			if (!IsValid(SelectedPawn))
			{
				continue;
			}

			AAIController* AIController = Cast<AAIController>(SelectedPawn->GetController());
			if (IsValid(AIController))
			{
				AIController->MoveToLocation(TargetLocation);
			}
		}
	}
}