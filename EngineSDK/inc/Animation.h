#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CBone;

class CAnimation :
    public CBase
{
private:
    CAnimation();
    CAnimation(const CAnimation& Prototype);
public:
    virtual ~CAnimation() = default;

public:
    HRESULT Initialize(ifstream& InFile);
    //HRESULT Initialize(const aiAnimation* pAIAnimation, class CModel* pModel);

    _bool Update_TransformationMatrices(_float fTimeDelta, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop);
    _bool Blend_TransformationMatrices(_float timeDelta, const shared_ptr<CAnimation>& nextAnim, _float blendRatio, const vector<shared_ptr<CBone>>& bones, _bool isCurLoop, _bool isNextLoop/*, int32 rootNodeIndex = -1*/);
    void Update_ToBuffer(_float fTimeDelta, vector<_matrix>& OutMatrices, _bool isLoop, _uint boneCount);

    void ReStart() { m_currentTrackPosition = 0.f;
    	for (auto& iIndex : m_currentKeyFrameIndices)
    {
        iIndex = 0;
    }
    }
    _int Get_CurrentFrame() const
    {
        return static_cast<_int>(m_currentTrackPosition);
    }

    _int Get_TotalFrame() const
    {
        return static_cast<_int>(m_duration);
    }
public:
 
    _float m_duration = {};
    _float m_tickPerSecond = {};

    _float m_currentTrackPosition = {};

    _uint m_numChannels = {};
    vector<shared_ptr<class CChannel>> m_channels;

    vector<_uint>	m_currentKeyFrameIndices; // 원래 채널애 있었는데 여기로 이동
public:
    static shared_ptr<CAnimation> Create(ifstream& InFile);
	shared_ptr<CAnimation> Clone();
    //static shared_ptr<CAnimation> Create(const aiAnimation* pAIAnimation, class CModel* pModel);
    void Free() override;


};

NS_END