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
	enum EButtonInfo{ROTATION, THROWUP, RELEASE, SELL, PICK, STORAGE, BUY, END};
	struct ItemInfoDesc : public CUIPanel::UIPanelDesc
	{

	};
protected:
	CItemInfo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CItemInfo(const CUIPanel& prototype);
public:
	~CItemInfo() = default;

public:
	void UI_PanelActive(_bool isHold, ItemInst itemInst, EPlayerState PlayerState, _bool IsPlayer);
	virtual void UI_Active() override; //  UI 활성활시 호출되는 함수// 이 패널은 다 켜지면 안되기떄문에 오버라이딩으로 호출뻇고 여기서 처리
	HRESULT OnInit(void* pArg) override;
	void OnActive()override;
	void OnInActive()override;
	void OnDisabled()override;
	void OnUpdate(const _float& timeDelta)override;
	void OnLateUpdate()override;
    void OnPrepareRender() override;
	HRESULT OnRender()override;
	void OnClear()override;

	void SetPlayerState(EPlayerState state){m_playerState = state;}
public:
	_float Active_ButtonInfo(EButtonInfo btnInfo, _float2 AnchoredPos, _float cost = 0.f, _float2 Pivot = {0.5f, 0.5f});
	void GetButtonInfo(_uint buttonInfo, _uint& Texindex, wstring& str, _float extraInfo);
private:
	shared_ptr<CUIText> m_nameText = {};
	shared_ptr<CUIText> m_leftText = {};
	shared_ptr<CUIText> m_rightText = {};
	shared_ptr<CUIText> m_detailText = {};
	shared_ptr<CUIImage> m_lineImg = {};

	vector<shared_ptr<CUIImage>> m_vecIcon;
	shared_ptr<CUIText> m_buttonInfoText = {};

	_float2 m_targetSize = {};

	_float m_namePadding = {5.f};
	_float m_paddingX = {5.f};
	_float m_paddingY = {5.f};

	EPlayerState m_playerState = {};

public:
	static shared_ptr<CItemInfo> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void Free() override;
};

NS_END