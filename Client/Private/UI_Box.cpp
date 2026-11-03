#include "UI_Box.h"

#include <UIText.h>

#include "UIButton.h"

#include "UI_Inventory.h"
#include "UI_Storage.h"
#include "UIImage.h"
#include "EventBus.h"


CUI_Box::CUI_Box(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CUIPanel(device, context)
{
}

CUI_Box::CUI_Box(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

void CUI_Box::UI_Active()
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

void CUI_Box::UI_PanelActive(_bool isStorage, _bool isBox, _uint boxNum )
{
	if(isStorage == true)
	{
		m_nameText->Set_Text(L"내 물품 보관함");
		m_storage->UI_Active();
		m_UITransformCom->SetLocalScale(_float2(2.56f, 3.21f));
	}
	else
	{
		m_nameText->Set_Text(L"발견한 아이템");
		m_box->UI_Active();
		m_box->Get_InvenPanel()->Set_Transparent(true);
		m_UITransformCom->SetLocalScale(_float2(2.56f, 1.7f));

	}


	//m_Children.clear();
//	_uint numPanel = {};
	// 일단 기본적으로 패널 은 다 inactive, 버튼은 iTabfig 에 따라 active
	//::OnActive() 에서 클릭한 거만 활성화
	m_nameBase->UI_Active();
	m_nameText->UI_Active();
	/*m_Storage->UI_Active();*/



	Set_ActiveForCustom();

	m_isRenderReady = false;

	if (m_UITransformCom)
		m_UITransformCom->UpdateLayoutIfDirty();

	//m_bInteractable = true;

	//for (auto& it : m_Children)
	//{
	//	it->UI_Active();
	//}
	OnActive();

}

HRESULT CUI_Box::OnInit(void* arg)
{




	HRESULT hr = E_FAIL;
	BoxDesc* desc = static_cast<BoxDesc*>(arg);
	//TABCONTAINER_DESC pDesc = {};
	desc->isFullScreen = false;
	desc->isTransparent = false;
	desc->textureComLevel = ETOI(ELevel::STATIC);
	desc->textureProtoName = L"Prototype_Component_Texture_TabContainer";
	//BoxDesc->IsUseLayout = true;
	//pDesc.vAnchorPoint = 일단 패스
	//	pDesc.vPivot
	//	pDesc.vAnchoredPos

	hr = CUIPanel::OnInit(desc);


	CUIImage::UIImageDesc nameBaseDesc = {};
	nameBaseDesc.textureComLevel = ETOI(ELevel::STATIC);
	nameBaseDesc.textureProtoName = L"Prototype_Component_Texture_TitleBackground";
	shared_ptr<CUIImage> nameBase = CUIImage::Create(m_device, m_context);
	nameBase->Initialize(&nameBaseDesc);
	Add_Child(nameBase, L"NameBase", false);
	m_nameBase = nameBase;
	{
		CUIText::TextDesc nameDesc = {};
		nameDesc.fontTag = L"Noto_Sans_CJK_SC_24";
		nameDesc.text = L"내 물품 보관함";
		shared_ptr<CUIText> nameText = CUIText::Create(m_device, m_context);
		nameText->Initialize(&nameDesc);

		nameBase->Add_Child(nameText, L"Name_Text", false);
		m_nameText = nameText;
	}
	


	

		// 인벤
		CUI_Storage::StorageDesc storageDesc = {};
		storageDesc.isFullScreen = false;
		storageDesc.isTransparent = true;
		storageDesc.isSetParentSize = true;
		storageDesc.invenType = EInvenType::STORAGE;

		shared_ptr<CUI_Storage> storage = CUI_Storage::Create(m_device, m_context);
		storage->Initialize(&storageDesc);


		Add_Child(storage, L"Storage2", false);
		m_storage = storage;
		storage->UI_InActive();
	

		//////////////////////////////////////////

		// 인벤
		CUI_Storage::StorageDesc boxDesc = {};
		boxDesc.isFullScreen = false;
		boxDesc.isTransparent = true;
		boxDesc.isSetParentSize = true;
		boxDesc.invenType = EInvenType::CHEST;
	
		shared_ptr<CUI_Storage> box = CUI_Storage::Create(m_device, m_context);
		box->Initialize(&boxDesc);


		Add_Child(box, L"Box", false);
		m_box = box;
		box->UI_InActive();

		GetUITransform()->SetAnchoredPos(Vector2{ -GetUITransform()->Get_FinalSize().x, 0.f });

	return hr;
}

void CUI_Box::OnActive()
{
	

	GetUITransform()->SetAnchoredPos(Vector2{- GetUITransform()->Get_FinalSize().x, 0.f });
	m_vecAni = Vector2{ -1200.f, 0.f };
	m_isStart = true;
	m_duration = 1.5f;

	CUIPanel::OnActive();
}

void CUI_Box::OnInActive()
{
	m_isStart = false;
	m_timeAcc = 0;
	
	CUIPanel::OnInActive();
}

void CUI_Box::OnDisabled()
{

	CUIPanel::OnDisabled();
}

void CUI_Box::OnUpdate(const _float& timeDelta)
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

void CUI_Box::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CUI_Box::OnRender()
{
	return CUIPanel::OnRender();
}

void CUI_Box::OnClear()
{
	CUIPanel::OnClear();
}



shared_ptr<CUI_Box> CUI_Box::Create(ComPtr<ID3D11Device> device,
	ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUI_Box> instance(new CUI_Box(device, context), [](CUI_Box* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return instance;
}

void CUI_Box::Free()
{
	/*for (int i = 0; i < 32; ++i)
	{
		m_TabContents[i].reset();
		
	}*/

	CUIPanel::Free();
}
