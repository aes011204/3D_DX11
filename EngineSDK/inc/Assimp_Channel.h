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
	HRESULT Initialize(const aiNodeAnim* srcAIChannel, shared_ptr<class CAssimp_Model> model);
	void Update_TransformationMatrix(_float currentTrackPosition, const vector<shared_ptr<class CAssimp_Bone>>& Bones);

private:
	vector<KeyFrame> m_keyFrames;
	_uint m_numKeyFrames = {};
	_uint m_currentKeyFrameIndex = { 0 };
	_int m_boneIndex = { -1 };

public:
	static shared_ptr<CAssimp_Channel> Create(const aiNodeAnim* srcAIChannel, shared_ptr<class CAssimp_Model> model);
	void Free() override;

};

NS_END