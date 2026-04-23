#pragma once
#include "Client_Define.h"
#include "Level.h"
NS_BEGIN(Engine)

class CCamera;
	class CUI;
class CGameObject;

NS_END
NS_BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Post_Initialize()override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);

	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Wave(const _wstring& strLayerTag);

	HRESULT Ready_Layer_ETC(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);

private:
	_bool m_OnTab = false;
	shared_ptr<CUI> m_TapUI = {};
	shared_ptr<class CUI_Item> m_HoldItem ={};
	shared_ptr<CUI>  m_pNPC = {};
	shared_ptr<CUI>  m_pMiniGame = {};

	shared_ptr<class CInventory_Controller> m_pInvenCntl = {};
	weak_ptr<CGameObject> m_pPlayer = {};
	weak_ptr<class CInventory> m_PlayerInven = {};

	shared_ptr<class CUI> m_Village = {};

	shared_ptr<class CMiniGameController> m_pMiniGame_Controller = { nullptr };
	shared_ptr<CCamera> ClientCamera = {nullptr};
	_bool m_Flag = false;

public:
	static shared_ptr<CLevel_GamePlay> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};



NS_END;
