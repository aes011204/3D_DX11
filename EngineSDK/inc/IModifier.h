#pragma once
#include"Base.h"

//인터페이스 클래스의 특징? 규칙
//멤버 변수는 가급적 갖지 않는다
//가상 소멸자는 필수임
//모든 함수는 순수 가상 함수로 만든다

// 함수의 pOwner 은 맴버변수가 아닌이상 그냥  row pointer가 나음 
// 일단 해당 함수가 호출되었는것이 부모가 살아있다는거라는것

NS_BEGIN(Engine)

class ENGINE_DLL IModifier 
{
public:
	virtual ~IModifier() = default;
public:
	virtual void Tick(float fDeltaTime, class CUI* pOwner) = 0;
	virtual void OnAppear(float fDeltaTime, class CUI* pOwner) = 0;
	virtual void OnDisappear(float fDeltaTime, class CUI* pOwner) = 0;
};

NS_END