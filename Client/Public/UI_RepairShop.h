#pragma once
#include "UIText.h"

#include "UIPanel.h"
#include "Client_Define.h"
#include "PlayerBoat.h"
//오른쪽에서 나오는 효과있음
// 엔진 루프용 m_Children과는 별개로, 내가 직접 '조종'할 녀석들만 포인터로 보관
// 버튼은 상태를 바꿀 일이 거의 없으니 굳이 안 들고 있어도 되지만, 
// 패널(내용물)은 껏다 켰다 해야 하니 리스트로 관리합니다.

NS_BEGIN(Engine)

class CUIButton;
class CUIImage;
NS_END
NS_BEGIN(Client)

class CUI_RepairShop :
	public CUIPanel
{
public:

	struct REPAIR_DESC : public CUIPanel::UIPANEL_DESC
	{

	};
protected:
	CUI_RepairShop(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI_RepairShop(const CUIPanel& prototype);
public:
	~CUI_RepairShop() = default;

public:
	void UI_PanelActive();
	virtual void UI_Active() override; //  UI 활성활시 호출되는 함수// 이 패널은 다 켜지면 안되기떄문에 오버라이딩으로 호출뻇고 여기서 처리
	HRESULT OnInit(void* pArg) override;
	void OnActive()override;
	void OnInActive()override;
	void OnDisabled()override;
	void OnUpdate(const _float& timeDelta)override;
	void OnLateUpdate()override;
	HRESULT OnRender()override;
	void OnClear()override;

	void SetActiveTab(_uint btnIndex);
public:


private:

	shared_ptr<CUIImage> m_Line = { nullptr };
	shared_ptr<CUIImage> m_Line2 = { nullptr };
	shared_ptr<CUIImage> m_TextIMG = { nullptr };
	shared_ptr<CUIText> m_TextFont = { nullptr };
	shared_ptr<CUIButton> m_BtnFix = { nullptr };
	shared_ptr<CUIText> m_FixButtonTex = { nullptr };
	float m_repairPrice = {};

	shared_ptr<CUIButton> m_ButtonContents[ETOI(SHOPTAB::END)] = {};
	shared_ptr<CUIPanel> m_TabContents[ETOI(SHOPTAB::END)] = {};


	Vector2 m_vecAni = {};
	_bool m_bStart = false;
	float m_TimeAcc = {};
	float m_fDuration = {};


	//weak_ptr<CPlayerBoat> m_PlayerBoat = {  };

public:
	static shared_ptr<CUI_RepairShop> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void Free() override;
};

NS_END