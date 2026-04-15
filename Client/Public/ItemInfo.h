#pragma once

#include "UIPanel.h"
#include "Client_Define.h"
#include "PlayerBoat.h"
NS_BEGIN(Engine)

class CUIText;

	class CUIButton;
class CUIImage;
NS_END
NS_BEGIN(Client)

class CItemInfo :
	public CUIPanel
{
public:
	enum BUTTONINFO{ROTATION, THROWUP, RELEASE, SELL, PICK, STORAGE, BUY, END};
	struct ITEMINFO_DESC : public CUIPanel::UIPANEL_DESC
	{

	};
protected:
	CItemInfo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CItemInfo(const CUIPanel& prototype);
public:
	~CItemInfo() = default;

public:
	void UI_PanelActive(_bool isHold, Item_Inst itemInst, E_PLAYERSTATE PlayerState);
	virtual void UI_Active() override; //  UI 활성활시 호출되는 함수// 이 패널은 다 켜지면 안되기떄문에 오버라이딩으로 호출뻇고 여기서 처리
	HRESULT OnInit(void* pArg) override;
	void OnActive()override;
	void OnInActive()override;
	void OnDisabled()override;
	void OnUpdate(const _float& timeDelta)override;
	void OnLateUpdate()override;
	HRESULT OnRender()override;
	void OnClear()override;

	void SetPlayerState(E_PLAYERSTATE state){m_PlayerState = state;}
public:
	_float Active_ButtonInfo(BUTTONINFO btnInfo, _float2 AnchoredPos, _float cost = 0.f, _float2 Pivot = {0.5f, 0.5f});
	void GetButtonInfo(_uint buttonInfo, _uint& Texindex, wstring& str, _float extraInfo);
private:
	shared_ptr<CUIText> m_NameText = {};
	shared_ptr<CUIText> m_LeftText = {};
	shared_ptr<CUIText> m_RightText = {};
	shared_ptr<CUIText> m_DetailText = {};
	shared_ptr<CUIImage> m_LineImg = {};

	vector<shared_ptr<CUIImage>> m_vecIcon;
	shared_ptr<CUIText> m_ButtonInfoText = {};

	_float2 m_Targetsize = {};

	_float m_NamePadding = {5.f};
	_float m_PaddingX = {5.f};
	_float m_PaddingY = {5.f};

	E_PLAYERSTATE m_PlayerState = {};

public:
	static shared_ptr<CItemInfo> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void Free() override;
};

NS_END