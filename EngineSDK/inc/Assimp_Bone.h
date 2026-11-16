#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CAssimp_Bone :
    public CBase
{
private:
    CAssimp_Bone();
public:
    virtual ~CAssimp_Bone() = default;

public:
    HRESULT Initialize(const aiNode* srcAINode, _int parentBoneIndex);

    void Update_CombinedTransformMatrix(const vector<shared_ptr<CAssimp_Bone>> Bones, _fmatrix preTransformMatrix);
    void Update_TransformationMatrix(_fmatrix transformMatrix);
public:
    _bool isCompare(const _char* boneName) {return !strcmp(boneName, m_name);}
    const _float4x4* Get_CombinedTransformationMatrixPtr() const { return &m_combinedTransformationMatrix; }
private:

    _char m_name[MAX_PATH] = {};
    _float4x4 m_transformationMatrix = {};
    _float4x4 m_combinedTransformationMatrix = {};
    _int m_parentBoneIndex = { -1 }; // -1 이면  root bone 임

public:
    static shared_ptr<CAssimp_Bone> Create(const aiNode* srcAINode, _int parentBoneIndex);
    void Free() override;


};

NS_END