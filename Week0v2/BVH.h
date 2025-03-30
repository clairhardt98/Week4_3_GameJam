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

     //현재 Renderer class 내부에서 
     //BVH.h 내 FBoundingVolume 구조체의 BuildBVHTopDown 함수 수정 예제
    //FBoundingVolume* BuildBVHTopDown(TArray<StaticMeshComp*>& meshes, int depth = 0)
    //{
    //    FBoundingVolume* node = new FBoundingVolume();

    //    // 각 mesh의 월드 AABB를 계산하여 현재 노드의 AABB를 업데이트
    //    for (StaticMeshComp* mesh : meshes)
    //    {
    //        // 각 mesh의 월드 변환 행렬 계산
    //        FMatrix scaleMatrix = FMatrix::CreateScale(
    //            mesh->GetWorldScale().x,
    //            mesh->GetWorldScale().y,
    //            mesh->GetWorldScale().z);
    //        FMatrix rotationMatrix = FMatrix::CreateRotation(
    //            mesh->GetWorldRotation().x,
    //            mesh->GetWorldRotation().y,
    //            mesh->GetWorldRotation().z);
    //        FMatrix translationMatrix = FMatrix::CreateTranslationMatrix(mesh->GetWorldLocation());
    //        FMatrix worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    //        // 변환된 월드 AABB 계산 (TransformBoundingBox 함수의 변형)
    //        FVector localVertices[8] = {
    //            { mesh->AABB.min.x, mesh->AABB.min.y, mesh->AABB.min.z },
    //            { mesh->AABB.max.x, mesh->AABB.min.y, mesh->AABB.min.z },
    //            { mesh->AABB.min.x, mesh->AABB.max.y, mesh->AABB.min.z },
    //            { mesh->AABB.max.x, mesh->AABB.max.y, mesh->AABB.min.z },
    //            { mesh->AABB.min.x, mesh->AABB.min.y, mesh->AABB.max.z },
    //            { mesh->AABB.max.x, mesh->AABB.min.y, mesh->AABB.max.z },
    //            { mesh->AABB.min.x, mesh->AABB.max.y, mesh->AABB.max.z },
    //            { mesh->AABB.max.x, mesh->AABB.max.y, mesh->AABB.max.z }
    //        };

    //        FVector worldVertices[8];

    //        // 첫 번째 버텍스 변환
    //        FMatrixSIMD simdMatrix(worldMatrix);
    //        worldVertices[0] = mesh->GetWorldLocation() + simdMatrix.TransformVector(localVertices[0]);
    //        FVector meshMin = worldVertices[0], meshMax = worldVertices[0];

    //        for (int i = 1; i < 8; ++i)
    //        {
    //            // 매번 동일한 worldMatrix를 사용
    //            worldVertices[i] = mesh->GetWorldLocation() + simdMatrix.TransformVector(localVertices[i]);

    //            meshMin.x = (worldVertices[i].x < meshMin.x) ? worldVertices[i].x : meshMin.x;
    //            meshMin.y = (worldVertices[i].y < meshMin.y) ? worldVertices[i].y : meshMin.y;
    //            meshMin.z = (worldVertices[i].z < meshMin.z) ? worldVertices[i].z : meshMin.z;

    //            meshMax.x = (worldVertices[i].x > meshMax.x) ? worldVertices[i].x : meshMax.x;
    //            meshMax.y = (worldVertices[i].y > meshMax.y) ? worldVertices[i].y : meshMax.y;
    //            meshMax.z = (worldVertices[i].z > meshMax.z) ? worldVertices[i].z : meshMax.z;
    //        }

    //        // worldAABB for current mesh
    //        FBoundingBox worldAABB;
    //        worldAABB.min = meshMin;
    //        worldAABB.max = meshMax;

    //        // 현재 노드의 AABB를 업데이트
    //        node->MinBound = FVector::Min(node->MinBound, worldAABB.min);
    //        node->MaxBound = FVector::Max(node->MaxBound, worldAABB.max);

    //        // 리프 노드 조건: mesh 수가 1 이하이면 해당 mesh들을 저장하고 종료
    //        if (meshes.Num() <= 1)
    //        {
    //            node->Meshes = meshes;
    //            return node;
    //        }

    //        // 분할 기준: 현재 노드 AABB의 가장 긴 축 선택
    //        FVector extent = node->MaxBound - node->MinBound;
    //        int axis = 0; // 0:x, 1:y, 2:z
    //        if (extent.y > extent.x)
    //            axis = 1;
    //        if (extent.z > FVector::GetByIndex(extent, axis))
    //            axis = 2;

    //        // mesh들을 선택된 축의 중앙 좌표 기준으로 정렬
    //        std::sort(meshes.begin(), meshes.end(), [axis](StaticMeshComp* a, StaticMeshComp* b) {
    //            // a의 월드 AABB 중심 계산
    //            FMatrix aScale = FMatrix::CreateScale(a->GetWorldScale().x, a->GetWorldScale().y, a->GetWorldScale().z);
    //            FMatrix aRotation = FMatrix::CreateRotation(a->GetWorldRotation().x, a->GetWorldRotation().y, a->GetWorldRotation().z);
    //            FMatrix aTranslation = FMatrix::CreateTranslationMatrix(a->GetWorldLocation());
    //            FMatrix aWorldMatrix = aScale * aRotation * aTranslation;
    //            FMatrixSIMD aSimd(aWorldMatrix);
    //            FVector aMin = a->AABB.min, aMax = a->AABB.max;
    //            FVector aLocalVerts[8] = {
    //                { aMin.x, aMin.y, aMin.z },
    //                { aMax.x, aMin.y, aMin.z },
    //                { aMin.x, aMax.y, aMin.z },
    //                { aMax.x, aMax.y, aMin.z },
    //                { aMin.x, aMin.y, aMax.z },
    //                { aMax.x, aMin.y, aMax.z },
    //                { aMin.x, aMax.y, aMax.z },
    //                { aMax.x, aMax.y, aMax.z }
    //            };
    //            FVector aWorldMin = a->GetWorldLocation() + aSimd.TransformVector(aLocalVerts[0]);
    //            FVector aWorldMax = aWorldMin;
    //            for (int i = 1; i < 8; ++i)
    //            {
    //                FVector v = a->GetWorldLocation() + aSimd.TransformVector(aLocalVerts[i]);
    //                aWorldMin = FVector::Min(aWorldMin, v);
    //                aWorldMax = FVector::Max(aWorldMax, v);
    //            }
    //            FVector aCenter = (aWorldMin + aWorldMax) * 0.5f;

    //            // b의 월드 AABB 중심 계산 (동일하게)
    //            FMatrix bScale = FMatrix::CreateScale(b->GetWorldScale().x, b->GetWorldScale().y, b->GetWorldScale().z);
    //            FMatrix bRotation = FMatrix::CreateRotation(b->GetWorldRotation().x, b->GetWorldRotation().y, b->GetWorldRotation().z);
    //            FMatrix bTranslation = FMatrix::CreateTranslationMatrix(b->GetWorldLocation());
    //            FMatrix bWorldMatrix = bScale * bRotation * bTranslation;
    //            FMatrixSIMD bSimd(bWorldMatrix);
    //            FVector bMin = b->AABB.min, bMax = b->AABB.max;
    //            FVector bLocalVerts[8] = {
    //                { bMin.x, bMin.y, bMin.z },
    //                { bMax.x, bMin.y, bMin.z },
    //                { bMin.x, bMax.y, bMin.z },
    //                { bMax.x, bMax.y, bMin.z },
    //                { bMin.x, bMin.y, bMax.z },
    //                { bMax.x, bMin.y, bMax.z },
    //                { bMin.x, bMax.y, bMax.z },
    //                { bMax.x, bMax.y, bMax.z }
    //            };
    //            FVector bWorldMin = b->GetWorldLocation() + bSimd.TransformVector(bLocalVerts[0]);
    //            FVector bWorldMax = bWorldMin;
    //            for (int i = 1; i < 8; ++i)
    //            {
    //                FVector v = b->GetWorldLocation() + bSimd.TransformVector(bLocalVerts[i]);
    //                bWorldMin = FVector::Min(bWorldMin, v);
    //                bWorldMax = FVector::Max(bWorldMax, v);
    //            }
    //            FVector bCenter = (bWorldMin + bWorldMax) * 0.5f;

    //            return FVector::GetByIndex(aCenter, axis) < FVector::GetByIndex(bCenter, axis);
    //            });

    //        size_t half = meshes.Num() / 2;
    //        TArray<StaticMeshComp*> left;
    //        TArray<StaticMeshComp*> right;
    //        for (int32 i = 0; i < half; i++)
    //            left.Add(meshes[i]);
    //        for (int32 i = half; i < meshes.Num(); i++)
    //            right.Add(meshes[i]);

    //        node->Left = BuildBVHTopDown(left, depth + 1);
    //        node->Right = BuildBVHTopDown(right, depth + 1);

    //        return node;
    //    }
    //}

    FBoundingVolume* BuildBVHTopDown(TArray<StaticMeshComp*>& meshes, int depth = 0)
    {
        FBoundingVolume* node = new FBoundingVolume();

        // 각 mesh의 월드 AABB를 계산하여 현재 노드의 AABB를 업데이트
        for (StaticMeshComp* mesh : meshes)
        {
            // 각 mesh의 월드 변환 행렬 계산
            FMatrix scaleMatrix = FMatrix::CreateScale(
                mesh->GetWorldScale().x,
                mesh->GetWorldScale().y,
                mesh->GetWorldScale().z);
            FMatrix rotationMatrix = FMatrix::CreateRotation(
                mesh->GetWorldRotation().x,
                mesh->GetWorldRotation().y,
                mesh->GetWorldRotation().z);
            FMatrix translationMatrix = FMatrix::CreateTranslationMatrix(mesh->GetWorldLocation());
            FMatrix worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

            // 변환된 월드 AABB 계산 (TransformBoundingBox 함수의 변형)
            FVector localVertices[8] = {
                { mesh->AABB.min.x, mesh->AABB.min.y, mesh->AABB.min.z },
                { mesh->AABB.max.x, mesh->AABB.min.y, mesh->AABB.min.z },
                { mesh->AABB.min.x, mesh->AABB.max.y, mesh->AABB.min.z },
                { mesh->AABB.max.x, mesh->AABB.max.y, mesh->AABB.min.z },
                { mesh->AABB.min.x, mesh->AABB.min.y, mesh->AABB.max.z },
                { mesh->AABB.max.x, mesh->AABB.min.y, mesh->AABB.max.z },
                { mesh->AABB.min.x, mesh->AABB.max.y, mesh->AABB.max.z },
                { mesh->AABB.max.x, mesh->AABB.max.y, mesh->AABB.max.z }
            };

            FVector worldVertices[8];
            FMatrixSIMD simdMatrix(worldMatrix);

            // 첫 번째 버텍스 계산
            worldVertices[0] = mesh->GetWorldLocation() + simdMatrix.TransformVector(localVertices[0]);
            FVector meshMin = worldVertices[0], meshMax = worldVertices[0];

            // 1부터 7까지 반복하면서 최소, 최대값 업데이트
            for (int i = 1; i < 8; ++i)
            {
                worldVertices[i] = mesh->GetWorldLocation() + simdMatrix.TransformVector(localVertices[i]);
                meshMin.x = (worldVertices[i].x < meshMin.x) ? worldVertices[i].x : meshMin.x;
                meshMin.y = (worldVertices[i].y < meshMin.y) ? worldVertices[i].y : meshMin.y;
                meshMin.z = (worldVertices[i].z < meshMin.z) ? worldVertices[i].z : meshMin.z;

                meshMax.x = (worldVertices[i].x > meshMax.x) ? worldVertices[i].x : meshMax.x;
                meshMax.y = (worldVertices[i].y > meshMax.y) ? worldVertices[i].y : meshMax.y;
                meshMax.z = (worldVertices[i].z > meshMax.z) ? worldVertices[i].z : meshMax.z;
            }

            // worldAABB for current mesh
            FBoundingBox worldAABB;
            worldAABB.min = meshMin;
            worldAABB.max = meshMax;

            // 현재 노드의 AABB를 업데이트 (모든 mesh의 AABB를 포함하도록)
            node->MinBound = FVector::Min(node->MinBound, worldAABB.min);
            node->MaxBound = FVector::Max(node->MaxBound, worldAABB.max);
        }

        // 리프 노드 조건: mesh 수가 1 이하이면 해당 mesh들을 저장하고 종료
        if (meshes.Num() <= 1)
        {
            node->Meshes = meshes;
            return node;
        }

        // 분할 기준: 현재 노드 AABB의 가장 긴 축 선택
        FVector extent = node->MaxBound - node->MinBound;
        int axis = 0; // 0:x, 1:y, 2:z
        if (extent.y > extent.x)
            axis = 1;
        if (extent.z > FVector::GetByIndex(extent, axis))
            axis = 2;

        // mesh들을 선택된 축의 중앙 좌표 기준으로 정렬
        std::sort(meshes.begin(), meshes.end(), [axis](StaticMeshComp* a, StaticMeshComp* b) {
            // a의 월드 AABB 중심 계산
            // (중복된 코드는 실제 코드에서는 함수화하는 것이 좋습니다)
            FMatrix aScale = FMatrix::CreateScale(a->GetWorldScale().x, a->GetWorldScale().y, a->GetWorldScale().z);
            FMatrix aRotation = FMatrix::CreateRotation(a->GetWorldRotation().x, a->GetWorldRotation().y, a->GetWorldRotation().z);
            FMatrix aTranslation = FMatrix::CreateTranslationMatrix(a->GetWorldLocation());
            FMatrix aWorldMatrix = aScale * aRotation * aTranslation;
            FMatrixSIMD aSimd(aWorldMatrix);
            FVector aMin = a->AABB.min, aMax = a->AABB.max;
            FVector aLocalVerts[8] = {
                { aMin.x, aMin.y, aMin.z },
                { aMax.x, aMin.y, aMin.z },
                { aMin.x, aMax.y, aMin.z },
                { aMax.x, aMax.y, aMin.z },
                { aMin.x, aMin.y, aMax.z },
                { aMax.x, aMin.y, aMax.z },
                { aMin.x, aMax.y, aMax.z },
                { aMax.x, aMax.y, aMax.z }
            };
            FVector aWorldMin = a->GetWorldLocation() + aSimd.TransformVector(aLocalVerts[0]);
            FVector aWorldMax = aWorldMin;
            for (int i = 1; i < 8; ++i)
            {
                FVector v = a->GetWorldLocation() + aSimd.TransformVector(aLocalVerts[i]);
                aWorldMin = FVector::Min(aWorldMin, v);
                aWorldMax = FVector::Max(aWorldMax, v);
            }
            FVector aCenter = (aWorldMin + aWorldMax) * 0.5f;

            // b의 월드 AABB 중심 계산 (동일하게)
            FMatrix bScale = FMatrix::CreateScale(b->GetWorldScale().x, b->GetWorldScale().y, b->GetWorldScale().z);
            FMatrix bRotation = FMatrix::CreateRotation(b->GetWorldRotation().x, b->GetWorldRotation().y, b->GetWorldRotation().z);
            FMatrix bTranslation = FMatrix::CreateTranslationMatrix(b->GetWorldLocation());
            FMatrix bWorldMatrix = bScale * bRotation * bTranslation;
            FMatrixSIMD bSimd(bWorldMatrix);
            FVector bMin = b->AABB.min, bMax = b->AABB.max;
            FVector bLocalVerts[8] = {
                { bMin.x, bMin.y, bMin.z },
                { bMax.x, bMin.y, bMin.z },
                { bMin.x, bMax.y, bMin.z },
                { bMax.x, bMax.y, bMin.z },
                { bMin.x, bMin.y, bMax.z },
                { bMax.x, bMin.y, bMax.z },
                { bMin.x, bMax.y, bMax.z },
                { bMax.x, bMax.y, bMax.z }
            };
            FVector bWorldMin = b->GetWorldLocation() + bSimd.TransformVector(bLocalVerts[0]);
            FVector bWorldMax = bWorldMin;
            for (int i = 1; i < 8; ++i)
            {
                FVector v = b->GetWorldLocation() + bSimd.TransformVector(bLocalVerts[i]);
                bWorldMin = FVector::Min(bWorldMin, v);
                bWorldMax = FVector::Max(bWorldMax, v);
            }
            FVector bCenter = (bWorldMin + bWorldMax) * 0.5f;

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

