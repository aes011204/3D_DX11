#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class  CChannel :
	public CBase
{
private:
	CChannel();
public:
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(ifstream& InFile);
	void Update_TransformationMatrix(_uint* pCurrentKeyFrameIndex, _float fCurrentTrackPosition, const vector<shared_ptr<class CBone>>& Bones);

private:
	vector<KEYFRAME> m_KeyFrames;
	_uint m_iNumKeyFrames = {};
	//_uint m_iCurrentKeyFrameIndex = { 0 };
	_int m_iBoneIndex = { -1 };

public:
	static shared_ptr<CChannel> Create(ifstream& InFile);
	void Free() override;

};

NS_END