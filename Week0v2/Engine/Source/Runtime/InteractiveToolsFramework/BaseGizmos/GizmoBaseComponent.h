#pragma once
#include "Components/StaticMeshComponent.h"


class UGizmoBaseComponent : public StaticMeshComp
{
    DECLARE_CLASS(UGizmoBaseComponent, StaticMeshComp)

public:
    enum GizmoType : uint8
    {
        ArrowX,
        ArrowY,
        ArrowZ,
        CircleX,
        CircleY,
        CircleZ,
        ScaleX,
        ScaleY,
        ScaleZ
    };
public:
    UGizmoBaseComponent() = default;

    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance) override;
    virtual void TickComponent(float DeltaTime) override;

private:
    GizmoType gizmoType;

public:
    GizmoType GetGizmoType() const { return gizmoType; }
    void SetGizmoType(GizmoType _gizmoType) { gizmoType = _gizmoType; }
};
