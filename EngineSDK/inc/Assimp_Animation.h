#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CAssimp_Animation :
    public CBase
{
private:
    CAssimp_Animation();
public:
    virtual ~CAssimp_Animation() = default;

public:
    HRESULT Initialize(const aiAnimation* srcAIAnimation, shared_ptr<class CAssimp_Model> model);

    _bool Update_TransformationMatrices(_float timeDelta, const vector<shared_ptr<class CAssimp_Bone>>& Bones, _bool isLoop);
    //_bool Update_TransformationMatrices(_float fTimeDelta, const vector<shared_ptr<class CAssimp_Bone>>& Bones, _bool isLoop)

public:
 
    _float m_duration = {};
    _float m_tickPerSecond = {};

    _float m_currentTrackPosition = {};

    _uint m_numChannels = {};
    vector<shared_ptr<class CAssimp_Channel>> m_channels;

public:
    static shared_ptr<CAssimp_Animation> Create(const aiAnimation* srcAIAnimation, shared_ptr<class CAssimp_Model> model);
    void Free() override;


};

NS_END