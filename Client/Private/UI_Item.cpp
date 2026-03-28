#include "UI_Item.h"
#include "ItemDB.h"
#include "GameInstance.h"
#include "DInput_Manager.h"
//#include "UIImage.h"

CUI_Item::CUI_Item(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIImage(pDevice, pContext)
{
}

CUI_Item::CUI_Item(const CUI_Item& prototype)
	:CUIImage(prototype)
{
}

void CUI_Item::HoldItem(Item_Inst HoldItem)
{
	if (HoldItem.ItemInst_ID == ID_Absence)
		return;

		m_bIsHold = true; 
		m_HoldItem = HoldItem; 
		Change_Texture(CItemDB::GetInstance()->GetItemByID(HoldItem.ItemDef_ID).pTexture);
		Set_Transparent(false);
		m_Children[0]->UI_Active();
		static_pointer_cast<CUIRenderable>(m_Children[0])->Set_Transparent(false);
	
}
void CUI_Item::ReleaseItem()
{
	m_bIsHold = false;
	/*m_HoldItem = HoldItem;*/
	Item_Inst NoInst = {};
	m_HoldItem = NoInst;
	//Change_Texture(CItemDB::GetInstance()->GetItemByID(HoldItem.ItemDef_ID).pTexture);
	Set_Transparent(true);
	m_Children[0]->UI_InActive();
	static_pointer_cast<CUIRenderable>(m_Children[0])->Set_Transparent(true);
}

HRESULT CUI_Item::OnInit(void* pArg)
{
	CUI_Item::ITEM_DESC pDesc{};

	pDesc.IsTransparent = true;

	Set_Zorder(1);


	{
	CUIImage::UIIMAGE_DESC selectImage{};
	selectImage.TextureComLevel = ETOI(LEVEL::STATIC);
	selectImage.TextureProtoName = L"Prototype_Component_Texture_Select";
	selectImage.PxSliceLRTB = _float4{ 46.f,46.f,22.f,22.f };
	selectImage.bUseNineSlice = true;

	//selectImage.bSetParentSize = true;
	shared_ptr<CUIImage> Select = CUIImage::Create(m_pDevice, m_pContext);
	Select->Initialize(&selectImage);
	Select->UI_InActive();
	Select->Set_Transparent(true);
	wstring NameTag1 = L"SELECT" ;
	Add_Child(Select, NameTag1, false);
	//
	Select->Set_Zorder(6);
	
	}

	Set_Interactive(false);
	__super::OnInit(&pDesc);


	return S_OK;
}

void CUI_Item::OnActive()
{
	__super::OnActive();

}

void CUI_Item::OnInActive()
{
	__super::OnInActive();
}

void CUI_Item::OnDisabled()
{
	__super::OnDisabled();
}

void CUI_Item::OnUpdate(const _float& timeDelta)
{

	_float2 musPos = m_pGameInstance.lock()->Get_DInput_Manger()->Get_MousePos();
	_float2 screen = m_pGameInstance.lock()->Get_WinSize().Size();
	m_pUITransformCom->SetAnchoredPos({ musPos.x-screen.x *0.5f, musPos.y-screen.y*0.5f});
	m_pUITransformCom->SetRotation(m_HoldItem.Rotation * -90.f);
	//m_Children[0]->GetUITransform()->SetRotation(m_HoldItem.Rotation * 90.f);
	_float2 sizeD = m_Children[0]->GetUITransform()->Get_SizeDelta();

	_float2 finalSize = m_pUITransformCom->Get_FinalSize();

	if (m_HoldItem.Rotation % 2 != 0) // 90도 혹은 270도일 때
	{
		swap(finalSize.x, finalSize.y);
	}
	m_Children[0]->GetUITransform()->SetLocalScale(finalSize/ sizeD);

	__super::OnUpdate(timeDelta);
}

void CUI_Item::OnLateUpdate()
{
	__super::OnLateUpdate();
}

HRESULT CUI_Item::OnRender()
{
	__super::OnRender();
	return S_OK;
}

void CUI_Item::OnClear()
{
	__super::OnClear();
}

shared_ptr<CUI_Item> CUI_Item::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_Item> pInstance(new CUI_Item(pDevice, pContext), [](CUI_Item* p) {p->Free();delete(p);});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIImage");
	}
	return pInstance;
}

void CUI_Item::Free()
{
	__super::Free();

}

