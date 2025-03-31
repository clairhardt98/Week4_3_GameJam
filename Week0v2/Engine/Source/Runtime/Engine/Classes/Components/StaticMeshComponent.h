#pragma once
#include "Components/MeshComponent.h"
#include "Mesh/StaticMesh.h"

class StaticMeshComp : public UMeshComponent
{
    DECLARE_CLASS(StaticMeshComp, UMeshComponent)

public:
    StaticMeshComp() = default;
    virtual ~StaticMeshComp() override = default;

    PROPERTY(int, selectedSubMeshIndex);

    virtual uint32 GetNumMaterials() const override;
    virtual UMaterial* GetMaterial(uint32 ElementIndex) const override;
    virtual uint32 GetMaterialIndex(FName MaterialSlotName) const override;
    virtual TArray<FName> GetMaterialSlotNames() const override;
    virtual void GetUsedMaterials(TArray<UMaterial*>& Out) const override;

    virtual int CheckRayIntersection(const FVector& rayOrigin, const FVector& rayDirection, float& pfNearHitDistance) override;
    
    UStaticMesh* GetStaticMesh() const { return staticMesh; }

    void SetStaticMesh(UStaticMesh* value)
    { 
        staticMesh = value;
        OverrideMaterials.SetNum(value->GetMaterials().Num());
        AABB = FBoundingBox(staticMesh->GetRenderData()->BoundingBoxMin, staticMesh->GetRenderData()->BoundingBoxMax);
    }


protected:
    UStaticMesh* staticMesh = nullptr;
    int selectedSubMeshIndex = -1;
};