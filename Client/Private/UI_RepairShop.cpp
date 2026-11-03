#include "UI_RepairShop.h"

#include <UIText.h>

#include "UIButton.h"

#include "UI_Inventory.h"
#include "UI_Storage.h"
#include "UIImage.h"
#include "EventBus.h"


CUI_RepairShop::CUI_RepairShop(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CUI_RepairShop::CUI_RepairShop(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

void CUI_RepairShop::UI_Active()
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

void CUI_RepairShop::UI_PanelActive()
{
	//m_Children.clear();
	_uint numPanel = {};
	// 일단 기본적으로 패널 은 다 inactive, 버튼은 iTabfig 에 따라 active
	//::OnActive() 에서 클릭한 거만 활성화

	for(auto Button: m_buttonContents)
	{
		Button->UI_Active();
	}
	for (auto content : m_tabContents)
	{
		content->UI_InActive();
		//dynamic_pointer_cast<CUI_Storage>(content)->Set_SlotNum(8, 8);
	}
	m_tabContents[ETOI(EShopTab::ROT)]->UI_Active();
	
	m_line->UI_Active();
	m_line2->UI_Active();
//	m_TextIMG->UI_Active();
	m_btnFix->UI_Active();
	m_fixButtonTex->UI_Active();
	/*_wstring strTag = format(L"모두 수리[{:.2f}]", m_repairPrice);
	m_FixButtonTex->Set_Text(strTag);*/
	Change_LayoutRawCol( 4,1);

	//m_Active = Active;

	Set_ActiveForCustom();
	

	m_isRenderReady = false;
	
	if (m_UITransformCom)
		m_UITransformCom->UpdateLayoutIfDirty();
	
	//m_bInteractable = true;
	OnActive();

}

HRESULT CUI_RepairShop::OnInit(void* pArg)
{
	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_RepairCost>([this](const Evt_RepairCost& e)
		{
			_wstring strTag = format(L"모두 수리[${}]", e.cost);
			m_fixButtonTex->Set_Text(strTag);
			m_repairPrice = e.cost;
		});


	HRESULT hr = E_FAIL;
	RepairDesc* TABpDesc = static_cast<RepairDesc*>(pArg);
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

	CUIImage::UIImageDesc img2 = {};
	img2.textureComLevel = ETOI(ELevel::STATIC);
	img2.textureProtoName = L"Prototype_Component_Texture_TabDivider";
	img2.useNineSlice = true;
	shared_ptr<CUIImage> Line2 = CUIImage::Create(m_device, m_context);
	Line2->Initialize(&img2);
	Line2->Set_Zorder(1);

	Add_Child(Line2, L"Line2", false);
	m_line2 = Line2;

	for (_uint i = 0; i < ETOI(EShopTab::END); i++)
	{
		EShopTab eTab = static_cast<EShopTab>(i);

		/// 버튼
		CUIButton::UIButtonDesc ButDesc = {};
		ButDesc.textureComLevel = ETOI(ELevel::STATIC);
		ButDesc.textureProtoName = L"Prototype_Component_Texture_Button_RED";

		ButDesc.scale = Vector2{ 3.28f, 1.25f };
		ButDesc.typeIndex = ETOI(eTab);
		ButDesc.OverlapStartEvent = [this](CUIButton* pThis)
		{
				m_gameInstance.lock()->Play_Once(L"Hover");

		};
		ButDesc.OverlapEndEvent = [](CUIButton* pThis) {};
		ButDesc.ClickEvent = [this,i](CUIButton* pThis)
			{
				SetActiveTab(i);
				m_gameInstance.lock()->Play_Once(L"Click");

			};
		shared_ptr<CUIButton> button = CUIButton::Create(m_device, m_context);
		button->Initialize(&ButDesc);
		button->Set_Zorder(2);
		button->UI_InActive();

		{
		
			wstring texKey = {};
			switch (eTab)
			{
			case EShopTab::ROT:
			
				texKey = L"Prototype_Component_Texture_Shop_Rod";
				break;
			case EShopTab::ENGINE:
				
				texKey=L"Prototype_Component_Texture_Shop_Engine";
				break;
			case EShopTab::NET:
				
				texKey = L"Prototype_Component_Texture_Shop_Net";
				break;
			case EShopTab::LIGHT:
				
				texKey = L"Prototype_Component_Texture_Shop_Light";
				break;
			}

			CUIImage::UIImageDesc icon{};
			icon.textureComLevel = ETOI(ELevel::STATIC);
			icon.textureProtoName = texKey;
			shared_ptr<CUIImage> iconImg = CUIImage::Create(m_device, m_context);
			iconImg->Initialize(&icon);

			wstring NameTag = L"iconImg" + S2W(string(magic_enum::enum_name(eTab)));
			button->Add_Child(iconImg, NameTag, false);
			
		}

		//Add_Child(pChild, NameTag, false);
		wstring NameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(eTab)));
		Add_Layout_Child(button, NameTag, false);
		m_buttonContents[ETOI(eTab)] = button;

	}

	for (_uint i = 0; i < ETOI(EShopTab::END); i++)
	{
		{
			EShopTab eTab = static_cast<EShopTab>(i);
			// 일단 테스트 인벤 3개
			CUI_Storage::StorageDesc storageDesc = {};
			storageDesc.isFullScreen = false;
			storageDesc.isTransparent = true;
			storageDesc.isSetParentSize = true;
			storageDesc.Inventype = EInvenType::SHOP;
			storageDesc.shopTab = eTab;
			

			shared_ptr<CUI_Storage> pStorage = CUI_Storage::Create(m_device, m_context);
			pStorage->Initialize(&storageDesc);



			wstring NameTag = S2W(string(magic_enum::enum_name(eTab)));
			Add_Child(pStorage, NameTag, false);
			m_tabContents[ETOI(eTab)] = pStorage;
			pStorage->UI_InActive();


			Change_LayoutRawCol(8,8);
		}
	}
		

		///돈///

	{
			CUIButton::UIButtonDesc ButDesc = {};
			ButDesc.textureComLevel = ETOI(ELevel::STATIC);
			ButDesc.textureProtoName = L"Prototype_Component_Texture_Button_RED";
			//ButDesc.TypeIndex = ETOI(eTab);
			ButDesc.OverlapStartEvent = [](CUIButton* pThis) {};
			ButDesc.OverlapEndEvent = [](CUIButton* pThis) {};
			ButDesc.ClickEvent = [this](CUIButton* pThis)
				{
					// 전체수리

					Evt_FixAll event{};
					
					CGameInstance::GetInstance()->Get_EventBus()->Publish(event);
				};
			shared_ptr<CUIButton> button = CUIButton::Create(m_device, m_context);
			button->Initialize(&ButDesc);
			button->Set_Zorder(2);
			button->UI_InActive();
			button->Set_SelectState(false);

			{
			

				CUIText::TextDesc text_Desc = {};
				text_Desc.fontTag = L"Noto_Sans_CJK_SC_24";
				text_Desc.text = L"테스트";
				shared_ptr<CUIText> Text_storage = CUIText::Create(m_device, m_context);
				Text_storage->Initialize(&text_Desc);
				button->Add_Child(Text_storage, L"Text_repairPrice", false);
				m_fixButtonTex = Text_storage;
			}

			//Add_Child(pChild, NameTag, false);
			wstring NameTag = L"BUTTON_repair";
			Add_Child(button, NameTag, false);
			m_btnFix = button;
	}

	return hr;
}

void CUI_RepairShop::OnActive()
{
	//m_TabContents[ETOI(m_Active)]->UI_Active();
	//m_ButtonContents[ETOI(m_Active)]->ChangeState(BUTTON_STATE::SELECT);

	//m_TabContents[ETOI(m_Active)]->GetUITransform()->SetLocalScale(GetUITransform()->Get_LocalScale());
	//m_TabContents[ETOI(m_Active)]->GetUITransform()->SetSizeDelta(GetUITransform()->Get_SizeDelta());

	//m_ButtonContents[ETOI(m_Active)]->Set_Zorder(2);

	GetUITransform()->SetAnchoredPos(Vector2{ GetUITransform()->Get_FinalSize().x, 0.f });
	//m_vecAni = Vector2{ GetUITransform()->Get_FinalSize().x, 0.f };
	m_vecAni = Vector2{ -1200.f,0.f };
	m_isStart = true;
	m_duration = 1.5f;

	m_buttonContents[ETOI(EShopTab::ROT)]->ChangeState(EButtonState::SELECT);


	CUIPanel::OnActive();
}

void CUI_RepairShop::OnInActive()
{
	m_isStart = false;
	m_timeAcc = 0;
	//m_Active = TAB::NONE;
	CUIPanel::OnInActive();
}

void CUI_RepairShop::OnDisabled()
{

	CUIPanel::OnDisabled();
}

void CUI_RepairShop::OnUpdate(const _float& timeDelta)
{
	// 여기에서 안보였다가 와야함
	if (m_isStart == true)
	{
		m_timeAcc += timeDelta;

		float t = m_timeAcc / m_duration;

		if (t >= 1.f) t = 1.f;
		m_vecAni = Vector2{ lerp(m_vecAni.x,  -600.f, t),0.f };
		GetUITransform()->SetAnchoredPos(m_vecAni);
		//LOG_F(LOG_LEVEL::INFO, "m_vecAni%d", m_vecAni);

		if (t >= 1.f) {
			m_isStart = false;
			m_timeAcc = 0;
		}
	}

	

	CUIPanel::OnUpdate(timeDelta);
}

void CUI_RepairShop::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CUI_RepairShop::OnRender()
{
	return CUIPanel::OnRender();
}

void CUI_RepairShop::OnClear()
{
	CUIPanel::OnClear();
}

void CUI_RepairShop::SetActiveTab(_uint btnIndex)
{
	

	for (int i = 0; i < ETOI(EShopTab::END); i++)
	{
		if (m_tabContents[i])
			m_tabContents[i]->UI_InActive(); // 탭은 인엑티브를 해야하고 

		if (m_buttonContents[i])
			m_buttonContents[i]->ChangeState(EButtonState::NORMAL); // 버튼은 언셀렉트 
	}

	if (m_tabContents[ETOI(btnIndex)])
		m_tabContents[ETOI(btnIndex)]->UI_Active(); // 해당 패널 활성화

	if (m_buttonContents[ETOI(btnIndex)])
		m_buttonContents[ETOI(btnIndex)]->ChangeState(EButtonState::SELECT); // 셀렉트
}

shared_ptr<CUI_RepairShop> CUI_RepairShop::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_RepairShop> pInstance(new CUI_RepairShop(pDevice, pContext), [](CUI_RepairShop* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CUI_RepairShop::Free()
{
	for (int i = 0; i < ETOI(EShopTab::END); ++i)
	{
		m_tabContents[i].reset();
		m_buttonContents[i].reset();
	}

	CUIPanel::Free();
}
