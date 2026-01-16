#pragma once
#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END


NS_BEGIN(Client)

class CMainApp final 
	: public CBase
{
private:
	CMainApp();
	~CMainApp() = default;

public:
	HRESULT Initialize();
	int Update(_float fTimeDelta);
	void LateUpdate();
	HRESULT Render();


public:

	static CMainApp* Create();

	void Free() override;



private:
	CGameInstance* m_pGameInstance ={ nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

};


NS_END