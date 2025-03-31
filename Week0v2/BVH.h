#pragma once

#include "Define.h"
#include "Components/StaticMeshComponent.h"

struct FBoundingVolume
{
    FBoundingVolume() :
        MinBound(FLT_MAX, FLT_MAX, FLT_MAX),
        MaxBound(-FLT_MAX, -FLT_MAX, -FLT_MAX),
        Left(nullptr), Right(nullptr),
        bIsRenderable(false)
    {
    }
    bool IsLeaf() const { return Left == nullptr && Right == nullptr; }

    FBoundingVolume* BuildBVHTopDown(TArray<StaticMeshComp*>& meshes, int depth = 0)
    {
        if (meshes.Num() <= 1)
        {
            FBoundingVolume* leafNode = new FBoundingVolume();
            leafNode->Meshes = meshes;
            if (meshes.Num() == 1)
            {
                leafNode->MinBound = meshes[0]->AABB.min;
                leafNode->MaxBound = meshes[0]->AABB.max;
            }
            return leafNode;
        }

        FBoundingVolume* node = new FBoundingVolume();

        // 모든 메쉬의 AABB를 합쳐서 현재 노드의 AABB를 계산
        for (StaticMeshComp* mesh : meshes)
        {
            node->MinBound = FVector::Min(node->MinBound, mesh->AABB.min);
            node->MaxBound = FVector::Max(node->MaxBound, mesh->AABB.max);
        }

        // 분할 기준: 현재 노드 AABB의 가장 긴 축 선택
        FVector extent = node->MaxBound - node->MinBound;
        int axis = 0; // 0:x, 1:y, 2:z
        if (extent.y > extent.x)
            axis = 1;
        if (extent.z > FVector::GetByIndex(extent, axis))
            axis = 2;

        // 선택한 축의 중앙 좌표를 기준으로 정렬
        std::sort(meshes.begin(), meshes.end(), [axis](StaticMeshComp* a, StaticMeshComp* b) {
            FVector aCenter = (a->AABB.min + a->AABB.max) * 0.5f;
            FVector bCenter = (b->AABB.min + b->AABB.max) * 0.5f;
            return FVector::GetByIndex(aCenter, axis) < FVector::GetByIndex(bCenter, axis);
            });

        size_t half = meshes.Num() / 2;
        TArray<StaticMeshComp*> left;
        TArray<StaticMeshComp*> right;
        for (int32 i = 0; i < half; i++)
            left.Add(meshes[i]);
        for (int32 i = half; i < meshes.Num(); i++)
            right.Add(meshes[i]);

        node->Left = BuildBVHTopDown(left, depth + 1);
        node->Right = BuildBVHTopDown(right, depth + 1);

        // 내부 노드에도 모든 자식 메쉬 정보를 병합해서 저장하고 싶다면:
        if (node->Left && node->Right)
        {
            TArray<StaticMeshComp*> merged;
            for (int32 i = 0; i < node->Left->Meshes.Num(); ++i)
            {
                merged.Add(node->Left->Meshes[i]);
            }
            for (int32 i = 0; i < node->Right->Meshes.Num(); ++i)
            {
                merged.Add(node->Right->Meshes[i]);
            }
            node->Meshes = merged;

        }
        else if (node->Left)
        {
            node->Meshes = node->Left->Meshes;
        }
        else if (node->Right)
        {
            node->Meshes = node->Right->Meshes;
        }

        return node;
    }

    // 재귀적으로 BVH 노드를 순회하며 피킹하는 함수
    bool BVHIntersect(const FVector& rayOrigin, const FVector& rayDirection, float& closestHit, StaticMeshComp*& outHitMesh) const
    {
         //먼저, 현재 노드의 경계가 유효한지 체크합니다.
        if (MinBound.x > MaxBound.x || MinBound.y > MaxBound.y || MinBound.z > MaxBound.z)
            return false;

        float nodeHitDistance;
        if (!FBoundingBox(MinBound, MaxBound).Intersect(rayOrigin, rayDirection, nodeHitDistance))
            return false;

        bool hitFound = false;

        if (IsLeaf())
        {
            // 현재는 임계값이 1이라서 단일 메쉬 즉 AABB로 바로 검사함
            for (StaticMeshComp* mesh : Meshes)
            {
                float meshHitDistance;
                if (mesh->CheckRayIntersection(rayOrigin, rayDirection, meshHitDistance) > 0)
                {
                    if (meshHitDistance < closestHit)
                    {
                        closestHit = meshHitDistance;
                        outHitMesh = mesh;
                        hitFound = true;
                    }
                }
            }
        }
        else
        {
            if (Left && Left->BVHIntersect(rayOrigin, rayDirection, closestHit, outHitMesh))
                hitFound = true;
            if (Right && Right->BVHIntersect(rayOrigin, rayDirection, closestHit, outHitMesh))
                hitFound = true;
        }

        return hitFound;
    }
    
    bool IsBVHInsideFrustum(const TArray<FrustumPlane>& planes)
    {
        FBoundingBox box;
        box.min = MinBound;
        box.max = MaxBound;

        if (!IsLeaf())
        {
            bool leftInside = Left->IsBVHInsideFrustum(planes);
            bool rightInside = Right->IsBVHInsideFrustum(planes);
        }

        // 현재 노드의 AABB가 절두체 내부에 있는지 확인
        if (!IsBoxInsideFrustum(box, planes))
        {
            bIsRenderable = false;
            return false;
        }
        bIsRenderable = true;
        return true;
    }

    bool IsBoxInsideFrustum(const FBoundingBox& box, const TArray<FrustumPlane>& planes)
    {
        for (int i = 0; i < 6; i++) {
            FVector positive;
            positive.x = (planes[i].Normal.x >= 0) ? box.max.x : box.min.x;
            positive.y = (planes[i].Normal.y >= 0) ? box.max.y : box.min.y;
            positive.z = (planes[i].Normal.z >= 0) ? box.max.z : box.min.z;

            float distance = planes[i].Normal.Dot(positive) + planes[i].Distance;
            if (distance < 0)
                return false;
        }
        return true;
    }

    TArray<StaticMeshComp*> GetChunkMeshes(const TArray<FrustumPlane>& planes)
    {
        TArray<StaticMeshComp*> result;
        FBoundingBox box;
        box.min = MinBound;
        box.max = MaxBound;

        // 현재 노드 AABB가 frustum 내부에 없으면 빈 배열 반환
        if (!IsBoxInsideFrustum(box, planes))
            return result;

        // 만약 이 노드가 청크 크기 이하라면, 이 노드에 저장된 Meshes를 반환
        if (Meshes.Num() <= 32)
        {
            return Meshes;
        }
        else
        {
            // 자식 노드들의 결과를 병합하여 반환합니다.
            if (Left)
            {
                TArray<StaticMeshComp*> leftMeshes = Left->GetChunkMeshes(planes);
                for (int32 i = 0; i < leftMeshes.Num(); ++i)
                    result.Add(leftMeshes[i]);
            }
            if (Right)
            {
                TArray<StaticMeshComp*> rightMeshes = Right->GetChunkMeshes(planes);
                for (int32 i = 0; i < rightMeshes.Num(); ++i)
                    result.Add(rightMeshes[i]);
            }
        }
        return result;
    }

    FVector MinBound;
    FVector MaxBound;

    FBoundingVolume* Left;
    FBoundingVolume* Right;

    TArray<StaticMeshComp*> Meshes;
    bool bIsRenderable;
};

