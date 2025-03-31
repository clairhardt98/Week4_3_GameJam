#pragma once

#include "Define.h"
#include "Components/StaticMeshComponent.h"

struct FBoundingVolume
{
    FBoundingVolume() :
        MinBound(FLT_MAX, FLT_MAX, FLT_MAX),
        MaxBound(-FLT_MAX, -FLT_MAX, -FLT_MAX),
        Left(nullptr), Right(nullptr)
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

        for (StaticMeshComp* mesh : meshes)
        {
            node->MinBound = FVector::Min(node->MinBound, mesh->AABB.min);
            node->MaxBound = FVector::Max(node->MaxBound, mesh->AABB.max);
        }

        // 분할 기준: 현재 노드의 BV에서 가장 긴 축 선택
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

    FVector MinBound;
    FVector MaxBound;

    FBoundingVolume* Left;
    FBoundingVolume* Right;

    TArray<StaticMeshComp*> Meshes;
};

