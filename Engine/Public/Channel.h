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
	void Update_TransformationMatrix(_uint* currentKeyFrameIndex, _float currentTrackPosition, const vector<shared_ptr<class CBone>>& Bones);
	void Update_ToMatrix(_uint* currentKeyFrameIndex, _float currentTrackPosition, vector<_matrix>& outMatrices);
	int Get_BoneIndex() const { return m_boneIndex; }
private:
	vector<KeyFrame> m_keyFrames;
	_uint m_numKeyFrames = {};
	//_uint m_iCurrentKeyFrameIndex = { 0 };
	_int m_boneIndex = { -1 };

public:
	static shared_ptr<CChannel> Create(ifstream& InFile);
	void Free() override;

};

NS_END