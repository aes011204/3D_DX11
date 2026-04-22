#include "Inventory.h"

#include "GameInstance.h"
#include "EventBus.h"

#include "ItemDB.h"
#include "PlayerBoat.h"


static _uint Num = { 1 };

CInventory::CInventory(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)
{
}

HRESULT CInventory::Initialize_Prototype()
{
	m_w = 9;
	m_h = 11;
	return S_OK;
}

HRESULT CInventory::Initialize(void* pArg)
{

	INVEN_DESC* inven_desc = static_cast<INVEN_DESC*>(pArg);
	m_InvenType = inven_desc->invenType;

	if (pArg != nullptr)
	{

		switch (inven_desc->invenType)
		{
		case INVENTYPE::PLAYER:
		{
			Init_BoatUpgrade();
			Upgrade_Boat(0);
			break;
		}
		case INVENTYPE::SHOP:
		{
			m_w = 8;
			m_h = 8;
			break;
		}
		case INVENTYPE::STORAGE:
		{
			m_w = 7;
			m_h = 8;
			break;
		}
		case INVENTYPE::CHEST:
		{
			m_w = 3;
			m_h = 3;
			break;
		}
		default:
		{
			break;
		}
		}
	}



	// 아이템 갯수기준
	m_Inventory.reserve(m_w * m_h);

	// 슬롯기준 (최대 인밴)
	m_InvenSlot.resize(m_w * m_h);


	/*Evt_InvenPlayerInit_Data e = {};
	e.Inven_ptr = static_pointer_cast<CInventory>(shared_from_this());

	m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_InvenPlayerInit_Data>(e);*/
	// 이거 플레이어 보트로 옮김 


	switch (inven_desc->invenType)
	{
	case INVENTYPE::PLAYER:
	{
		Fish_Inst instfish = {};
		Item_Inst inst1 = Create_ItemInstance(1001, instfish, 3);

		inst1.BaseXY = { 2,2 };
		AddItem(inst1, 2, 2);

		Item_Inst inst = Create_ItemInstance(1001, instfish, 1);

		inst.BaseXY = { 0,2 };
		AddItem(inst, 0, 2);

		Item_Inst inst2 = Create_ItemInstance(1002, instfish, 0);

		inst2.BaseXY = { 2,0 };
		AddItem(inst2, 2, 0);


		Equip_Inst instEquip = {};
		Item_Inst inst3 = Create_ItemInstance(2001, instEquip, 0);
		AddItem(inst3, 1, 3);

		Item_Inst inst4 = Create_ItemInstance(2003, instEquip, 1);
		AddItem(inst4, 5, 3);
		break;
	}
	case INVENTYPE::SHOP:
	{
		Equip_Inst instEquip = {};
		Item_Inst inst3 = Create_ItemInstance(2001, instEquip, 0);
		AddItem(inst3, 1, 3);

		Item_Inst inst4 = Create_ItemInstance(2003, instEquip, 1);
		AddItem(inst4, 5, 3);
	
		break;
	}
	case INVENTYPE::STORAGE:
	{
		Fish_Inst instfish = {};
		Item_Inst inst1 = Create_ItemInstance(1001, instfish, 3);

		inst1.BaseXY = { 2,2 };
		AddItem(inst1, 2, 2);

		Item_Inst inst = Create_ItemInstance(1001, instfish, 1);
		break;
	}
	case INVENTYPE::CHEST:
	{
		
		Item_Inst inst3 = Create_ItemInstance(3001, monostate{}, 0);
		AddItem(inst3, 0, 0);

		inst3 = Create_ItemInstance(3003, monostate{}, 1);
		AddItem(inst3, 0, 1);

		inst3 = Create_ItemInstance(3005, monostate{}, 0);
		AddItem(inst3, 2, 1);

		inst3 = Create_ItemInstance(3001, monostate{}, 0);
		AddItem(inst3, 0, 2);
		break;
	}
	default:
	{
		break;
	}
	}

	//test
	//
	//
	
	return S_OK;
}
Item_Inst CInventory::Create_ItemInstance(ID_uint itemDefID, variant<monostate, Fish_Inst, Equip_Inst>  TypeDefInst, int rot)
{

	const Item_Def& def = CItemDB::GetInstance()->GetItemByID(itemDefID);


	Item_Inst newInst;
	newInst.ItemDef_ID = def.ItemID;
	newInst.ItemInst_ID = Num++/*Generate_Unique_ID()*/;
	newInst.ItemType = def.ItemType;

	newInst.Rotation = rot;

	newInst.TypeDef = TypeDefInst;;
	newInst.pCashingTexture = def.pTexture;

	if (auto pFish = get_if<Fish_Inst>(&TypeDefInst))
	{
		if (pFish->mutation_ID != ID_Absence)
		{
			Mutation mut = get<Fish_Def>(def.TypeDef).vec_Mutation[pFish->mutation_ID];
			newInst.MutaionCashing = mut;
			newInst.IsMutaion = true;
			newInst.pCashingTexture = mut.pTexture;

		}
	}


	//switch (def.ItemType)
	//{
	//case ITEM_TYPE::FISH:
	//    newInst.TypeDefInst = TypeDefInst;;
	//
	//    break;
	//
	//case ITEM_TYPE::EQUIP:
	//
	//    newInst.TypeDefInst = Equip_Inst{ false };
	//    break;
	//
	//case ITEM_TYPE::MATERIAL:
	//    break;
	//
	//}


	return newInst;
}
void CInventory::Update(_float fTimeDelta)
{

}

Item_Inst CInventory::AddItem(Item_Inst itemInst, _int BaseX, _int BaseY)
{
	// 밑에 아이템 한개랑 바꿔치기 가능 : 주황 / 불가능 : 빨강 / 가능 : 초록
	PLACE_COLOR color = {};
	int instID = CanPlace(itemInst, BaseX, BaseY, color); // 여기서 한번 더 할필요가 있나?
	Item_Inst holdInst = {};

	switch (color)
	{
	case PLACE_COLOR::GREEN:
	{
		if (instID != ID_Absence)
			MSG_BOX("Wrong Item");

		// 인밴에 바로 들어감
		PlaceOn_Inven(itemInst, BaseX, BaseY);
		m_Dirty = true;
		return holdInst; // 초기화된 빈인스턴스

	}
	case PLACE_COLOR::ORANGE:
	{
		// 넣을려고 하는 아이템과 원래 있던 하나의 아이템 스왑
		// 기존 인밴에 있던 inst 제거
		holdInst = RemoveFrom_Inven(instID);
		//
		PlaceOn_Inven(itemInst, BaseX, BaseY);
		m_Dirty = true;
		return holdInst; // 스왑된 인스턴스 
	}
	case PLACE_COLOR::RED:
	{
		if (instID != -1)
			MSG_BOX("Wrong Item");
		// 불가능 
		return itemInst; // 원래 받아온 인스턴스 그대로 

	}
	}



	// 초록
}

_bool CInventory::Auto_Add(Item_Inst& itemInst)
{
	for (int i = 0; i < m_h; i++)     // y 먼저
	{
		for (int j = 0; j < m_w; j++)
		{
			if (m_InvenSlot[i * m_w + j].ItemInst_ID == ID_Absence)
			{
				PLACE_COLOR color = {};
				CanPlace(itemInst, j, i, color);
				if (color == PLACE_COLOR::GREEN)
				{
					AddItem(itemInst, j, i);
					return true;
				}
				
			}
		}

		
	}
	return false;
}

_bool CInventory::Auto_Move_To(weak_ptr<CInventory> OtherInven, _uint BaseX, _uint BaseY)
{
	//m_InvenSlot[BaseY * m_w + BaseX].ItemInst_ID
		if (m_InvenSlot[BaseY * m_w + BaseX].ItemInst_ID != ID_Absence)
	{
		for (auto& item : m_Inventory)
		{
			if (m_InvenSlot[BaseY * m_w + BaseX].ItemInst_ID == item.ItemInst_ID)
			{
				_bool result = OtherInven.lock()->Auto_Add(item);


				if(result == true)
				{
				RemoveFrom_Inven(item.ItemInst_ID);
				return true;
				}
				else
				{
				return false;
					
				}
			}
		}
	}
		
		return false;
}
_int CInventory::CanPlace(Item_Inst& itemInst, _uint BaseX, _uint BaseY, PLACE_COLOR& color)
{
	// 해당 아이템의 모양 + BaseX,Y
	const Item_Def& def = CItemDB::GetInstance()->GetItemByID(itemInst.ItemDef_ID);
	_bool hasOverlap = false;
	int overlapID = {};
	int absenceNum = {};
	_uint ID_First = {};
	_uint rot = itemInst.Rotation % 4;
	for (int i = 0; i < def.ItemShape.Occ[rot].size(); i++)
	{
		_int fx = def.ItemShape.Occ[rot][i].dx + BaseX;
		_int fy = def.ItemShape.Occ[rot][i].dy + BaseY;
		// finalCells.push_back({ fx,fy });

		if (fx < 0 || fx >= m_w || fy < 0 || fy >= m_h) {
			color = PLACE_COLOR::RED;
			return -1;
		}
		// 락이랑 겹치는지, 밑아이템 하나랑 겹치는지 , 바로 놓을 수 있는지

		const Slot& the_Slot = m_InvenSlot[fy * m_w + fx];
		ID_First = the_Slot.ItemInst_ID;


		if (the_Slot.IsLock == true || the_Slot.IsBroken == true)
		{
			//하나라도 락이랑 겹치면 불가 - 빨강
			color = PLACE_COLOR::RED;
			return -1;
		}

		if (the_Slot.ItemInst_ID == ID_Absence)
		{
			absenceNum++;
		}
		/* if (the_Slot.ItemInst_ID != ID_First)*/
		else {
			////다른 종류의 아이템이 2 개 이상 겹쳐 있다 - place 불가 - 빨간
			//color = PLACE_COLOR::RED;
			//return -1;
			if (!hasOverlap)
			{
				//첫 번째 겹친 아이템
				overlapID = the_Slot.ItemInst_ID;
				hasOverlap = true;
			}
			else if (the_Slot.ItemInst_ID != overlapID)
			{

				color = PLACE_COLOR::RED;
				return -1;
			}
		}
	}

	if (absenceNum == def.ItemShape.Occ[rot].size())
	{
		// 해당 칸 전부 빈칸이면 - 그린
		color = PLACE_COLOR::GREEN;
		return ID_Absence;
	}



	// 겹치는 아이템이 있긴한데 동일한 아이템이다 들고 있는거랑 스위치 가능 - 주황
	color = PLACE_COLOR::ORANGE;
	return static_cast<_int>(overlapID);
}

Item_Inst CInventory::RemoveFrom_Inven(int inst_id)
{

	Item_Inst inst = {};
	// 인밴에서 지우기
	for (auto it = m_Inventory.begin(); it != m_Inventory.end(); it++) // 하나 지우고 바로 나갈꺼라 ㄱㅊ
	{
		if ((*it).ItemInst_ID == inst_id)
		{
			inst = ((*it));
			m_Inventory.erase(it);
			break;
		}
	}

	// 인밴슬롯에서 지우기

  //  for (int i = 0;i < m_InvenSlot.size(); i++)
  //  {
		////전체 순회하면서 해당 인스턴스 아이디 다 지우기
  //      if(m_InvenSlot[i].ItemInst_ID== inst_id)
  //      {
  //          m_InvenSlot[i].ItemInst_ID = 0;// 0 이 아무것도 없음 
  //      }
  //  }

	for (int i = 0; i < inst.CurBase.size(); i++)
	{
		m_InvenSlot[inst.CurBase[i].dy * m_w + inst.CurBase[i].dx].ItemInst_ID = 0;
	}

	m_Dirty = true;

	return inst;
}

Item_Inst CInventory::Peek_Itme(_uint MusX, _uint MusY)
{
	if (MusX >= m_w || MusY >= m_h)
	{
		Item_Inst emptyItem = {};
		emptyItem.ItemInst_ID = ID_Absence;
		return emptyItem;
	}

	_uint SlotIndex = MusY * m_w + MusX;

	Slot& CurrentSlot = m_InvenSlot[SlotIndex];

	if (CurrentSlot.ItemInst_ID != ID_Absence)
	{
		for (auto& item : m_Inventory)
		{
			if (item.ItemInst_ID == CurrentSlot.ItemInst_ID)
				return item;
		}
	}

	Item_Inst emptyItem = {};
	emptyItem.ItemInst_ID = ID_Absence;
	return emptyItem;
}

void CInventory::OnGui()
{

	ImGui::Text("Size: %d x %d", m_w, m_h);
	ImGui::Text("Item Count: %d", (int)m_Inventory.size());

	if (ImGui::BeginTabBar("InvenTabs"))
	{
		// --- Tab 1: 격자 시각화 (Slot View) ---
		if (ImGui::BeginTabItem("Grid View"))
		{
			// 격자 그리기 시작
			if (ImGui::BeginTable("InvenGrid", m_w, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit))
			{
				for (int row = 0; row < m_h; row++)
				{
					ImGui::TableNextRow();
					for (int col = 0; col < m_w; col++)
					{
						ImGui::TableSetColumnIndex(col);

						int slotIndex = row * m_w + col;
						Slot& slot = m_InvenSlot[slotIndex];

						// 슬롯 상태에 따른 색상 지정
						ImVec4 color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // 기본 빈칸 (어두운 회색)

						if (slot.IsLock) color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);      // 잠금 (검정)
						else if (slot.IsBroken) color = ImVec4(0.8f, 0.2f, 0.2f, 1.0f); // 파손 (빨강)
						else if (slot.ItemInst_ID != 0) color = ImVec4(0.2f, 0.7f, 0.2f, 1.0f); // 아이템 존재 (초록)

						// 해당 색상으로 버튼 출력 (내부에는 ID 표시)
						ImGui::PushStyleColor(ImGuiCol_Button, color);
						string slotLabel = to_string(slot.ItemInst_ID) + "##" + to_string(slotIndex);
						ImGui::Button(slotLabel.c_str(), ImVec2(35, 35));

						// 마우스 올리면 툴팁으로 상세 정보 표시
						if (ImGui::IsItemHovered()) {
							ImGui::BeginTooltip();
							ImGui::Text("Index: %d (x:%d, y:%d)", slotIndex, col, row);
							ImGui::Text("ItemInstID: %d", slot.ItemInst_ID);
							ImGui::Text("Type: %d", (int)slot.slotType);
							ImGui::EndTooltip();
						}
						ImGui::PopStyleColor();
					}
				}
				ImGui::EndTable();
			}
			ImGui::EndTabItem();
		}

		// --- Tab 2: 아이템 리스트 (Data View) ---
		if (ImGui::BeginTabItem("Item List"))
		{
			for (auto& item : m_Inventory)
			{
				string headerLabel = "InstID: " + to_string(item.ItemInst_ID) + " (DefID: " + to_string(item.ItemDef_ID) + ")";
				if (ImGui::CollapsingHeader(headerLabel.c_str()))
				{
					ImGui::BulletText("Type: %d", (int)item.ItemType);
					ImGui::BulletText("Rotation: %d", item.Rotation);
					ImGui::BulletText("Base Pos: %d, %d", item.BaseXY.x, item.BaseXY.y);

					if (ImGui::TreeNode("Occupied Cells"))
					{
						for (auto& occ : item.CurBase)
							ImGui::Text(" - dx: %d, dy: %d (Global x:%d, y:%d)", occ.dx, occ.dy, item.BaseXY.x + occ.dx, item.BaseXY.y + occ.dy);
						ImGui::TreePop();
					}
				}
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}


}

Evt_ShipStat CInventory::CalculateEquip()
{
	Evt_ShipStat stat = {};

	auto DB = CItemDB::GetInstance();
	for (_uint i = 0; i < m_Inventory.size(); i++)
	{
		if (m_Inventory[i].ItemType == ITEM_TYPE::EQUIP)
		{
			Item_Def def = DB->GetItemByID(m_Inventory[i].ItemDef_ID);

			if (Equip_Def* pEquip = get_if<Equip_Def>(&def.TypeDef))
			{
				int cost = pEquip->Cost;
				_uint installTime = pEquip->InstallTime;
				EQUIP_TYPE EquipType = pEquip->EquipType;

				if (Equip_Engine* Engine = get_if<Equip_Engine>(&pEquip->EquipType_Effect))
				{
					stat.EngineSpeed += Engine->Speed_Kn;
				}
				else if (Equip_Light* Light = get_if<Equip_Light>(&pEquip->EquipType_Effect))
				{
					stat.LightIntensity += Light->Lumen;
				}
				else if (Equip_Rod* Rod = get_if<Equip_Rod>(&pEquip->EquipType_Effect))
				{
					stat.FishingSpeed += Rod->Speed_Per;
					stat.SeaMask |= Rod->SeaType_Mask;

				}
				/*  else if (Equip_Net* Net = get_if<Equip_Net>(&pEquip->EquipType_Effect))
				  {

				  }
				  else if (Equip_CrapPot* CrapPot = get_if<Equip_CrapPot>(&pEquip->EquipType_Effect))
				  {

				  }*/
			}
			if (m_Inventory[i].ItemType == ITEM_TYPE::FISH)
			{
				Item_Def def = DB->GetItemByID(m_Inventory[i].ItemDef_ID);
				if (Fish_Def* pFish = get_if<Fish_Def>(&def.TypeDef))
				{

					if (m_Inventory[i].IsMutaion == false)
					{
						stat.InvenMoney += pFish->Cost;

					}
					else
					{
						stat.InvenMoney += m_Inventory[i].MutaionCashing.MutCost;
					}
				}
	
			}
		}


	}
	//auto player = dynamic_pointer_cast<CPlayerBoat>(Get_GOwner());
	//if (player != nullptr)
	//{

	//	player->Set_ShipStats(EngineSpeed, LightIntensity, FishingSpeed, SeaMask);
	//	/* player->Set_Light(LightIntensity);
	//	 player->Set_FishingSpeed(FishingSpeed);
	//	 player->Set_SeaMask(SeaMask);*/
	//}

	return stat;
}

void CInventory::Get_Damage()
{
	// 사이즈가 맥스로 랜덤 인뎃스
	int RandIndex = { -1 };
	RandIndex = static_cast<int>(m_pGameInstance.lock()->Random(0, m_InvenSlot.size()-1));
	// 전체 순회 -> 만일 이미 고장난 칸이거나 투명칸이면 다시 렌덤
	while(m_InvenSlot[RandIndex].IsBroken == true || m_InvenSlot[RandIndex].IsLock == true)
	{
		RandIndex = abs(m_pGameInstance.lock()->Random(0, m_InvenSlot.size()));
	}

	// 장비가 있을떄 -> 장비 이것도 그냥 버려버려?? 일단은 버려
	// 물고기가 있을 떄 -> 물고기 버리기 ->칸 x로 변환
	if(m_InvenSlot[RandIndex].ItemInst_ID!= ID_Absence)
	{
		//ThrowAwayFrom_Inven() // 이건 위치 
		RemoveFrom_Inven(m_InvenSlot[RandIndex].ItemInst_ID); // 아이디로
	}
		m_InvenSlot[RandIndex].IsBroken = true;

		return;
}

void CInventory::PlaceOn_Inven(Item_Inst itemInst, _int BaseX, _int BaseY)
{
	itemInst.BaseXY = _float2{ static_cast<_float>(BaseX), static_cast<_float>(BaseY) };

	itemInst.CurBase.clear();

	// 인밴에 넣기
	m_Inventory.push_back(itemInst);
	Item_Inst& stored = m_Inventory.back();
	// 인밴슬롯에 넣기
	const Item_Def& def = CItemDB::GetInstance()->GetItemByID(itemInst.ItemDef_ID);
	_uint rot = itemInst.Rotation % 4;


	for (int i = 0; i < def.ItemShape.Occ[rot].size(); i++)
	{
		_uint fx = (def.ItemShape.Occ[rot][i].dx) + BaseX;
		_uint fy = (def.ItemShape.Occ[rot][i].dy) + BaseY;
		// finalCells.push_back({ fx,fy });

	  // 락이랑 겹치는지, 밑아이템 하나랑 겹치는지 , 바로 놓을 수 있는지
		//LOG_F(LOG_LEVEL::INFO, "fx:%d fy:%d w:%d h:%d", fx, fy, m_w, m_h);
		m_InvenSlot[fy * m_w + fx].ItemInst_ID = itemInst.ItemInst_ID;

		stored.CurBase.push_back({ fx,fy });
	}

	m_Dirty = true;

}

Item_Inst CInventory::TryMove_Item(_uint BaseX, _uint BaseY)
{
	ID_uint id = m_InvenSlot[BaseY * m_w + BaseX].ItemInst_ID;

	Item_Inst holdInst = RemoveFrom_Inven(id);


	return holdInst;

}

void CInventory::ThrowAwayFrom_Inven(_uint BaseX, _uint BaseY)
{
	//집고 있을떄랑 / 인밴에 있을떄 둘다 버릴수 있음

	// 집고 있는거 버리는건 컨트롤러에서 (집고 있는 아이템의 관리는 컨트로러)

	ID_uint id = m_InvenSlot[BaseY * m_w + BaseX].ItemInst_ID;

	RemoveFrom_Inven(id);
	m_Dirty = true;

}

void CInventory::Apply_BaseMask(vector<Slot>& vecSlot)
{


}

void CInventory::Init_BoatUpgrade()
{

	_uint tw = m_BoatUpgrade_type[0].m_width = 6;
	_uint th = m_BoatUpgrade_type[0].m_height = 8;
	m_BoatUpgrade_type[0].m_type.resize(m_w * m_h);
	m_BoatUpgrade_type[0].m_type = {
	'O','A','A','L','O','O',
	'O','A','A','A','A','O',
	'A','A','A','A','A','A',
	'R','R','A','A','A','R',
	'R','R','A','A','A','R',
	'A','A','A','A','A','R',
	'O','A','E','E','A','O',
	'O','O','E','E','O','O',
	};

	tw = m_BoatUpgrade_type[1].m_width = 7;
	th = m_BoatUpgrade_type[1].m_height = 9;
	m_BoatUpgrade_type[1].m_type.resize(m_w * m_h);
	m_BoatUpgrade_type[1].m_type = {
	'O','O','A','L','O','O','O',
	'O','A','A','A','A','O','O',
	'A','A','A','A','A','A','A',
	'R','R','A','A','A','R','R',
	'R','R','A','A','A','R','R',
	'A','A','A','A','A','R','R',
	'O','A','E','E','A','O','O',
	'O','O','E','E','O','O','O',
	'O','O','E','E','O','O','O',
	};

	tw = m_BoatUpgrade_type[2].m_width = 9;
	th = m_BoatUpgrade_type[2].m_height = 11;
	m_BoatUpgrade_type[2].m_type.resize(m_w * m_h);
	m_BoatUpgrade_type[2].m_type = {
	'O','O','O','L','L','L','O','O','O',
	'O','O','A','A','A','A','A','O','O',
	'O','A','A','A','A','A','A','A','O',
	'R','R','A','A','A','A','A','R','R',
	'R','R','A','A','A','A','A','R','R',
	'R','R','A','A','A','A','R','R','R',
	'R','A','A','A','A','A','A','A','R',
	'O','A','A','A','A','A','A','A','O',
	'O','O','A','E','E','A','A','O','O',
	'O','O','E','E','E','A','A','O','O',
	'O','O','E','E','E','A','A','O','O',
	};

}

void CInventory::Upgrade_Boat(_uint index)
{

	m_h = m_BoatUpgrade_type[index].m_height;
	m_w = m_BoatUpgrade_type[index].m_width;

	m_Inventory.reserve(m_w * m_h);
	m_InvenSlot.resize(m_w * m_h);

	//

	for (int th = 0; th < m_h; th++)
	{
		for (int tw = 0; tw < m_w; tw++)
		{
			_int tmp = static_cast<_int>(m_BoatUpgrade_type[index].m_type[th * m_w + tw]);
			switch (tmp)
			{
			case  static_cast<_int>('O'):
				m_InvenSlot[th * m_w + tw].IsLock = true;
				m_InvenSlot[th * m_w + tw].slotType = SLOT_TYPE::END;
				break;
			case static_cast<_int>('A'):
				m_InvenSlot[th * m_w + tw].IsLock = false;
				m_InvenSlot[th * m_w + tw].slotType = SLOT_TYPE::ANY;
				break;
			case static_cast<_int>('E'):
				m_InvenSlot[th * m_w + tw].IsLock = false;
				m_InvenSlot[th * m_w + tw].slotType = SLOT_TYPE::ENGINE;
				break;
			case static_cast<_int>('L'):
				m_InvenSlot[th * m_w + tw].IsLock = false;
				m_InvenSlot[th * m_w + tw].slotType = SLOT_TYPE::LIGHT;
				break;
			case static_cast<_int>('R'):
				m_InvenSlot[th * m_w + tw].IsLock = false;
				m_InvenSlot[th * m_w + tw].slotType = SLOT_TYPE::ROT;
				break;
			case static_cast<_int>('N'):
				m_InvenSlot[th * m_w + tw].IsLock = false;
				m_InvenSlot[th * m_w + tw].slotType = SLOT_TYPE::NET;
				break;
			}
			// 아 비트 플레그 해야 하나...
		}
	}

}

void CInventory::SetHighlightArea(Item_Inst& itemInst, _uint BaseX, _uint BaseY, PLACE_COLOR color)
{
	//const Item_Def& def = CItemDB::GetInstance()->GetItemByID(itemInst.ItemDef_ID);
	//_bool hasOverlap = false;
	//int overlapID = {};
	//int absenceNum = {};
	//_uint ID_First = {};
	//_uint rot = itemInst.Rotation % 4;




	const Item_Def& def = CItemDB::GetInstance()->GetItemByID(itemInst.ItemDef_ID);
	_uint rot = itemInst.Rotation % 4;



	for (auto& slot : m_InvenSlot)
	{
		slot.Slot_Color = PLACE_COLOR::END;
	}

	for (int i = 0; i < def.ItemShape.Occ[rot].size(); i++)
	{

		_uint fx = (def.ItemShape.Occ[rot][i].dx) + BaseX;
		_uint fy = (def.ItemShape.Occ[rot][i].dy) + BaseY;

		if (fx < 0 || fx >= (int)m_w || fy < 0 || fy >= (int)m_h)
		{
			//TODO::나간것도 빨간색
			color = PLACE_COLOR::RED;
			continue;;

		}


		switch (color)
		{
		case PLACE_COLOR::GREEN:

		{
			Slot& slot = m_InvenSlot[fy * m_w + fx];
			slot.Slot_Color = PLACE_COLOR::GREEN;
		}
		break;
		case PLACE_COLOR::ORANGE:
		{
			Slot& slot = m_InvenSlot[fy * m_w + fx];
			slot.Slot_Color = PLACE_COLOR::ORANGE;
		}
		break;

		case PLACE_COLOR::RED:
		{
			Slot& slot = m_InvenSlot[fy * m_w + fx];
			slot.Slot_Color = PLACE_COLOR::RED;
		}
		break;
		}
	}



}

shared_ptr<CInventory> CInventory::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CInventory> pInstance(new CInventory(pDevice, pContext), [](CInventory* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInventory");
	}
	return pInstance;
}


shared_ptr<CComponent> CInventory::Clone(void* pArg)
{
	shared_ptr<CInventory> pInstance(new CInventory(*this), [](CInventory* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInventory");
		return nullptr;
	}
	return pInstance;
}
void CInventory::Free()
{
	__super::Free();
}
