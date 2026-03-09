#include "UI_TabContainer.h"

#include "UIButton.h"
#include "UI_Inventory.h"


CUI_TabContainer::CUI_TabContainer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CUI_TabContainer::CUI_TabContainer(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

void CUI_TabContainer::UI_Active()
{

	//m_bEnabled = true;
	//m_bVisible = true;

	//m_bRenderReady = false;

	//if (m_pUITransformCom)
	//	m_pUITransformCom->UpdateLayoutIfDirty();

	////m_bInteractable = true;
	//OnActive();

	////for (auto& it : m_Children)
	////{
	////	it->UI_Active();
	////}


}

void CUI_TabContainer::UI_PanelActive(_uint iTabfig, TAB Active)
{
	//m_Children.clear();
	_uint numPanel = {};
	// 일단 기본적으로 패널 은 다 inactive, 버튼은 iTabfig 에 따라 active
	//::OnActive() 에서 클릭한 거만 활성화
	if (iTabfig & ETOI(TAB::INVEN))
	{
		//Add_Child(m_ButtonContents[ETOI(TAB::INVEN)], L"BUTTON_INVEN", false);
		(m_ButtonContents[ETOI(TAB::INVEN)]->UI_Active());
		//Add_Child(m_TabContents[ETOI(TAB::INVEN)], L"INVEN", false);
		m_TabContents[ETOI(TAB::INVEN)]->UI_InActive();
		numPanel++;
	}
	if (iTabfig & ETOI(TAB::STORAGE))
	{
		//(m_ButtonContents[ETOI(TAB::STORAGE)], L"BUTTON_STORAGE", false);
		(m_ButtonContents[ETOI(TAB::STORAGE)]->UI_Active());
		//Add_Child(m_TabContents[ETOI(TAB::STORAGE)], L"STORAGE", false);
		m_TabContents[ETOI(TAB::STORAGE)]->UI_InActive();
		numPanel++;

	}
	if (iTabfig & ETOI(TAB::ETC))
	{
		//(m_ButtonContents[ETOI(TAB::ETC)], L"BUTTON_ETC", false);
		m_ButtonContents[ETOI(TAB::ETC)]->UI_Active();
		//Add_Child(m_TabContents[ETOI(TAB::ETC)], L"ETC", false);
		m_TabContents[ETOI(TAB::ETC)]->UI_InActive();
		numPanel++;
	}



	Change_LayoutRawCol(1, numPanel);

	m_Active = Active;

	Set_ActiveForCustom();

	m_bRenderReady = false;

	if (m_pUITransformCom)
		m_pUITransformCom->UpdateLayoutIfDirty();

	//m_bInteractable = true;
	OnActive();

	//for (auto& it : m_Children)
	//{
	//	it->UI_Active();
	//}

	//for (auto& it : m_ButtonContents)
	//	it->UI_Active();


	//for (auto& it : m_TabContents)
	//	it->UI_InActive();
}

HRESULT CUI_TabContainer::OnInit(void* pArg)
{
	HRESULT hr = E_FAIL;
	TABCONTAINER_DESC* TABpDesc = static_cast<TABCONTAINER_DESC*>(pArg);
	//TABCONTAINER_DESC pDesc = {};
	TABpDesc->IsFullScreen = false;
	TABpDesc->IsTrnasparent = false;
	TABpDesc->TextureComLevel = ETOI(LEVEL::STATIC);
	TABpDesc->TextureProtoName = L"Prototype_Component_Texture_TabContainer";
	TABpDesc->IsUseLayout = true;
	//pDesc.vAnchorPoint = 일단 패스
	//	pDesc.vPivot
	//	pDesc.vAnchoredPos

	hr = CUIPanel::OnInit(TABpDesc);

	for (_uint i = 0; i < 3; i++)
	{
		TAB eTab = static_cast<TAB>(1 << i);


		/// 버튼
		CUIButton::UIBUTTON_DESC ButDesc = {};
		ButDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		ButDesc.TextureProtoName = L"Prototype_Component_Texture_Button_RED";



		//ButDesc.vAnchoredPos = Vector2{ 0.f,16.7f + (98.f ) };
		//ButDesc.vSizeDelta = Vector2{ 50.f,50.f };// 안건드려도 됨 텍스쳐에서 초기화 예정
		//ButDesc.vAnchorPoint = Vector2{ 0.254f,0.5f };
		//ButDesc.vPivot = Vector2{ 0.5f, 0.5f };
		//ButDesc.vScale = Vector2{ 1.3f, 1.0f };
		ButDesc.Index = ETOI(eTab);
		ButDesc.OverlapStartEvent = [](CUIButton* pThis) {};
		ButDesc.OverlapEndEvent = [](CUIButton* pThis) {};
		ButDesc.ClickEvent = [this](CUIButton* pThis)
			{
				SetActiveTab(static_cast<TAB>(pThis->Get_TypeIndex()));
			};
		shared_ptr<CUIButton> pChild = CUIButton::Create(m_pDevice, m_pContext);
		pChild->Initialize(&ButDesc);
		pChild->UI_InActive();


		//Add_Child(pChild, NameTag, false);
		wstring NameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(eTab)));
		Add_Layout_Child(pChild, NameTag, false);
		m_ButtonContents[ETOI(eTab)] = pChild;

	}
	for (_uint i = 0; i < 3; i++)
	{
		TAB eTab = static_cast<TAB>(1 << i);
		// 일단 테스트 인벤 3개

		CUI_Inventory::INVENTORY_DESC InvenDesc = {};
		InvenDesc.IsFullScreen = false;
		InvenDesc.IsTrnasparent = true;
		InvenDesc.bSetParentSize = true;


		shared_ptr<CUI_Inventory> pInven = CUI_Inventory::Create(m_pDevice, m_pContext);
		pInven->Initialize(&InvenDesc);

		wstring NameTag = S2W(string(magic_enum::enum_name(eTab)));

		//(pInven, L"INVENTORY", false);
		Add_Child(pInven, NameTag, false);
		m_TabContents[ETOI(eTab)] = pInven;
		pInven->UI_InActive();
	}


	return hr;
}

void CUI_TabContainer::OnActive()
{
	m_TabContents[ETOI(m_Active)]->UI_Active();
	m_ButtonContents[ETOI(m_Active)]->ChangeState(BUTTON_STATE::SELECT);

	//m_TabContents[ETOI(m_Active)]->GetUITransform()->SetLocalScale(GetUITransform()->Get_LocalScale());
	//m_TabContents[ETOI(m_Active)]->GetUITransform()->SetSizeDelta(GetUITransform()->Get_SizeDelta());

	m_ButtonContents[ETOI(m_Active)]->Set_Zorder(2);

	GetUITransform()->SetAnchoredPos(Vector2{ GetUITransform()->Get_FinalSize().x, 0.f });
	m_vecAni = Vector2{ GetUITransform()->Get_FinalSize().x, 0.f };
	m_bStart = true;
	m_fDuration = 1.5f;

	CUIPanel::OnActive();
}

void CUI_TabContainer::OnInActive()
{
	m_bStart = false;
	m_TimeAcc = 0;
	CUIPanel::OnInActive();
}

void CUI_TabContainer::OnDisabled()
{

	CUIPanel::OnDisabled();
}

void CUI_TabContainer::OnUpdate(const _float& timeDelta)
{
	// 여기에서 안보였다가 와야함
	if (m_bStart == true)
	{
		m_TimeAcc += timeDelta;

		float t = m_TimeAcc / m_fDuration;

		if (t >= 1.f) t = 1.f;

		m_vecAni = Vector2{ lerp(m_vecAni.x, 0.f, t),0.f };
		GetUITransform()->SetAnchoredPos(m_vecAni);
		LOG_F(LOG_LEVEL::INFO, "m_vecAni%d", m_vecAni);

		if (t >= 1.f) {
			m_bStart = false;
			m_TimeAcc = 0;
		}
	}


	CUIPanel::OnUpdate(timeDelta);
}

void CUI_TabContainer::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CUI_TabContainer::OnRender()
{
	return CUIPanel::OnRender();
}

void CUI_TabContainer::OnClear()
{
	CUIPanel::OnClear();
}

void CUI_TabContainer::SetActiveTab(TAB tab)
{
	m_Active = tab;

	for (int i = 0; i < 32; i++)
	{
		if (m_TabContents[i])
			m_TabContents[i]->UI_InActive(); // 탭은 인엑티브를 해야하고 

		if (m_ButtonContents[i])
			m_ButtonContents[i]->ChangeState(BUTTON_STATE::NORMAL); // 버튼은 언셀렉트 
	}

	if (m_TabContents[ETOI(tab)])
		m_TabContents[ETOI(tab)]->UI_Active(); // 해당 패널 활성화

	if (m_ButtonContents[ETOI(tab)])
		m_ButtonContents[ETOI(tab)]->ChangeState(BUTTON_STATE::SELECT); // 셀렉트
}

shared_ptr<CUI_TabContainer> CUI_TabContainer::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_TabContainer> pInstance(new CUI_TabContainer(pDevice, pContext), [](CUI_TabContainer* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CUI_TabContainer::Free()
{
	for (int i = 0; i < 32; ++i)
	{
		m_TabContents[i].reset();
		m_ButtonContents[i].reset();
	}

	CUIPanel::Free();
}
