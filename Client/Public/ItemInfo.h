#pragma once

#include "UIPanel.h"
#include "Client_Define.h"
#include "PlayerBoat.h"
NS_BEGIN(Engine)

class CUIButton;
class CUIImage;
NS_END
NS_BEGIN(Client)

class CItemInfo :
	public CUIPanel
{
public:

	struct ITEMINFO_DESC : public CUIPanel::UIPANEL_DESC
	{

	};
protected:
	CItemInfo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CItemInfo(const CUIPanel& prototype);
public:
	~CItemInfo() = default;

public:
	void UI_PanelActive(_bool isHold, Item_Inst itemDef, LOCATIONSTATE locationState);
	virtual void UI_Active() override; //  UI 활성활시 호출되는 함수// 이 패널은 다 켜지면 안되기떄문에 오버라이딩으로 호출뻇고 여기서 처리
	HRESULT OnInit(void* pArg) override;
	void OnActive()override;
	void OnInActive()override;
	void OnDisabled()override;
	void OnUpdate(const _float& timeDelta)override;
	void OnLateUpdate()override;
	HRESULT OnRender()override;
	void OnClear()override;

public:


private:

public:
	static shared_ptr<CItemInfo> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void Free() override;
};

NS_END