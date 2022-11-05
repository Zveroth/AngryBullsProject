// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interaction/InteractionShapeComponent.h"
#include "InteractionSphereComponent.generated.h"



class FPrimitiveSceneProxy;

//This class is a copy of USphereComponent but the base class needed to be changed for interaction.
UCLASS()
class ANGRYBULLSPROJECT_API UInteractionSphereComponent : public UInteractionShapeComponent
{
	GENERATED_BODY()
	
protected:
	/** The radius of the sphere **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category = Shape)
		float SphereRadius;

public:

	UInteractionSphereComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * Change the sphere radius. This is the unscaled radius, before component scale is applied.
	 * @param	InSphereRadius: the new sphere radius
	 * @param	bUpdateOverlaps: if true and this shape is registered and collides, updates touching array for owner actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Sphere")
		void SetSphereRadius(float InSphereRadius, bool bUpdateOverlaps = true);

	// @return the radius of the sphere, with component scale applied.
	UFUNCTION(BlueprintCallable, Category = "Components|Sphere")
		float GetScaledSphereRadius() const;

	// @return the radius of the sphere, ignoring component scale.
	UFUNCTION(BlueprintCallable, Category = "Components|Sphere")
		float GetUnscaledSphereRadius() const;

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual bool IsZeroExtent() const override;
	virtual struct FCollisionShape GetCollisionShape(float Inflation = 0.0f) const override;
	virtual bool AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual void CalcBoundingCylinder(float& CylinderRadius, float& CylinderHalfHeight) const override;
	//~ End USceneComponent Interface

	//~ Begin UShapeComponent Interface
	virtual void UpdateBodySetup() override;
	//~ End UShapeComponent Interface

	// Get the scale used by this shape. This is a uniform scale that is the minimum of any non-uniform scaling.
	// @return the scale used by this shape.
	UFUNCTION(BlueprintCallable, Category = "Components|Sphere")
		float GetShapeScale() const;

	// Sets the sphere radius without triggering a render or physics update.
	FORCEINLINE void InitSphereRadius(float InSphereRadius) { SphereRadius = InSphereRadius; }
};


// ----------------- INLINES ---------------

FORCEINLINE float UInteractionSphereComponent::GetScaledSphereRadius() const
{
	return SphereRadius * GetShapeScale();
}

FORCEINLINE float UInteractionSphereComponent::GetUnscaledSphereRadius() const
{
	return SphereRadius;
}

FORCEINLINE float UInteractionSphereComponent::GetShapeScale() const
{
	return GetComponentTransform().GetMinimumAxisScale();
}