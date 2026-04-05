#include "ItemInfo.h"

#include <UIImage.h>

#include "ItemDB.h"
#include "UIText.h"
#include "EventBus.h"

CItemInfo::CItemInfo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CItemInfo::CItemInfo(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}

void CItemInfo::UI_PanelActive(_bool isHold, Item_Inst itemInst, LOCATIONSTATE locationState)
{
	wstring NameInfo = L"";
	wstring LeftInfo = L"";
	wstring RightInfo = L"";
	wstring DescInfo = L"";
	//wstring KeyInfo = L"";
	


	if (itemInst.ItemInst_ID == ID_Absence)
	{
		
		UI_InActive();
		return;
	}

	float cost = {};

	float fCurrentY = -m_NamePadding;
	
	Item_Def itemDef = CItemDB::GetInstance()->GetItemByID(itemInst.ItemDef_ID);
	switch (itemInst.ItemType)
	{
	case ITEM_TYPE::FISH:

		if (Fish_Inst* FishInst = get_if<Fish_Inst>(&itemInst.TypeDef))
		{
			Fish_Def* FishDef = get_if<Fish_Def>(&itemDef.TypeDef);

			if (FishInst->mutation_ID == ID_Absence)
			{
				NameInfo = S2W(itemDef.ItemName);
				DescInfo = S2W(itemDef.ItemDesc);
				cost = FishDef->Cost;
			}
			else
			{
				NameInfo = S2W(FishDef->vec_Mutation[FishInst->mutation_ID].MutName);
				DescInfo = S2W(FishDef->vec_Mutation[FishInst->mutation_ID].MutDesc);
				cost = FishDef->vec_Mutation[FishInst->mutation_ID].MutCost;
			}

			if (isHold == true)
				break;
			LeftInfo = L"크기:\n상태:\n유형:";
			RightInfo = format(L"{:.2f}cm\n{}\n{}", FishInst->size, freshToWstr(FishInst->freshness), OcceanToWstr(FishDef->SeaType_Mask));
		}
		break;
	case ITEM_TYPE::EQUIP:
		Equip_Inst* EquipInst = get_if<Equip_Inst>(&itemInst.TypeDef);
		Equip_Def* EquipDef = get_if<Equip_Def>(&itemDef.TypeDef);

		NameInfo = S2W(itemDef.ItemName);

		if (isHold == true)
			break;

		if (Equip_Engine* EquipEngine = get_if<Equip_Engine>(&EquipDef->EquipType_Effect))
		{
			LeftInfo = L"설치 시간:\n상태:\n속도:";
			RightInfo = format(L"{}h\n{}\n+{}kn", EquipDef->InstallTime, GetBroken(EquipInst->IsBroken), EquipEngine->Speed_Kn);
		}
		else if (Equip_Light* EquipLight = get_if<Equip_Light>(&EquipDef->EquipType_Effect))
		{
			LeftInfo = L"설치 시간:\n상태:\n루멘:\n범위";
			RightInfo = format(L"{}h\n{}\n+{}lm\n{}m", EquipDef->InstallTime, GetBroken(EquipInst->IsBroken), EquipLight->Lumen, EquipLight->Range);
		}
		else if (Equip_Rod* EquipRod = get_if<Equip_Rod>(&EquipDef->EquipType_Effect))
		{
			LeftInfo = L"설치 시간:\n상태:\n낚시 속도:\n어종";
			RightInfo = format(L"{}h\n{}\n+{}%\n{}", EquipDef->InstallTime, GetBroken(EquipInst->IsBroken), EquipRod->Speed_Per, OcceanToWstr(EquipRod->SeaType_Mask));
		}
		else
		{
			//넷트랑 게통발은 패스 그거 까지 할시간 없을듯 후에 시간 남으면 추가
		}
		DescInfo = S2W(itemDef.ItemDesc);
		cost = EquipDef->Cost;

		break;
	}
	m_NameText->Set_Text(NameInfo); //위치조정 필요 없음
	m_NameText->UI_Active();
	m_NameText->GetUITransform()->SetAnchoredPos({  m_PaddingX, fCurrentY });
	fCurrentY -= (m_NameText->Get_TextSize().y + m_NamePadding);

	m_LineImg->UI_Active();
	m_LineImg->GetUITransform()->SetAnchoredPos({ 0, fCurrentY });
	fCurrentY -= (m_LineImg->GetUITransform()->Get_SizeDelta().y + m_PaddingY);

	m_LeftText->Set_Text(LeftInfo);
	m_LeftText->GetUITransform()->SetAnchoredPos({  m_PaddingX, fCurrentY });
	m_LeftText->UI_Active();

	m_RightText->Set_Text(RightInfo);
	m_RightText->GetUITransform()->SetAnchoredPos({  -m_PaddingX, fCurrentY });
	m_RightText->UI_Active();
	fCurrentY -= (m_LeftText->Get_TextSize().y + m_PaddingY);
	

	m_DetailText->Set_Text(DescInfo);
	m_DetailText->GetUITransform()->SetAnchoredPos({ -m_PaddingX, fCurrentY });
	m_DetailText->UI_Active();
	fCurrentY -= (m_DetailText->Get_TextSize().y + m_PaddingY);

	for(auto& infoBtn:m_vecIcon)
	{
		if (infoBtn->Get_UIState() == UI_STATE::ACTIVE)
			infoBtn->UI_InActive();
	}
	float sizeY = {};
	if (isHold == true)
	{

		switch (locationState)
		{
		case LOCATIONSTATE::FISHING:
		case LOCATIONSTATE::SEA:
			// 놓기 회전 버리기
			sizeY = Active_ButtonInfo(BUTTONINFO::RELEASE, { 40.f ,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::ROTATION, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::THROWUP, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			break;
		case LOCATIONSTATE::SHOP_INVEN:
			//설치 회전 창고로보내기 환불
			sizeY = Active_ButtonInfo(BUTTONINFO::RELEASE, { 40.f,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::ROTATION, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);


			sizeY = Active_ButtonInfo(BUTTONINFO::STORAGE, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::SELL, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			break;
		}
	}
	else
	{
		
		switch (locationState)
		{
		case LOCATIONSTATE::FISHING:
		case LOCATIONSTATE::SEA:
			//집기 버리기
			sizeY = Active_ButtonInfo(BUTTONINFO::PICK, { 50.f ,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::THROWUP, { 50.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);
			break;
		case LOCATIONSTATE::SHOP:
			//구매
			sizeY = Active_ButtonInfo(BUTTONINFO::BUY, { 50.f ,fCurrentY }, cost);
			fCurrentY -= (sizeY + m_PaddingY);

		break;
		case LOCATIONSTATE::SHOP_INVEN:
			//집기 버리기 창고로보내기 판매
			sizeY = Active_ButtonInfo(BUTTONINFO::PICK, { 50.f ,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::THROWUP, { 50.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);
			sizeY = Active_ButtonInfo(BUTTONINFO::STORAGE, { 50.f ,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::SELL, { 50.f, fCurrentY }, cost * 0.8f);
			fCurrentY -= (sizeY + m_PaddingY);
			break;
		}
	}
	//_float Ysize = 0;
	/*for (size_t i = 0; i < m_vecIcon.size(); i++)
	{
		if (m_vecIcon[i]->Get_UIState() == UI_STATE::ACTIVE)
		{
			Ysize = max(Ysize, m_vecIcon[i]->GetUITransform()->Get_WorldRect().Bottom());
		
		}
	}*/
	m_Targetsize.y = abs(fCurrentY) + m_PaddingY;
	//m_Targetsize.y = rc.Bottom() - GetUITransform()->Get_WorldRect().Top() ;


	Set_ActiveForCustom();

	m_bRenderReady = false;

	if (m_pUITransformCom)
		m_pUITransformCom->UpdateLayoutIfDirty();

	//m_bInteractable = true;
	OnActive();

	
}

void CItemInfo::UI_Active()
{
	CUIPanel::UI_Active();
}

HRESULT CItemInfo::OnInit(void* pArg)
{
	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_ToolTip>([this](const Evt_ToolTip& e)
		{
		if(e.locationState == LOCATIONSTATE::END)
			UI_InActive();
		this->UI_PanelActive(e.isHold, e.itemInst, e.locationState);
		});

	HRESULT hr = E_FAIL;
	ITEMINFO_DESC* INFODesc = static_cast<ITEMINFO_DESC*>(pArg);
	INFODesc->bUseNineSlice = true;
	INFODesc->TextureComLevel = ETOI(LEVEL::STATIC);
	INFODesc->TextureProtoName = L"Prototype_Component_Texture_PopupBackground";
	INFODesc->vPivot = (_float2{ 0.f,0.f });
	hr = CUIPanel::OnInit(INFODesc);


	//이름 - 고정
	CUIText::TEXT_DESC ItemNameDesc = {};
	ItemNameDesc.fontaline = CUIText::FONTALINE::LEFT;
	ItemNameDesc.strFontTag = L"Noto_Sans_CJK_SC_32";
	ItemNameDesc.strText = L"이름";
	ItemNameDesc.vPivot = (_float2{ 0.f,1.f });
	ItemNameDesc.vAnchorPoint = (_float2{ 0.f,1.f });
	
	shared_ptr<CUIText> ItemNameText = CUIText::Create(m_pDevice, m_pContext);
	ItemNameText->Initialize(&ItemNameDesc);
	Add_Child(ItemNameText, L"ItemNameText", false);
	m_NameText = ItemNameText;

	// 선 - 고정
		CUIImage::UIIMAGE_DESC LineDesc = {};
		LineDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		LineDesc.TextureProtoName = L"Prototype_Component_Texture_TabDivider";
		LineDesc.vPivot = (_float2{ 0.5f,0.5f });
		LineDesc.vAnchorPoint = (_float2{ 0.5f,1.f });

		shared_ptr<CUIImage> Line = CUIImage::Create(m_pDevice, m_pContext);
		Line->Initialize(&LineDesc);
		Add_Child(Line, L"Line", false);
		m_LineImg = Line;
	

	//오른쪽 왼쪽 - 변동
	CUIText::TEXT_DESC ItemLeftDesc = {};
	ItemLeftDesc.fontaline = CUIText::FONTALINE::LEFT;
	ItemLeftDesc.strFontTag = L"Noto_Sans_CJK_SC";
	ItemLeftDesc.strText = L"왼쪽";
	ItemLeftDesc.vPivot = (_float2{ 0.f,1.f });
	ItemLeftDesc.vAnchorPoint = _float2{ 0.f,1.f };
	shared_ptr<CUIText> ItemLeftText = CUIText::Create(m_pDevice, m_pContext);
	ItemLeftText->Initialize(&ItemLeftDesc);
	Add_Child(ItemLeftText, L"ItemLeftText", false);
	m_LeftText = ItemLeftText;

	CUIText::TEXT_DESC ItemRightDesc = {};
	ItemRightDesc.fontaline = CUIText::FONTALINE::RIGHT;
	ItemRightDesc.strFontTag = L"Noto_Sans_CJK_SC";
	ItemRightDesc.strText = L"오른쪽";
	ItemRightDesc.vPivot = (_float2{ 1.f,1.f });
	ItemRightDesc.vAnchorPoint =_float2{ 1.f,1.f };
	shared_ptr<CUIText> ItemRightText = CUIText::Create(m_pDevice, m_pContext);
	ItemRightText->Initialize(&ItemRightDesc);
	Add_Child(ItemRightText, L"ItemRightText", false);
	m_RightText = ItemRightText;

	// 상세 설명 - 변동 

	CUIText::TEXT_DESC ItemDetailDesc = {};
	ItemDetailDesc.fontaline = CUIText::FONTALINE::CENTER;
	ItemDetailDesc.strFontTag = L"Noto_Sans_CJK_SC_24";
	ItemDetailDesc.strText = L"상세 설명";
	ItemDetailDesc.TextColor = _float4{ 0.6f,0.6f,0.6f,1.f };
	ItemDetailDesc.vPivot = (_float2{ 0.5f,1.f });
	ItemDetailDesc.vAnchorPoint = (_float2{ 0.5f,1.f });
	
	shared_ptr<CUIText> ItemDetailText = CUIText::Create(m_pDevice, m_pContext);
	ItemDetailText->Initialize(&ItemDetailDesc);
	Add_Child(ItemDetailText, L"ItemDetailText", false);
	m_DetailText = ItemDetailText;




	{


		m_vecIcon.resize(BUTTONINFO::END);
		for (size_t i = 0; i < BUTTONINFO::END; i++)
		{
			BUTTONINFO currentBtn = static_cast<BUTTONINFO>(i);
			string viewName = string(magic_enum::enum_name(currentBtn));
			wstring wName = S2W(viewName);

			//버튼인포 아이콘 - 변동
			CUIImage::UIIMAGE_DESC ButtonIconDesc = {};
			ButtonIconDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			ButtonIconDesc.TextureProtoName = L"Prototype_Component_Texture_KEY";
			ButtonIconDesc.TextureIndex = 119;
			ButtonIconDesc.vPivot = (_float2{ 0.5f,1.f });
			ButtonIconDesc.vAnchorPoint = (_float2{ 0.5f,1.f });
			ButtonIconDesc.vScale = (_float2{ 0.6f,0.6f });
			ButtonIconDesc.vAnchoredPos= _float2{ 10.f,0.f };

			shared_ptr<CUIImage> icon = CUIImage::Create(m_pDevice, m_pContext);
			icon->Initialize(&ButtonIconDesc);
			Add_Child(icon, L"icon"+ wName, false);
			icon->UI_InActive();

			// 버튼 인포
			CUIText::TEXT_DESC ButtonInfoDesc = {};
			ButtonInfoDesc.fontaline = CUIText::FONTALINE::RIGHT;
			ButtonInfoDesc.strFontTag = L"Noto_Sans_CJK_SC_24";
			ButtonInfoDesc.strText = L"버튼인포";
			ButtonInfoDesc.vPivot = _float2{ 1.f,0.5f };
			ButtonInfoDesc.vAnchorPoint = _float2{1.f,0.5f};
			ButtonInfoDesc.vAnchoredPos = _float2{ -(icon->GetUITransform()->Get_FinalSize().x +5.f),0.f };

			shared_ptr<CUIText> ButtonInfoText = CUIText::Create(m_pDevice, m_pContext);
			ButtonInfoText->Initialize(&ButtonInfoDesc);


			
			icon->Add_Child(ButtonInfoText, wName, false);
			ButtonInfoText->UI_InActive();

			m_vecIcon[i] = icon;
		}

	}
	return hr;
}
void CItemInfo::GetButtonInfo(_uint buttonInfo, _uint& Texindex, wstring& str, _float extraInfo)
{
	// 119 - 집기 / 213- z / 123 - 창고 / 53 - 판매 /119 놓기/ 121 회전
	switch (buttonInfo)
	{
	case ETOI(CItemInfo::BUTTONINFO::STORAGE):
		Texindex = 123;
		str = L"창고로 보내기";
		return;
	case ETOI(CItemInfo::BUTTONINFO::BUY):
		Texindex = 123;
		str = format(L"구매 [${:.2f}]", extraInfo);
		return;
	case ETOI(CItemInfo::BUTTONINFO::PICK):
		Texindex = 119;
		str = L"집기";
		return;
	case ETOI(CItemInfo::BUTTONINFO::RELEASE):
		Texindex = 119;
		str = L"놓기";
		return;
	case ETOI(CItemInfo::BUTTONINFO::ROTATION):
		Texindex = 121;
		str = L"회전";
		return;
	case ETOI(CItemInfo::BUTTONINFO::SELL):
		Texindex = 53;
		str = format(L"판매 [${:.2f}]", extraInfo);
		return;
	case ETOI(CItemInfo::BUTTONINFO::THROWUP):
		Texindex = 213;
		str = L"버리기";
		return;

	}
}
void CItemInfo::OnActive()
{
	CUIPanel::OnActive();
}

void CItemInfo::OnInActive()
{
	CUIPanel::OnInActive();
}

void CItemInfo::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CItemInfo::OnUpdate(const _float& timeDelta)
{

	GetUITransform()->SetLocalScale({ GetUITransform()->Get_LocalScale().x, m_Targetsize.y / GetUITransform()->Get_SizeDelta().y });
	m_LineImg->GetUITransform()->SetLocalScale({ GetUITransform()->Get_FinalSize().x/ m_LineImg->GetUITransform()->Get_SizeDelta().x, 0.5f});

		
	CUIPanel::OnUpdate(timeDelta);
}

void CItemInfo::OnLateUpdate()
{
	GetUITransform()->SetAnchoredPos({0.f,m_pGameInstance.lock()->Get_MousePos().y- (m_pGameInstance.lock()->Get_WinSize().Bottom()/2)});

	CUIPanel::OnLateUpdate();
}

HRESULT CItemInfo::OnRender()
{
	return CUIPanel::OnRender();
}

void CItemInfo::OnClear()
{
	CUIPanel::OnClear();
}

_float CItemInfo::Active_ButtonInfo(BUTTONINFO btnInfo, _float2 AnchoredPos, _float cost, _float2 Pivot)
{
	wstring str = L"";
	_uint TexIndex = {};

	GetButtonInfo(btnInfo, TexIndex, str, cost);
	m_vecIcon[ETOI(btnInfo)]->Set_TextureIndex(TexIndex);
	m_vecIcon[ETOI(btnInfo)]->GetUITransform()->SetPivot(Pivot);
	m_vecIcon[ETOI(btnInfo)]->GetUITransform()->SetAnchoredPos(AnchoredPos);
	auto tex = dynamic_pointer_cast<CUIText>(m_vecIcon[ETOI(btnInfo)]->GetChildren()[0]);
	tex->Set_Text(str);
	m_vecIcon[ETOI(btnInfo)]->UI_Active();
	m_vecIcon[ETOI(btnInfo)]->GetUITransform()->SetPivot({ 0.5f,1.f });
	
	//m_vecIcon[ETOI(btnInfo)]->GetUITransform()->Set

	return m_vecIcon[ETOI(btnInfo)]->GetUITransform()->Get_FinalSize().y;// 사이즈 
}

shared_ptr<CItemInfo> CItemInfo::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CItemInfo> pInstance(new CItemInfo(pDevice, pContext), [](CItemInfo* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItemInfo");

	}
	return pInstance;
}

void CItemInfo::Free()
{
	CUIPanel::Free();
}
