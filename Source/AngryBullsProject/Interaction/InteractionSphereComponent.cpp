// Copyright Epic Games, Inc. All Rights Reserved.


#include "Interaction/InteractionSphereComponent.h"
#include "WorldCollision.h"
#include "PrimitiveViewRelevance.h"
#include "PrimitiveSceneProxy.h"
#include "PhysicsEngine/SphereElem.h"
#include "SceneManagement.h"
#include "PhysicsEngine/BodySetup.h"
#include "PrimitiveSceneProxy.h"



UInteractionSphereComponent::UInteractionSphereComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SphereRadius = 32.0f;
	ShapeColor = FColor(255, 0, 0, 255);

	bUseEditorCompositing = true;
}

FBoxSphereBounds UInteractionSphereComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FVector::ZeroVector, FVector(SphereRadius), SphereRadius).TransformBy(LocalToWorld);
}

void UInteractionSphereComponent::CalcBoundingCylinder(float& CylinderRadius, float& CylinderHalfHeight) const
{
	CylinderRadius = SphereRadius * GetComponentTransform().GetMaximumAxisScale();
	CylinderHalfHeight = CylinderRadius;
}

template <EShapeBodySetupHelper UpdateBodySetupAction>
bool InvalidateOrUpdateSphereBodySetup(UBodySetup*& ShapeBodySetup, bool bUseArchetypeBodySetup, float SphereRadius)
{
	check((bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::InvalidateSharingIfStale) || (!bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup));
	check(ShapeBodySetup->AggGeom.SphereElems.Num() == 1);
	FKSphereElem* SphereElem = ShapeBodySetup->AggGeom.SphereElems.GetData();

	// check for mal formed values
	float Radius = SphereRadius;
	if (Radius < KINDA_SMALL_NUMBER)
	{
		Radius = 0.1f;
	}

	if (UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup)
	{
		// now set the PhysX data values
		SphereElem->Center = FVector::ZeroVector;
		SphereElem->Radius = Radius;
	}
	else
	{
		if (SphereElem->Radius != Radius)
		{
			ShapeBodySetup = nullptr;
			bUseArchetypeBodySetup = false;
		}
	}

	return bUseArchetypeBodySetup;
}

void UInteractionSphereComponent::UpdateBodySetup()
{
	if (PrepareSharedBodySetup<UInteractionSphereComponent>())
	{
		bUseArchetypeBodySetup = InvalidateOrUpdateSphereBodySetup<EShapeBodySetupHelper::InvalidateSharingIfStale>(ShapeBodySetup, bUseArchetypeBodySetup, SphereRadius);
	}

	//CreateShapeBodySetupIfNeeded<FKSphereElem>();
	//------LINKING ERROR FIX------
	if (ShapeBodySetup == nullptr || ShapeBodySetup->IsPendingKill())
	{
		ShapeBodySetup = NewObject<UBodySetup>(this, NAME_None, RF_Transient);
		if (GUObjectArray.IsDisregardForGC(this))
		{
			ShapeBodySetup->AddToRoot();
		}

		// If this component is in GC cluster, make sure we add the body setup to it to
		ShapeBodySetup->AddToCluster(this);
		// if we got created outside of game thread, but got added to a cluster, 
		// we no longer need the Async flag
		if (ShapeBodySetup->HasAnyInternalFlags(EInternalObjectFlags::Async) && GUObjectClusters.GetObjectCluster(ShapeBodySetup))
		{
			ShapeBodySetup->ClearInternalFlags(EInternalObjectFlags::Async);
		}

		ShapeBodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;
		ShapeBodySetup->AggGeom.SphereElems.Add(FKSphereElem()); //AddShapeToGeomArray<FKSphereElem>();
		ShapeBodySetup->bNeverNeedsCookedCollisionData = true;
		bUseArchetypeBodySetup = false;	//We're making our own body setup, so don't use the archetype's.

		//Update bodyinstance and shapes
		BodyInstance.BodySetup = ShapeBodySetup;
		{
			if (BodyInstance.IsValidBodyInstance())
			{
				FPhysicsCommand::ExecuteWrite(BodyInstance.GetActorReferenceWithWelding(), [this](const FPhysicsActorHandle& Actor)
				{
					TArray<FPhysicsShapeHandle> Shapes;
					BodyInstance.GetAllShapes_AssumesLocked(Shapes);

					for (FPhysicsShapeHandle& Shape : Shapes)	//The reason we iterate is we may have multiple scenes and thus multiple shapes, but they are all pointing to the same geometry
					{
						//Update shape with the new body setup. Make sure to only update shapes owned by this body instance
						if (BodyInstance.IsShapeBoundToBody(Shape))
						{
							FPhysicsInterface::SetUserData(Shape, (void*)ShapeBodySetup->AggGeom.SphereElems[0].GetUserData()); //SetShapeToNewGeom<FKSphereElem>(Shape);
						}
					}
				});
			}
		}
	}
	//------------

	if (!bUseArchetypeBodySetup)
	{
		InvalidateOrUpdateSphereBodySetup<EShapeBodySetupHelper::UpdateBodySetup>(ShapeBodySetup, bUseArchetypeBodySetup, SphereRadius);
	}
}

void UInteractionSphereComponent::SetSphereRadius(float InSphereRadius, bool bUpdateOverlaps)
{
	SphereRadius = InSphereRadius;
	UpdateBounds();
	UpdateBodySetup();
	MarkRenderStateDirty();

	if (bPhysicsStateCreated)
	{
		// Update physics engine collision shapes
		BodyInstance.UpdateBodyScale(GetComponentTransform().GetScale3D(), true);

		if (bUpdateOverlaps && IsCollisionEnabled() && GetOwner())
		{
			UpdateOverlaps();
		}
	}
}

bool UInteractionSphereComponent::IsZeroExtent() const
{
	return SphereRadius == 0.f;
}


FPrimitiveSceneProxy* UInteractionSphereComponent::CreateSceneProxy()
{
	/** Represents a DrawLightRadiusComponent to the scene manager. */
	class FSphereSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		/** Initialization constructor. */
		FSphereSceneProxy(const UInteractionSphereComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(InComponent->bDrawOnlyIfSelected)
			, SphereColor(InComponent->ShapeColor)
			, SphereRadius(InComponent->SphereRadius)
		{
			bWillEverBeLit = false;
		}

		// FPrimitiveSceneProxy interface.

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_SphereSceneProxy_GetDynamicMeshElements);

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

					const FMatrix& LocalToWorld = GetLocalToWorld();
					const FLinearColor DrawSphereColor = GetViewSelectionColor(SphereColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					// Taking into account the min and maximum drawing distance
					const float DistanceSqr = (View->ViewMatrices.GetViewOrigin() - LocalToWorld.GetOrigin()).SizeSquared();
					if (DistanceSqr < FMath::Square(GetMinDrawDistance()) || DistanceSqr > FMath::Square(GetMaxDrawDistance()))
					{
						continue;
					}

					float AbsScaleX = LocalToWorld.GetScaledAxis(EAxis::X).Size();
					float AbsScaleY = LocalToWorld.GetScaledAxis(EAxis::Y).Size();
					float AbsScaleZ = LocalToWorld.GetScaledAxis(EAxis::Z).Size();
					float MinAbsScale = FMath::Min3(AbsScaleX, AbsScaleY, AbsScaleZ);

					FVector ScaledX = LocalToWorld.GetUnitAxis(EAxis::X) * MinAbsScale;
					FVector ScaledY = LocalToWorld.GetUnitAxis(EAxis::Y) * MinAbsScale;
					FVector ScaledZ = LocalToWorld.GetUnitAxis(EAxis::Z) * MinAbsScale;

					const int32 SphereSides = FMath::Clamp<int32>(SphereRadius / 4.f, 16, 64);
					DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledX, ScaledY, DrawSphereColor, SphereRadius, SphereSides, SDPG_World);
					DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledX, ScaledZ, DrawSphereColor, SphereRadius, SphereSides, SDPG_World);
					DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledY, ScaledZ, DrawSphereColor, SphereRadius, SphereSides, SDPG_World);
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bVisibleForSelection = !bDrawOnlyIfSelected || IsSelected();
			const bool bVisibleForShowFlags = true; // @TODO

			// Should we draw this because collision drawing is enabled, and we have collision
			const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && bVisibleForSelection && bVisibleForShowFlags) || bShowForCollision;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}

		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	private:
		const uint32				bDrawOnlyIfSelected : 1;
		const FColor				SphereColor;
		const float					SphereRadius;
	};

	return new FSphereSceneProxy(this);
}


FCollisionShape UInteractionSphereComponent::GetCollisionShape(float Inflation) const
{
	const float Radius = FMath::Max(0.f, GetScaledSphereRadius() + Inflation);
	return FCollisionShape::MakeSphere(Radius);
}

bool UInteractionSphereComponent::AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const
{
	// All rotations are equal when scale is uniform.
	// Not detecting rotations around non-uniform scale.
	return Scale3D.GetAbs().AllComponentsEqual() || A.Equals(B);
}
