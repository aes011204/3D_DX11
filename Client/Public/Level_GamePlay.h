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
	_bool m_isOnTab = false;
	shared_ptr<CUI> m_tabUi = {};
	shared_ptr<class CUI_Item> m_holdItem ={};
	shared_ptr<CUI>  m_npc = {};
	shared_ptr<CUI>  m_miniGame = {};

	shared_ptr<class CInventory_Controller> m_invenCtrl = {};
	weak_ptr<CGameObject> m_player = {};
	weak_ptr<class CInventory> m_playerInven = {};

	shared_ptr<class CUI> m_time = {};

	shared_ptr<class CMiniGameController> m_miniGameController = { nullptr };
	shared_ptr<CCamera> ClientCamera = {nullptr};
	_bool m_isFlag = false;


	_bool m_prevIsNight = true;
public:
	static shared_ptr<CLevel_GamePlay> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};



NS_END;
