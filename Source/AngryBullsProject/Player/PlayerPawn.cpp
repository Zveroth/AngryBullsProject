// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/PlayerPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Interaction/InteractionShapeComponent.h"
#include "Selection/SelectionComponent.h"
#include "AI/OrderQueueAIController.h"
#include "AI/Orders/OrderMoveToLocation.h"
#include "Blueprint/UserWidget.h"



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
	Camera->SetOrthoWidth(2048.0f);

	ScrollSpeed = 1000.0f;
	m_MovementInput = FVector2D::ZeroVector;
	m_bSelectingPriority = false;
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
	//Block input when selecting priority
	if (m_bSelectingPriority)
	{
		return;
	}

	FHitResult HitResult;
	APlayerController* PlayerController = GetPlayerController();
	PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, false, HitResult);

	if (UInteractionShapeComponent* InteractionComp = Cast<UInteractionShapeComponent>(HitResult.GetComponent()))
	{
		if (IsAnyControlDown(PlayerController))
		{
			InteractionComp->Deselect(this);
		}
		else
		{
			InteractionComp->Select(this);
		}
	}
	else if (!IsAnyShiftDown(PlayerController))
	{
		ClearSelection();
	}
}

void APlayerPawn::Interact()
{
	//Block input when selecting priority
	if (m_bSelectingPriority)
	{
		return;
	}

	FHitResult HitResult;
	APlayerController* PlayerController = GetPlayerController();
	PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, false, HitResult);

	if (UInteractionShapeComponent* InteractionComp = Cast<UInteractionShapeComponent>(HitResult.GetComponent()))
	{
		InteractionComp->Interact(this);
	}
	else if(HitResult.bBlockingHit)
	{
		SendOrder(UOrderMoveToLocation::Create(this, HitResult.Location));
	}
}

APlayerController* APlayerPawn::GetPlayerController() const
{
	return Cast<APlayerController>(GetController());
}

void APlayerPawn::AddToSelection(USelectionComponent* SelectionComp, bool bIgnoreModifierKeys)
{
	ensureMsgf(IsValid(SelectionComp), TEXT("Invalid selection component added to selection!"));

	APlayerController* PlayerController = GetPlayerController();
	const bool bClearSelection = bIgnoreModifierKeys || !IsAnyShiftDown(PlayerController);

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
		//Don't deselect the one that was selected
		m_Selection.RemoveSingle(SelectionComp);

		ClearSelection();

		//Add it back
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

void APlayerPawn::SendOrder(UOrderBase* Order)
{
	const bool bClearQueue = !IsAnyShiftDown();

	//Select priority only if we want to add to the queue. There is no need to select it if there are no other orders.
	if (!bClearQueue)
	{
		DisplayPriorityWidgetForOrders({ Order });
	}
	else
	{
		for (USelectionComponent* SelectionComp : m_Selection)
		{
			if (IsValid(SelectionComp))
			{
				AOrderQueueAIController* AIController = GetControllerFromSelectionComponent(SelectionComp);
				if (IsValid(AIController))
				{
					AIController->AddOrder(Order->CreateCopy(AIController), bClearQueue);
				}
			}
		}
	}
}

void APlayerPawn::SendOrders(const TArray<UOrderBase*>& OrderTemplates)
{
	const bool bClearQueue = !IsAnyShiftDown();

	//Select priority only if we want to add to the queue. There is no need to select it if there are no other orders.
	if(!bClearQueue)
	{
		DisplayPriorityWidgetForOrders(OrderTemplates);
	}
	else
	{
		for (USelectionComponent* SelectionComp : m_Selection)
		{
			if (IsValid(SelectionComp))
			{
				AOrderQueueAIController* AIController = GetControllerFromSelectionComponent(SelectionComp);
				if (IsValid(AIController))
				{
					AIController->ClearQueue();

					for (UOrderBase* Order : OrderTemplates)
					{
						AIController->AddOrderWaiting(Order->CreateCopy(AIController));
					}

					AIController->FlushOrders();
				}
			}
		}
	}
}

bool APlayerPawn::IsAnyShiftDown(APlayerController* PlayerController) const
{
	if (!PlayerController)
	{
		PlayerController = GetPlayerController();
	}

	return PlayerController->IsInputKeyDown(EKeys::LeftShift) || PlayerController->IsInputKeyDown(EKeys::RightShift);
}

bool APlayerPawn::IsAnyControlDown(APlayerController* PlayerController) const
{
	if (!PlayerController)
	{
		PlayerController = GetPlayerController();
	}

	return PlayerController->IsInputKeyDown(EKeys::LeftControl) || PlayerController->IsInputKeyDown(EKeys::RightControl);
}

AOrderQueueAIController* APlayerPawn::GetControllerFromSelection(uint32 Index) const
{
	APawn* SelectedPawn = m_Selection[Index]->GetOwner<APawn>();
	if (!IsValid(SelectedPawn))
	{
		return nullptr;
	}

	return Cast<AOrderQueueAIController>(SelectedPawn->GetController());
}

AOrderQueueAIController* APlayerPawn::GetControllerFromSelectionComponent(USelectionComponent* SelectionComp) const
{
	APawn* SelectedPawn = SelectionComp->GetOwner<APawn>();
	if (!IsValid(SelectedPawn))
	{
		return nullptr;
	}

	return Cast<AOrderQueueAIController>(SelectedPawn->GetController());
}

void APlayerPawn::ConfirmPriority(int32 Priority)
{
	m_bSelectingPriority = false;

	for (UOrderBase* Order : m_PriorityPendingOrders)
	{
		Order->Priority = Priority;
	}

	for (USelectionComponent* SelectionComp : m_Selection)
	{
		if (IsValid(SelectionComp))
		{
			AOrderQueueAIController* AIController = GetControllerFromSelectionComponent(SelectionComp);
			if (IsValid(AIController))
			{
				for (UOrderBase* Order : m_PriorityPendingOrders)
				{
					AIController->AddOrderWaiting(Order->CreateCopy(AIController));
				}

				AIController->FlushOrders();
			}
		}
	}

	//GetPlayerController()->SetInputMode(FInputModeGameOnly());
}

void APlayerPawn::DisplayPriorityWidgetForOrders(const TArray<UOrderBase*>& Orders)
{
	m_bSelectingPriority = true;
	m_PriorityPendingOrders = Orders;

	//GetPlayerController()->SetInputMode(FInputModeUIOnly());

	UUserWidget::CreateWidgetInstance(*GetPlayerController(), PrioritySelectionWidgetClass, FName("PrioritySelection"))->AddToViewport();
}