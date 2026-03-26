#pragma once

#include "IModifier.h"
#include "Client_Define.h"
NS_BEGIN(Engine)
class CUIRenderable;
NS_END
NS_BEGIN(Client)
class CFadeModifier :public IModifier
{
public:
	enum FADE {FADE_OUT, FADE_IN, FADE_END};

public:
	CFadeModifier( FADE fadeType, _float duration, _bool useColor, _float4 color);
	virtual ~CFadeModifier();


	// IModifier을(를) 통해 상속됨
	void Tick(float fDeltaTime, CUI* pOwner) override;

	void OnAppear(float fDeltaTime, CUI* pOwner) override;

	void OnDisappear(float fDeltaTime, CUI* pOwner) override;

	virtual bool IsFinished() override { return m_Finished; };

private:
	CUIRenderable* m_This = {nullptr};

	FADE m_fadeType = FADE_END;
	_float m_Duration = {};
	_bool m_useColor = {};
	_float4 m_color = {};


	_float  m_AccTime = {};
	_bool   m_Finished = false;
	_float  m_Alpha = {};
};

NS_END