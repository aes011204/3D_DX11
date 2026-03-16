#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class  CAssimp_Channel :
	public CBase
{
private:
	CAssimp_Channel();
public:
	virtual ~CAssimp_Channel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, shared_ptr<class CAssimp_Model> pModel);
	void Update_TransformationMatrix(_float fCurrentTrackPosition, const vector<shared_ptr<class CAssimp_Bone>>& Bones);

private:
	vector<KEYFRAME> m_KeyFrames;
	_uint m_iNumKeyFrames = {};
	_uint m_iCurrentKeyFrameIndex = { 0 };
	_int m_iBoneIndex = { -1 };

public:
	static shared_ptr<CAssimp_Channel> Create(const aiNodeAnim* pAIChannel, shared_ptr<class CAssimp_Model> pModel);
	void Free() override;

};

NS_END