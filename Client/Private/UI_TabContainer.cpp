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
	UI_Clear();

	if(iTabfig & ETOI(TAB::INVEN))
	{
		Add_Child(m_ButtonContents[ETOI(TAB::INVEN)], L"BUTTON_INVEN", false);
		(m_ButtonContents[ETOI(TAB::INVEN)]->UI_Active());

		Add_Child(m_TabContents[ETOI(TAB::INVEN)], L"INVEN", false);
		m_TabContents[ETOI(TAB::INVEN)]->UI_InActive();

	}
	if (iTabfig & ETOI(TAB::STORAGE))
	{
		Add_Child(m_ButtonContents[ETOI(TAB::STORAGE)], L"BUTTON_STORAGE", false);
		(m_ButtonContents[ETOI(TAB::STORAGE)]->UI_Active());

		Add_Child(m_TabContents[ETOI(TAB::STORAGE)], L"STORAGE", false);
		m_TabContents[ETOI(TAB::STORAGE)]->UI_InActive();
	}
	if (iTabfig & ETOI(TAB::ETC))
	{
		Add_Child(m_ButtonContents[ETOI(TAB::ETC)], L"BUTTON_ETC", false);
		m_ButtonContents[ETOI(TAB::ETC)]->UI_Active();

		Add_Child(m_TabContents[ETOI(TAB::ETC)], L"ETC", false);
		m_TabContents[ETOI(TAB::ETC)]->UI_InActive();

	}

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

	TABCONTAINER_DESC* TABpDesc = static_cast<TABCONTAINER_DESC*>(pArg);
	//TABCONTAINER_DESC pDesc = {};
	TABpDesc->IsFullScreen = false;
	TABpDesc->IsTrnasparent = false;
	TABpDesc->TextureComLevel = ETOI(LEVEL::STATIC);
	TABpDesc->TextureProtoName = L"Prototype_Component_Texture_TabContainer";
	//pDesc.vAnchorPoint = 일단 패스
	//	pDesc.vPivot
	//	pDesc.vAnchoredPos

	for (_uint i = 0; i < 3; i++)
	{
		TAB eTab = static_cast<TAB>(1<<i);


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
		ButDesc.ClickEvent = [](CUIButton* pThis)
		{
			
		};
		shared_ptr<CUIButton> pChild = CUIButton::Create(m_pDevice, m_pContext);
		pChild->Initialize(&ButDesc);

		//wstring NameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(eTab)));

		//Add_Child(pChild, NameTag, false);
		m_ButtonContents[ETOI(eTab)] = pChild;

		pChild->Set_Zorder(2);
	}
	for (_uint i = 0; i < 3; i++)
	{
		TAB eTab = static_cast<TAB>(1 << i);
		// 일단 테스트 인벤 3개

		CUI_Inventory::INVENTORY_DESC InvenDesc = {};
		InvenDesc.IsFullScreen = false;
		InvenDesc.IsUseLayout = true;
		InvenDesc.IsTrnasparent = true;


		shared_ptr<CUI_Inventory> pInven = CUI_Inventory::Create(m_pDevice, m_pContext);
		pInven->Initialize(&InvenDesc);
		wstring Tag = L"INVENTORY";
		//(pInven, L"INVENTORY", false);
		m_TabContents[ETOI(eTab)] = pInven;
	}



	return CUIPanel::OnInit(TABpDesc);
}

void CUI_TabContainer::OnActive()
{
	m_TabContents[ETOI(m_Active)]->UI_Active();
	
	CUIPanel::OnActive();
}

void CUI_TabContainer::OnInActive()
{
	CUIPanel::OnInActive();
}

void CUI_TabContainer::OnDisabled()
{

	CUIPanel::OnDisabled();
}

void CUI_TabContainer::OnUpdate(const _float& timeDelta)
{
	

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
