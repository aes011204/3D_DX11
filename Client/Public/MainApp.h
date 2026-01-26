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
	HRESULT Ready_StartLevel(LEVEL eStartLevelID);
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

private:
	CGameInstance* m_pGameInstance ={ nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

};


NS_END