#include "FLoaderOBJ.h"
#include "UObject/ObjectFactory.h"
#include "Components/Material/Material.h"
#include "Components/Mesh/StaticMesh.h"

UMaterial* FManagerOBJ::CreateMaterial(FObjMaterialInfo materialInfo)
{
    if (materialMap[materialInfo.MTLName] != nullptr)
        return materialMap[materialInfo.MTLName];

    UMaterial* newMaterial = FObjectFactory::ConstructObject<UMaterial>();
    newMaterial->SetMaterialInfo(materialInfo);
    materialMap.Add(materialInfo.MTLName, newMaterial);
    return newMaterial;
}

UMaterial* FManagerOBJ::GetMaterial(FString name)
{
    return materialMap[name];
}

UStaticMesh* FManagerOBJ::CreateStaticMesh(FString filePath)
{

    OBJ::FStaticMeshRenderData* staticMeshRenderData = FManagerOBJ::LoadObjStaticMeshAsset(filePath);

    if (staticMeshRenderData == nullptr) return nullptr;

    UStaticMesh* staticMesh = GetStaticMesh(staticMeshRenderData->ObjectName);
    if (staticMesh != nullptr) {
        return staticMesh;
    }

    staticMesh = FObjectFactory::ConstructObject<UStaticMesh>();
    staticMesh->SetData(staticMeshRenderData);

    staticMeshMap.Add(staticMeshRenderData->ObjectName, staticMesh);
    return staticMesh;
}

UStaticMesh* FManagerOBJ::CreateStaticMesh(FString filePath, const FMatrix& worldMatrix)
{
    OBJ::FStaticMeshRenderData* baseRenderData = LoadObjStaticMeshAsset(filePath);
    if (baseRenderData == nullptr) return nullptr;

    // baseRenderData는 공유용이므로, 복사해서 변형 적용
    OBJ::FStaticMeshRenderData* transformedData = new OBJ::FStaticMeshRenderData(*baseRenderData);

    // 위치/회전/스케일 반영한 정점 좌표 변환
    FMatrix normalMatrix = FMatrix::Transpose(FMatrix::Inverse(worldMatrix));

    for (FVertexSimple& v : transformedData->Vertices)
    {
        FVector pos(v.x, v.y, v.z);
        pos = worldMatrix.TransformPosition(pos);
        v.x = pos.x;
        v.y = pos.y;
        v.z = pos.z;

        FVector normal(v.nx, v.ny, v.nz);
        normal = FMatrix::TransformVector(normal.Normalize(), normalMatrix);
        v.nx = normal.x;
        v.ny = normal.y;
        v.nz = normal.z;
    }

    // 버텍스 변형 이후 바운딩 박스 재계산
    FLoaderOBJ::ComputeBoundingBox(transformedData->Vertices, transformedData->BoundingBoxMin, transformedData->BoundingBoxMax);

    // StaticMesh 객체 생성
    UStaticMesh* mesh = FObjectFactory::ConstructObject<UStaticMesh>();
    mesh->SetData(transformedData);
    return mesh;
}


UStaticMesh* FManagerOBJ::GetStaticMesh(FWString name)
{
    return staticMeshMap[name];
}
