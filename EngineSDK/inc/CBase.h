#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default; // ~CBase() {} 랑 같음

public:
	// 레퍼런스 카운트를 증가시킨다 
	// 리턴값 : 증가한 레퍼런스 카운트
	unsigned int AddRef();

	// 레퍼런스 카운트를 감소시켜준다 or 삭제한다
	// 리턴값 : 감소하기 이전의 레퍼런스 카운트 
	unsigned int Release();

protected:
	unsigned int m_iRefCnt = {};// 유니폼 초기화, 이제 long XXX

public:
	virtual void Free() {} // 소멸자에 해도 되는데 호출순서를 조금 자율적으로 하기 위해


};

NS_END