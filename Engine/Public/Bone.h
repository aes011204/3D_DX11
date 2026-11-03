#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CBone :
    public CBase
{
private:
    CBone();
public:
    virtual ~CBone() = default;

public:
    HRESULT Initialize(const Cvt_Bone& BoneDesc);

    void Update_CombinedTransformMatrix(const vector<shared_ptr<CBone>> Bones, _fmatrix PreTransformMatrix);
    void Update_TransformationMatrix(_fmatrix TransformMatrix);
public:
    _bool isCompare(const _char* pBoneName) {return !strcmp(pBoneName, m_name);}
    const _float4x4* Get_CombinedTransformationMatrixPtr() const { return &m_combinedTransformationMatrix; }
private:

    _char m_name[MAX_PATH] = {};
    _float4x4 m_transformationMatrix = {};
    _float4x4 m_combinedTransformationMatrix = {};
    _int m_parentBoneIndex = { -1 }; // -1 이면  root bone 임

public:
   
    static shared_ptr<CBone> Create(const Cvt_Bone& BoneDesc);
	shared_ptr<CBone> Clone();
    void Free() override;


};

NS_END