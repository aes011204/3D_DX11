#pragma once
#include <cstdint>
#include <windows.h>

struct Cvt_Header
{
    uint32_t iMagic {}; // 'TAD' (내 맘대로 정한 고유번호)
    uint32_t bIsAnim = {};
    uint32_t iNumMeshes = {};

};

struct Cvt_MeshInfo
{
    char szName[64]; 
    uint32_t iMaterialIndex;
    uint32_t iNumVertices;
    uint32_t iNumIndices;

};

struct VTXMESH {
    float vPos[3]={};
    float vUV[3]={};
    float vNormal[3]={};
    float vTangent[3]={};

    int32_t vIndices[4]= { -1, -1, -1, -1 }; // Bone Indices
    float vWeights[4] = { 0.f, 0.f, 0.f, 0.f }; // Bone Weights
};

struct Cvt_Material
{
    char szName[64];
    char szDiffusePath[_MAX_PATH];
    char szNormalPath[_MAX_PATH];
    char szSpecularPath[_MAX_PATH];
    char szOpacityPath[_MAX_PATH];
    /* Color  ambient;
     Color  diffuse;
     Color  specular;
     Color  emissive;*/
};