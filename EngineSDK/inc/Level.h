#pragma once
#include "Base.h"

/* 클라 개발자가 만들 각종 레밸에 부모가되는 클래스이다. */
// -> ENGINE_DLL , abstract , virtual 


// explicit 1개 인자 생성자에서 자동 형변환 막기 A a =3;  이게 컴파일 됨
NS_BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

NS_END