#pragma once
#include "UIPanel.h"
#include "Client_Define.h"

namespace Engine
{
	class CUIImage;
	class CUIText;
}

NS_BEGIN(Client)

class CUI_MiniGame :
	public CUIPanel
{
public:
	enum MINIGAME { BASIC_CIRCLE, END };
	struct MINIGAEMEPANEL_DESC : public CUIPanel::UIPANEL_DESC
	{

	};
protected:
	CUI_MiniGame(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI_MiniGame(const CUIPanel& prototype);
public:
	~CUI_MiniGame() = default;

public:

	void UI_PanelActive(MINIGAME MiniGameState, _uint Defid);

	HRESULT OnInit(void* pArg) override;
	void OnActive()override;
	void OnInActive()override;
	void OnDisabled()override;
	void OnUpdate(const _float& timeDelta)override;
	void OnLateUpdate()override;
	HRESULT OnRender()override;
	void OnClear()override;


	void Set_InvenCtrl(shared_ptr<class CInventory_Controller> invenCtrl) {
		m_InvenCtrl = invenCtrl
			;
	}

public:
	Zone m_zones[8];
	float m_zoneCount;
	float m_AccTime = { 0 };
	float m_Angle = { 0 };
	float m_Speed = {};
	_bool m_bStart = { false };

	///
	bool m_changeColor = { false };
	float m_AccTime2 = { 0 };
	float colortime = { 0 };

	///
	float m_prograssBar01 = { 0 };
	float m_RodSpeed = {};


	//
	_uint m_DefID = { ID_Absence };
	_bool m_bFin = { false };

	//
	_uint m_FishCount = {};
private:
	shared_ptr<CUIText> m_nameTex = { nullptr };
	shared_ptr<CUIText> m_amountTex = { nullptr };
	shared_ptr<CUIText> m_locationTex = { nullptr };
	shared_ptr<CUIImage> m_FishIcon = { nullptr };
	shared_ptr<CUIImage> m_Spinner = { nullptr };

	shared_ptr<CUIImage> m_pCircleEff = { nullptr };
	shared_ptr<CUIImage> m_pCircle = { nullptr };
	shared_ptr<CUIImage> m_PrograssIcon = { nullptr };

	shared_ptr<CShader> m_CircleShader = { nullptr };
	//
	weak_ptr<class CInventory_Controller> m_InvenCtrl = {};
public:
	static shared_ptr<CUI_MiniGame> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void Free() override;
};

NS_END