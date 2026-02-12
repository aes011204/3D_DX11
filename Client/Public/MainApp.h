#pragma once
#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)
class CEditorInstance;
NS_END


NS_BEGIN(Client)

class CMainApp final
	: public CBase
{
private:
	CMainApp();
public:
	~CMainApp();

public:
	HRESULT Initialize();
	int Update(_float fTimeDelta);

	HRESULT Render();


public:

	static unique_ptr<CMainApp> Create();

	void Free() override;

private:
	HRESULT Ready_StartLevel(LEVEL eStartLevelID);


private:
	weak_ptr<CGameInstance> m_pGameInstance = {};
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
	weak_ptr<Editor::CEditorInstance> m_pEditorInstance = { };

};


NS_END