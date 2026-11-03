#include "UI_TabContainer.h"

#include <UIText.h>

#include "UIButton.h"

#include "UI_Inventory.h"
#include "UI_Storage.h"
#include "UIImage.h"
#include "EventBus.h"


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

void CUI_TabContainer::UI_PanelActive(_uint iTabfig, ETab Active)
{
	//m_Children.clear();
	_uint numPanel = {};
	// 일단 기본적으로 패널 은 다 inactive, 버튼은 iTabfig 에 따라 active
	//::OnActive() 에서 클릭한 거만 활성화
	if (iTabfig & ETOI(ETab::INVEN))
	{
		//Add_Child(m_ButtonContents[ETOI(TAB::INVEN)], L"BUTTON_INVEN", false);
		(m_buttonContents[ETOI(ETab::INVEN)]->UI_Active());
		//Add_Child(m_TabContents[ETOI(TAB::INVEN)], L"INVEN", false);
		m_tabContents[ETOI(ETab::INVEN)]->UI_InActive();
		numPanel++;
	}
	if (iTabfig & ETOI(ETab::STORAGE))
	{
		//(m_ButtonContents[ETOI(TAB::STORAGE)], L"BUTTON_STORAGE", false);
		(m_buttonContents[ETOI(ETab::STORAGE)]->UI_Active());
		//Add_Child(m_TabContents[ETOI(TAB::STORAGE)], L"STORAGE", false);
		m_tabContents[ETOI(ETab::STORAGE)]->UI_InActive();
		numPanel++;

	}
	if (iTabfig & ETOI(ETab::ETC))
	{
		//(m_ButtonContents[ETOI(TAB::ETC)], L"BUTTON_ETC", false);
		m_buttonContents[ETOI(ETab::ETC)]->UI_Active();
		//Add_Child(m_TabContents[ETOI(TAB::ETC)], L"ETC", false);
		m_tabContents[ETOI(ETab::ETC)]->UI_InActive();
		numPanel++;
	}
	m_line->UI_Active();
	m_textImg->UI_Active();

	Change_LayoutRawCol( numPanel,1);

	m_active = Active;

	Set_ActiveForCustom();

	m_isRenderReady = false;
	
	if (m_UITransformCom)
		m_UITransformCom->UpdateLayoutIfDirty();
	
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
	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_AddMoney>([this](const Evt_AddMoney& e)
		{
			
			wstring strMoney = format(L"${:.2f}", e.money);
			//wstring strM = L"$";
			this->m_textFont->Set_Text(/*strM +*/ strMoney);
		});



	HRESULT hr = E_FAIL;
	TabContainerDesc* TABpDesc = static_cast<TabContainerDesc*>(pArg);
	//TABCONTAINER_DESC pDesc = {};
	TABpDesc->isFullScreen = false;
	TABpDesc->isTransparent = false;
	TABpDesc->textureComLevel = ETOI(ELevel::STATIC);
	TABpDesc->textureProtoName = L"Prototype_Component_Texture_TabContainer";
	TABpDesc->isUseLayout = true;
	//pDesc.vAnchorPoint = 일단 패스
	//	pDesc.vPivot
	//	pDesc.vAnchoredPos

	hr = CUIPanel::OnInit(TABpDesc);


	CUIImage::UIImageDesc img = {};
	img.textureComLevel = ETOI(ELevel::STATIC);
	img.textureProtoName = L"Prototype_Component_Texture_TabDivider";
	img.useNineSlice = true;
	shared_ptr<CUIImage> Line = CUIImage::Create(m_device, m_context);
	Line->Initialize(&img);
	Line->Set_Zorder(1);

	Add_Child(Line, L"Line", false); 
	m_line = Line;

	for (_uint i = 0; i < 3; i++)
	{
		ETab eTab = static_cast<ETab>(1 << i);

		/// 버튼
		CUIButton::UIButtonDesc ButDesc = {};
		ButDesc.textureComLevel = ETOI(ELevel::STATIC);
		ButDesc.textureProtoName = L"Prototype_Component_Texture_Button_RED";
		//ButDesc.vAnchoredPos = Vector2{ 0.f,16.7f + (98.f ) };
		//ButDesc.vSizeDelta = Vector2{ 50.f,50.f };// 안건드려도 됨 텍스쳐에서 초기화 예정
		//ButDesc.vAnchorPoint = Vector2{ 0.254f,0.5f };
		//ButDesc.vPivot = Vector2{ 0.5f, 0.5f };
		//ButDesc.vScale = Vector2{ 1.3f, 1.0f };
		ButDesc.typeIndex = ETOI(eTab);
		ButDesc.OverlapStartEvent = [this](CUIButton* pThis)
		{
				m_gameInstance.lock()->Play_Once(L"Hover");
		};
		ButDesc.OverlapEndEvent = [](CUIButton* pThis) {};
		ButDesc.ClickEvent = [this](CUIButton* pThis)
			{
				SetActiveTab(static_cast<ETab>(pThis->Get_TypeIndex()));
				m_gameInstance.lock()->Play_Once(L"Click");
			};
		shared_ptr<CUIButton> button = CUIButton::Create(m_device, m_context);
		button->Initialize(&ButDesc);
		button->Set_Zorder(2);
		button->UI_InActive();

		{
			wstring name = {};
			switch (eTab)
			{
			case ETab::INVEN:
				name = L"화물";
				break;
			case ETab::STORAGE:
				name = L"창고";

				break;
			case ETab::ETC:
				name = L"기타";
				break;
			}

			CUIText::TextDesc text_Desc = {};
			text_Desc.fontTag = L"Noto_Sans_CJK_SC_24";
			text_Desc.text = name;
			shared_ptr<CUIText> Text_storage = CUIText::Create(m_device, m_context);
			Text_storage->Initialize(&text_Desc);
			button->Add_Child(Text_storage, L"Text_"+S2W(string(magic_enum::enum_name(eTab))), false);
		}

		//Add_Child(pChild, NameTag, false);
		wstring NameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(eTab)));
		Add_Layout_Child(button, NameTag, false);
		m_buttonContents[ETOI(eTab)] = button;

	}

	//for (_uint i = 0; i < 3; i++)
	{
		ETab eTab = static_cast<ETab>(1 << 0);
		// 일단 테스트 인벤 3개
		CUI_Inventory::InventoryDesc InvenDesc = {};
		InvenDesc.isFullScreen = false;
		InvenDesc.isTransparent = true;
		InvenDesc.isSetParentSize = true;

		shared_ptr<CUI_Inventory> pInven = CUI_Inventory::Create(m_device, m_context);
		pInven->Initialize(&InvenDesc);
		
		wstring NameTag = S2W(string(magic_enum::enum_name(eTab)));
		Add_Child(pInven, NameTag, false);
		m_tabContents[ETOI(eTab)] = pInven;
		pInven->UI_InActive();
	}
		{
			ETab eTab = static_cast<ETab>(1 << 1);
			// 일단 테스트 인벤 3개
			CUI_Storage::StorageDesc storageDesc = {};
			storageDesc.isFullScreen = false;
			storageDesc.isTransparent = true;
			storageDesc.isSetParentSize = true;
			storageDesc.Inventype = EInvenType::STORAGE;

			shared_ptr<CUI_Storage> pStorage = CUI_Storage::Create(m_device, m_context);
			pStorage->Initialize(&storageDesc);

			


			wstring NameTag = S2W(string(magic_enum::enum_name(eTab)));
			Add_Child(pStorage, NameTag, false);
			m_tabContents[ETOI(eTab)] = pStorage;
			pStorage->UI_InActive();
		}

		///돈///

	{
		CUIImage::UIImageDesc moneyDesc = {};
		moneyDesc.textureComLevel = ETOI(ELevel::STATIC);
		moneyDesc.textureProtoName = L"Prototype_Component_Texture_Button";
		shared_ptr<CUIImage> money = CUIImage::Create(m_device, m_context);
		money->Initialize(&moneyDesc);
		{
			CUIText::TextDesc MoneyTexDesc = {};
			MoneyTexDesc.fontTag = L"Noto_Sans_CJK_SC";
			MoneyTexDesc.text = L"$0";
			MoneyTexDesc.fontAlign = CUIText::EFontAlign::RIGHT;
			shared_ptr<CUIText> MoneyTex = CUIText::Create(m_device, m_context);
			MoneyTex->Initialize(&MoneyTexDesc);
			money->Add_Child(MoneyTex, L"MoneyTex", false);
			m_textFont = MoneyTex;
		}
		Add_Child(money, L"MONEY", false);
		m_textImg = money;
	}

	return hr;
}

void CUI_TabContainer::OnActive()
{
	m_tabContents[ETOI(m_active)]->UI_Active();
	m_buttonContents[ETOI(m_active)]->ChangeState(EButtonState::SELECT);

	//m_TabContents[ETOI(m_Active)]->GetUITransform()->SetLocalScale(GetUITransform()->Get_LocalScale());
	//m_TabContents[ETOI(m_Active)]->GetUITransform()->SetSizeDelta(GetUITransform()->Get_SizeDelta());

	m_buttonContents[ETOI(m_active)]->Set_Zorder(2);

	GetUITransform()->SetAnchoredPos(Vector2{ GetUITransform()->Get_FinalSize().x, 0.f });
	m_vecAni = Vector2{ GetUITransform()->Get_FinalSize().x, 0.f };
	m_isStart = true;
	m_duration = 1.5f;

	m_gameInstance.lock()->Play_Once(L"Pursuits_OpenIndividual");

	CUIPanel::OnActive();
}

void CUI_TabContainer::OnInActive()
{
	m_isStart = false;
	m_timeAcc = 0;
	m_active = ETab::NONE;
	CUIPanel::OnInActive();
}

void CUI_TabContainer::OnDisabled()
{

	CUIPanel::OnDisabled();
}

void CUI_TabContainer::OnUpdate(const _float& timeDelta)
{
	// 여기에서 안보였다가 와야함
	if (m_isStart == true)
	{
		m_timeAcc += timeDelta;

		float t = m_timeAcc / m_duration;

		if (t >= 1.f) t = 1.f;

		m_vecAni = Vector2{ lerp(m_vecAni.x, 0.f, t),0.f };
		GetUITransform()->SetAnchoredPos(m_vecAni);
		//LOG_F(LOG_LEVEL::INFO, "m_vecAni%d", m_vecAni);

		if (t >= 1.f) {
			m_isStart = false;
			m_timeAcc = 0;
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

void CUI_TabContainer::SetActiveTab(ETab tab)
{
	m_active = tab;

	for (int i = 0; i < 32; i++)
	{
		if (m_tabContents[i])
			m_tabContents[i]->UI_InActive(); // 탭은 인엑티브를 해야하고 

		if (m_buttonContents[i])
			m_buttonContents[i]->ChangeState(EButtonState::NORMAL); // 버튼은 언셀렉트 
	}

	if (m_tabContents[ETOI(tab)])
		m_tabContents[ETOI(tab)]->UI_Active(); // 해당 패널 활성화

	if (m_buttonContents[ETOI(tab)])
		m_buttonContents[ETOI(tab)]->ChangeState(EButtonState::SELECT); // 셀렉트
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
		m_tabContents[i].reset();
		m_buttonContents[i].reset();
	}

	CUIPanel::Free();
}
