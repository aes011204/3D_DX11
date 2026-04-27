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

	for(auto Button: m_ButtonContents)
	{
		Button->UI_Active();
	}
	for (auto content : m_TabContents)
	{
		content->UI_InActive();
		//dynamic_pointer_cast<CUI_Storage>(content)->Set_SlotNum(8, 8);
	}
	m_TabContents[ETOI(SHOPTAB::ROT)]->UI_Active();
	
	m_Line->UI_Active();
	m_Line2->UI_Active();
//	m_TextIMG->UI_Active();
	m_BtnFix->UI_Active();
	m_FixButtonTex->UI_Active();
	/*_wstring strTag = format(L"모두 수리[{:.2f}]", m_repairPrice);
	m_FixButtonTex->Set_Text(strTag);*/
	Change_LayoutRawCol( 4,1);

	//m_Active = Active;

	Set_ActiveForCustom();
	

	m_bRenderReady = false;
	
	if (m_pUITransformCom)
		m_pUITransformCom->UpdateLayoutIfDirty();
	
	//m_bInteractable = true;
	OnActive();

}

HRESULT CUI_RepairShop::OnInit(void* pArg)
{
	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_RepairCoat>([this](const Evt_RepairCoat& e)
		{
			_wstring strTag = format(L"모두 수리[${}]", e.cost);
			m_FixButtonTex->Set_Text(strTag);
			m_repairPrice = e.cost;
		});


	HRESULT hr = E_FAIL;
	REPAIR_DESC* TABpDesc = static_cast<REPAIR_DESC*>(pArg);
	//TABCONTAINER_DESC pDesc = {};
	TABpDesc->IsFullScreen = false;
	TABpDesc->IsTransparent = false;
	TABpDesc->TextureComLevel = ETOI(LEVEL::STATIC);
	TABpDesc->TextureProtoName = L"Prototype_Component_Texture_TabContainer";
	TABpDesc->IsUseLayout = true;
	//pDesc.vAnchorPoint = 일단 패스
	//	pDesc.vPivot
	//	pDesc.vAnchoredPos

	hr = CUIPanel::OnInit(TABpDesc);


	CUIImage::UIIMAGE_DESC img = {};
	img.TextureComLevel = ETOI(LEVEL::STATIC);
	img.TextureProtoName = L"Prototype_Component_Texture_TabDivider";
	img.bUseNineSlice = true;
	shared_ptr<CUIImage> Line = CUIImage::Create(m_pDevice, m_pContext);
	Line->Initialize(&img);
	Line->Set_Zorder(1);

	Add_Child(Line, L"Line", false); 
	m_Line = Line;

	CUIImage::UIIMAGE_DESC img2 = {};
	img2.TextureComLevel = ETOI(LEVEL::STATIC);
	img2.TextureProtoName = L"Prototype_Component_Texture_TabDivider";
	img2.bUseNineSlice = true;
	shared_ptr<CUIImage> Line2 = CUIImage::Create(m_pDevice, m_pContext);
	Line2->Initialize(&img2);
	Line2->Set_Zorder(1);

	Add_Child(Line2, L"Line2", false);
	m_Line2 = Line2;

	for (_uint i = 0; i < ETOI(SHOPTAB::END); i++)
	{
		SHOPTAB eTab = static_cast<SHOPTAB>(i);

		/// 버튼
		CUIButton::UIBUTTON_DESC ButDesc = {};
		ButDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		ButDesc.TextureProtoName = L"Prototype_Component_Texture_Button_RED";

		ButDesc.vScale = Vector2{ 3.28f, 1.25f };
		ButDesc.TypeIndex = ETOI(eTab);
		ButDesc.OverlapStartEvent = [this](CUIButton* pThis)
		{
				m_pGameInstance.lock()->Play_Once(L"Hover");

		};
		ButDesc.OverlapEndEvent = [](CUIButton* pThis) {};
		ButDesc.ClickEvent = [this,i](CUIButton* pThis)
			{
				SetActiveTab(i);
				m_pGameInstance.lock()->Play_Once(L"Click");

			};
		shared_ptr<CUIButton> button = CUIButton::Create(m_pDevice, m_pContext);
		button->Initialize(&ButDesc);
		button->Set_Zorder(2);
		button->UI_InActive();

		{
		
			wstring texKey = {};
			switch (eTab)
			{
			case SHOPTAB::ROT:
			
				texKey = L"Prototype_Component_Texture_Shop_Rod";
				break;
			case SHOPTAB::ENGINE:
				
				texKey=L"Prototype_Component_Texture_Shop_Engine";
				break;
			case SHOPTAB::NET:
				
				texKey = L"Prototype_Component_Texture_Shop_Net";
				break;
			case SHOPTAB::LIGHT:
				
				texKey = L"Prototype_Component_Texture_Shop_Light";
				break;
			}

			CUIImage::UIIMAGE_DESC icon{};
			icon.TextureComLevel = ETOI(LEVEL::STATIC);
			icon.TextureProtoName = texKey;
			shared_ptr<CUIImage> iconImg = CUIImage::Create(m_pDevice, m_pContext);
			iconImg->Initialize(&icon);

			wstring NameTag = L"iconImg" + S2W(string(magic_enum::enum_name(eTab)));
			button->Add_Child(iconImg, NameTag, false);
			
		}

		//Add_Child(pChild, NameTag, false);
		wstring NameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(eTab)));
		Add_Layout_Child(button, NameTag, false);
		m_ButtonContents[ETOI(eTab)] = button;

	}

	for (_uint i = 0; i < ETOI(SHOPTAB::END); i++)
	{
		{
			SHOPTAB eTab = static_cast<SHOPTAB>(i);
			// 일단 테스트 인벤 3개
			CUI_Storage::STORAGE_DESC StorageDesc = {};
			StorageDesc.IsFullScreen = false;
			StorageDesc.IsTransparent = true;
			StorageDesc.bSetParentSize = true;
			StorageDesc.Inventype = INVENTYPE::SHOP;
			StorageDesc.shopTab = eTab;
			

			shared_ptr<CUI_Storage> pStorage = CUI_Storage::Create(m_pDevice, m_pContext);
			pStorage->Initialize(&StorageDesc);



			wstring NameTag = S2W(string(magic_enum::enum_name(eTab)));
			Add_Child(pStorage, NameTag, false);
			m_TabContents[ETOI(eTab)] = pStorage;
			pStorage->UI_InActive();


			Change_LayoutRawCol(8,8);
		}
	}
		

		///돈///

	{
			CUIButton::UIBUTTON_DESC ButDesc = {};
			ButDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			ButDesc.TextureProtoName = L"Prototype_Component_Texture_Button_RED";
			//ButDesc.TypeIndex = ETOI(eTab);
			ButDesc.OverlapStartEvent = [](CUIButton* pThis) {};
			ButDesc.OverlapEndEvent = [](CUIButton* pThis) {};
			ButDesc.ClickEvent = [this](CUIButton* pThis)
				{
					// 전체수리

					Evt_FixAll event{};
					
					CGameInstance::GetInstance()->Get_EventBus()->Publish(event);
				};
			shared_ptr<CUIButton> button = CUIButton::Create(m_pDevice, m_pContext);
			button->Initialize(&ButDesc);
			button->Set_Zorder(2);
			button->UI_InActive();
			button->Set_SelectState(false);

			{
			

				CUIText::TEXT_DESC text_Desc = {};
				text_Desc.strFontTag = L"Noto_Sans_CJK_SC_24";
				text_Desc.strText = L"테스트";
				shared_ptr<CUIText> Text_storage = CUIText::Create(m_pDevice, m_pContext);
				Text_storage->Initialize(&text_Desc);
				button->Add_Child(Text_storage, L"Text_repairPrice", false);
				m_FixButtonTex = Text_storage;
			}

			//Add_Child(pChild, NameTag, false);
			wstring NameTag = L"BUTTON_repair";
			Add_Child(button, NameTag, false);
			m_BtnFix = button;
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
	m_bStart = true;
	m_fDuration = 1.5f;

	m_ButtonContents[ETOI(SHOPTAB::ROT)]->ChangeState(BUTTON_STATE::SELECT);


	CUIPanel::OnActive();
}

void CUI_RepairShop::OnInActive()
{
	m_bStart = false;
	m_TimeAcc = 0;
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
	if (m_bStart == true)
	{
		m_TimeAcc += timeDelta;

		float t = m_TimeAcc / m_fDuration;

		if (t >= 1.f) t = 1.f;
		m_vecAni = Vector2{ lerp(m_vecAni.x,  -600.f, t),0.f };
		GetUITransform()->SetAnchoredPos(m_vecAni);
		//LOG_F(LOG_LEVEL::INFO, "m_vecAni%d", m_vecAni);

		if (t >= 1.f) {
			m_bStart = false;
			m_TimeAcc = 0;
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
	

	for (int i = 0; i < ETOI(SHOPTAB::END); i++)
	{
		if (m_TabContents[i])
			m_TabContents[i]->UI_InActive(); // 탭은 인엑티브를 해야하고 

		if (m_ButtonContents[i])
			m_ButtonContents[i]->ChangeState(BUTTON_STATE::NORMAL); // 버튼은 언셀렉트 
	}

	if (m_TabContents[ETOI(btnIndex)])
		m_TabContents[ETOI(btnIndex)]->UI_Active(); // 해당 패널 활성화

	if (m_ButtonContents[ETOI(btnIndex)])
		m_ButtonContents[ETOI(btnIndex)]->ChangeState(BUTTON_STATE::SELECT); // 셀렉트
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
	for (int i = 0; i < ETOI(SHOPTAB::END); ++i)
	{
		m_TabContents[i].reset();
		m_ButtonContents[i].reset();
	}

	CUIPanel::Free();
}
