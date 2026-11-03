#pragma once
#pragma once
#include <cstdint>
#include <windows.h>

struct Cvt_Header
{
    uint32_t magic{}; // 
    uint32_t isAnim = {};
    uint32_t numMeshes = {};
    uint32_t numMaterial = {};
    uint32_t numAnimation = { 0 };
    uint32_t totalNumBone = {0};

};

struct Cvt_MeshInfo
{
    char name[MAX_PATH];
    uint32_t materialIndex;
    uint32_t numVertices;
    uint32_t numIndices;
    uint32_t numBones = { 0 };

};

struct Cvt_VTXMESH {


    float pos[3] = {};
    float uv[8][2] = {};
    float normal[3] = {};
    float tangent[3] = {};

    int32_t indices[4] = { -1, -1, -1, -1 }; // Bone Indices
    float weights[4] = { 0.f, 0.f, 0.f, 0.f }; // Bone Weights
};


struct Cvt_Bone {
    char name[MAX_PATH] = {};
    float offsetMatrix[4][4];
    float transformationMatrix[4][4];
    int parentBoneIndex = { -1 }; // -1 이면  root bone 임
};


struct Cvt_BoneAdd
{
    uint32_t boneIndex;
    float offsetMatrix[4][4];

};

struct Cvt_Keyframe
{
    double trackPosition;       // 시간 (Ticks)
    float pos[3];      // 위치
    float rotation[4];      // 회전 (Quaternion: x, y, z, w)
    float scale[3];    // 크기
};

// 2. 채널 (특정 본의 모든 키프레임들)
struct Cvt_Channel 
{
    char name[MAX_PATH]; // 채널의 이름

    char boneName[MAX_PATH];    // 이 채널이 영향을 줄 본의 이름
    uint32_t boneIndex;   
    uint32_t numKeyframes; // 키프레임 개수
};

// 3. 애니메이션 (전체 동작 정보)
struct Cvt_Animation 
{
    char name[MAX_PATH];        // 애니메이션 이름 (예: "Run", "Attack")
    double duration;       // 전체 재생 시간
    double tickPerSecond;  // 초당 티크 수 (속도)
    uint32_t numChannels;  // 채널 개수
};


#define MAX_TEXTURE_SLOT 28


//struct Cvt_MaterialInfo
//{
//    char szName[MAX_PATH]; // 재질 이름
//    char szTexturePaths[MAX_TEXTURE_SLOT][MAX_PATH];
//};

struct Cvt_Material
{
    char name[64];
    char diffusePath[_MAX_PATH];
    char normalPath[_MAX_PATH];
    char specularPath[_MAX_PATH];
    char opacityPath[_MAX_PATH];

    /* Color  ambient;
     Color  diffuse;
     Color  specular;
     Color  emissive;*/
};

//
//struct Cvt_VTXANIMMESH : public Cvt_VTXMESH
//{
//
//    // --- 추가 데이터 ---
//    uint32_t iBlendIndex[4];  // 4바이트 * 4 = 16바이트
//    float    fBlendWeight[4]; // 4바이트 * 4 = 16바이트
//};

enum Cvt_TexType {
    TextureType_NONE = 0,


    TextureType_DIFFUSE = 1,


    TextureType_SPECULAR = 2,

    TextureType_AMBIENT = 3,

    TextureType_EMISSIVE = 4,


    TextureType_HEIGHT = 5,


    TextureType_NORMALS = 6,

    TextureType_SHININESS = 7,

    TextureType_OPACITY = 8,


    TextureType_DISPLACEMENT = 9,

    TextureType_LIGHTMAP = 10,

    TextureType_REFLECTION = 11,



    TextureType_BASE_COLOR = 12,
    TextureType_NORMAL_CAMERA = 13,
    TextureType_EMISSION_COLOR = 14,
    TextureType_METALNESS = 15,
    TextureType_DIFFUSE_ROUGHNESS = 16,
    TextureType_AMBIENT_OCCLUSION = 17,


    TextureType_UNKNOWN = 18,


    TextureType_SHEEN = 19,

    TextureType_CLEARCOAT = 20,


    TextureType_TRANSMISSION = 21,


    TextureType_MAYA_BASE = 22,
    TextureType_MAYA_SPECULAR = 23,
    TextureType_MAYA_SPECULAR_COLOR = 24,
    TextureType_MAYA_SPECULAR_ROUGHNESS = 25,


    TextureType_ANISOTROPY = 26,


    TextureType_GLTF_METALLIC_ROUGHNESS = 27
};