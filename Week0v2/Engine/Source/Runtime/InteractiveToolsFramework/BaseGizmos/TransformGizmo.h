#pragma once
#include "GameFramework/Actor.h"


class StaticMeshComp;
class UTransformGizmo : public AActor
{
    DECLARE_CLASS(UTransformGizmo, AActor)

public:
    UTransformGizmo();

    virtual void Tick(float DeltaTime) override;

    TArray<StaticMeshComp*>& GetArrowArr() { return ArrowArr; }
    TArray<StaticMeshComp*>& GetDiscArr() { return CircleArr; }
    TArray<StaticMeshComp*>& GetScaleArr() { return RectangleArr; }

private:
    TArray<StaticMeshComp*> ArrowArr;
    TArray<StaticMeshComp*> CircleArr;
    TArray<StaticMeshComp*> RectangleArr;
};
