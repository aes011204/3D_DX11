#pragma once
#include "Base.h"


NS_BEGIN(Engine)
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


public:
 
    _float m_fDuration = {};
    _float m_fTickPerSecond = {};

    _float m_fCurrentTrackPosition = {};

    _uint m_iNumChannels = {};
    vector<shared_ptr<class CChannel>> m_Channels;

    vector<_uint>	m_CurrentKeyFrameIndices; // 원래 채널애 있었는데 여기로 이동
public:
    static shared_ptr<CAnimation> Create(ifstream& InFile);
	shared_ptr<CAnimation> Clone();
    //static shared_ptr<CAnimation> Create(const aiAnimation* pAIAnimation, class CModel* pModel);
    void Free() override;


};

NS_END