#include "Inventory.h"

#include "GameInstance.h"
#include "EventBus.h"

#include "ItemDB.h"
#include "PlayerBoat.h"


static _uint num = { 1 };

CInventory::CInventory(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CComponent(device, context)
{
}

HRESULT CInventory::Initialize_Prototype()
{
	m_w = 9;
	m_h = 11;
	return S_OK;
}

HRESULT CInventory::Initialize(void* arg)
{

	InvenDesc* invenDesc = static_cast<InvenDesc*>(arg);
	m_invenType = invenDesc->invenType;

	if (arg != nullptr)
	{

		switch (invenDesc->invenType)
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
	m_inventory.reserve(m_w * m_h);

	// 슬롯기준 (최대 인밴)
	m_invenSlot.resize(m_w * m_h);


	/*Evt_InvenPlayerInit_Data e = {};
	e.Inven_ptr = static_pointer_cast<CInventory>(shared_from_this());

	m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_InvenPlayerInit_Data>(e);*/
	// 이거 플레이어 보트로 옮김 


	switch (invenDesc->invenType)
	{
	case INVENTYPE::PLAYER:
	{
		FishInst instfish = {};
		ItemInst inst1 = Create_ItemInstance(1001, instfish, 3);

		/*inst1.BaseXY = { 2,2 };
		AddItem(inst1, 2, 2);

		Item_Inst inst = Create_ItemInstance(1001, instfish, 1);

		inst.BaseXY = { 0,2 };
		AddItem(inst, 0, 2);

		Item_Inst inst2 = Create_ItemInstance(1002, instfish, 0);

		inst2.BaseXY = { 2,0 };
		AddItem(inst2, 2, 0);*/


		EquipInst instEquip = {};
		ItemInst inst3 = Create_ItemInstance(2001, instEquip, 0);
		AddItem(inst3, 2, 6);

		ItemInst inst4 = Create_ItemInstance(2003, instEquip, 1);
		AddItem(inst4, 5, 3);
		break;
	}
	case INVENTYPE::SHOP:
	{

		EquipInst instEquip = {};

		if (invenDesc->equipType == EQUIP_TYPE::ENGINE)
		{
			ItemInst inst3 = Create_ItemInstance(2001, instEquip, 0);
			AddItem(inst3, 0, 0);

			ItemInst inst4 = Create_ItemInstance(2006, instEquip, 0);
			AddItem(inst4, 1, 0);

			ItemInst inst5 = Create_ItemInstance(2007, instEquip, 0);
			AddItem(inst5, 3, 0);

			
		}
		else if (invenDesc->equipType == EQUIP_TYPE::LIGHT)
		{
			ItemInst inst3 = Create_ItemInstance(2002, instEquip, 0);
			AddItem(inst3, 0, 0);

			ItemInst inst4 = Create_ItemInstance(2008, instEquip, 0);
			AddItem(inst4, 1, 0);

			ItemInst inst5 = Create_ItemInstance(2009, instEquip, 0);
			AddItem(inst5, 2, 0);

			ItemInst inst6 = Create_ItemInstance(2010, instEquip, 0);
			AddItem(inst6, 4, 0);
		}
		else if (invenDesc->equipType == EQUIP_TYPE::NET)
		{

		}
		else if (invenDesc->equipType == EQUIP_TYPE::ROD)
		{
			ItemInst inst3 = Create_ItemInstance(2003, instEquip, 0);
			AddItem(inst3, 0, 0);

			ItemInst inst4 = Create_ItemInstance(2011, instEquip, 0);
			AddItem(inst4, 2, 0);

			ItemInst inst5 = Create_ItemInstance(2012, instEquip, 0);
			AddItem(inst5, 4, 0);

		}
		
	
		break;
	}
	case INVENTYPE::STORAGE:
	{
		
		/*Fish_Inst instfish = {};
		Item_Inst inst1 = Create_ItemInstance(1001, instfish, 3);

		inst1.BaseXY = { 2,2 };
		AddItem(inst1, 2, 2);

		Item_Inst inst = Create_ItemInstance(1001, instfish, 1);*/
		break;
	}
	case INVENTYPE::CHEST:
	{
		
		ItemInst inst3 = Create_ItemInstance(3001, monostate{}, 0);
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
ItemInst CInventory::Create_ItemInstance(ID_uint itemDefId, variant<monostate, FishInst, EquipInst>  typeDefInst, int rot)
{

	const ItemDef& def = CItemDB::GetInstance()->GetItemByID(itemDefId);


	ItemInst newInst;
	newInst.itemDefId = def.itemId;
	newInst.itemInstId = num++/*Generate_Unique_ID()*/;
	newInst.itemType = def.itemType;

	newInst.rotation = rot;

	newInst.typeDef = typeDefInst;;
	newInst.cachingTexture = def.texture;

	if (auto fish = get_if<FishInst>(&typeDefInst))
	{
		const FishDef& fishDef = get<FishDef>(def.typeDef);
		const int mutationIndex = static_cast<int>(fish->mutationId) - 1;

		if (mutationIndex >= 0 && mutationIndex < static_cast<int>(fishDef.vecMutation.size()))
		{
			const Mutation& mut = fishDef.vecMutation[mutationIndex];
			newInst.mutationCaching = mut;
			newInst.isMutation = true;
			newInst.cachingTexture = mut.texture;

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
void CInventory::Update(_float timeDelta)
{

}

ItemInst CInventory::AddItem(ItemInst itemInst, _int baseX, _int baseY)
{
	// 밑에 아이템 한개랑 바꿔치기 가능 : 주황 / 불가능 : 빨강 / 가능 : 초록
	PLACE_COLOR color = {};
	int instID = CanPlace(itemInst, baseX, baseY, color); // 여기서 한번 더 할필요가 있나?
	ItemInst holdInst = {};

	switch (color)
	{
	case PLACE_COLOR::GREEN:
	{
		if (instID != ID_Absence)
			MSG_BOX("Wrong Item");

		// 인밴에 바로 들어감
		PlaceOn_Inven(itemInst, baseX, baseY);
		m_isDirty = true;
		return holdInst; // 초기화된 빈인스턴스

	}
	case PLACE_COLOR::ORANGE:
	{
		// 넣을려고 하는 아이템과 원래 있던 하나의 아이템 스왑
		// 기존 인밴에 있던 inst 제거
		holdInst = RemoveFrom_Inven(instID);
		//
		PlaceOn_Inven(itemInst, baseX, baseY);
		m_isDirty = true;
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

_bool CInventory::Auto_Add(ItemInst& itemInst)
{
	for (int i = 0; i < m_h; i++)     // y 먼저
	{
		for (int j = 0; j < m_w; j++)
		{
			if (m_invenSlot[i * m_w + j].itemInstId == ID_Absence)
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

_bool CInventory::Auto_Move_To(weak_ptr<CInventory> otherInven, _uint baseX, _uint baseY)
{
	//m_InvenSlot[BaseY * m_w + BaseX].ItemInst_ID
		if (m_invenSlot[baseY * m_w + baseX].itemInstId != ID_Absence)
	{
		for (auto& item : m_inventory)
		{
			if (m_invenSlot[baseY * m_w + baseX].itemInstId == item.itemInstId)
			{
				_bool result = otherInven.lock()->Auto_Add(item);


				if(result == true)
				{
				RemoveFrom_Inven(item.itemInstId);
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
_int CInventory::CanPlace(ItemInst& itemInst, _uint baseX, _uint baseY, PLACE_COLOR& color)
{
	// 해당 아이템의 모양 + BaseX,Y
	const ItemDef& def = CItemDB::GetInstance()->GetItemByID(itemInst.itemDefId);
	_bool hasOverlap = false;
	int overlapID = {};
	int absenceNum = {};
	_uint rot = itemInst.rotation % 4;
	for (int i = 0; i < def.itemShape.occ[rot].size(); i++)
	{
		_int fx = def.itemShape.occ[rot][i].dx + baseX;
		_int fy = def.itemShape.occ[rot][i].dy + baseY;
		// finalCells.push_back({ fx,fy });

		if (fx < 0 || fx >= m_w || fy < 0 || fy >= m_h) {
			color = PLACE_COLOR::RED;
			return -1;
		}
		// 락이랑 겹치는지, 밑아이템 하나랑 겹치는지 , 바로 놓을 수 있는지

		const Slot& theSlot = m_invenSlot[fy * m_w + fx];


		if (theSlot.isLock == true || theSlot.isBroken == true)
		{
			//하나라도 락이랑 겹치면 불가 - 빨강
			color = PLACE_COLOR::RED;
			return -1;
		}
		if (itemInst.itemType == ITEM_TYPE::EQUIP&&INVENTYPE::PLAYER==m_invenType)
		{
			if (auto equip = get_if<EquipDef>(&def.typeDef))
			{
				switch (equip->equipTypeEffect.index())
				{
				case 0: // Equip_Engine
					if(theSlot.slotType!=SLOT_TYPE::ENGINE)
					{
						color = PLACE_COLOR::RED;
						return -1;
					}
					break;
				case 1: // Equip_Light
					if (theSlot.slotType != SLOT_TYPE::LIGHT)
					{
						color = PLACE_COLOR::RED;
						return -1;
					}
					break;
				case 2: // Equip_Rod
					if (theSlot.slotType != SLOT_TYPE::ROT)
					{
						color = PLACE_COLOR::RED;
						return -1;
					}
					break;
				case 3: // Equip_Net
					if (theSlot.slotType != SLOT_TYPE::NET)
					{
						color = PLACE_COLOR::RED;
						return -1;
					}
					break;
				
				}
				
			}
		}

		if (theSlot.itemInstId == ID_Absence)
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
				overlapID = theSlot.itemInstId;
				hasOverlap = true;
			}
			else if (theSlot.itemInstId != overlapID)
			{

				color = PLACE_COLOR::RED;
				return -1;
			}
		}
	}

	if (absenceNum == def.itemShape.occ[rot].size())
	{
		// 해당 칸 전부 빈칸이면 - 그린
		color = PLACE_COLOR::GREEN;
		return ID_Absence;
	}



	// 겹치는 아이템이 있긴한데 동일한 아이템이다 들고 있는거랑 스위치 가능 - 주황
	color = PLACE_COLOR::ORANGE;
	return static_cast<_int>(overlapID);
}

ItemInst CInventory::RemoveFrom_Inven(int instId)
{

	ItemInst inst = {};
	// 인밴에서 지우기
	for (auto it = m_inventory.begin(); it != m_inventory.end(); it++) // 하나 지우고 바로 나갈꺼라 ㄱㅊ
	{
		if ((*it).itemInstId == instId)
		{
			inst = ((*it));
			m_inventory.erase(it);
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

	for (int i = 0; i < inst.curBase.size(); i++)
	{
		m_invenSlot[inst.curBase[i].dy * m_w + inst.curBase[i].dx].itemInstId = 0;
	}

	m_isDirty = true;

	return inst;
}

ItemInst CInventory::Peek_Item(_uint musX, _uint musY)
{
	if (musX >= m_w || musY >= m_h)
	{
		ItemInst emptyItem = {};
		emptyItem.itemInstId = ID_Absence;
		return emptyItem;
	}

	_uint slotIndex = musY * m_w + musX;

	Slot& currentSlot = m_invenSlot[slotIndex];

	if (currentSlot.itemInstId != ID_Absence)
	{
		for (auto& item : m_inventory)
		{
			if (item.itemInstId == currentSlot.itemInstId)
				return item;
		}
	}

	ItemInst emptyItem = {};
	emptyItem.itemInstId = ID_Absence;
	return emptyItem;
}

void CInventory::OnGui()
{

	ImGui::Text("Size: %d x %d", m_w, m_h);
	ImGui::Text("Item Count: %d", (int)m_inventory.size());

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
						Slot& slot = m_invenSlot[slotIndex];

						// 슬롯 상태에 따른 색상 지정
						ImVec4 color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // 기본 빈칸 (어두운 회색)

						if (slot.isLock) color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);      // 잠금 (검정)
						else if (slot.isBroken) color = ImVec4(0.8f, 0.2f, 0.2f, 1.0f); // 파손 (빨강)
						else if (slot.itemInstId != 0) color = ImVec4(0.2f, 0.7f, 0.2f, 1.0f); // 아이템 존재 (초록)

						// 해당 색상으로 버튼 출력 (내부에는 ID 표시)
						ImGui::PushStyleColor(ImGuiCol_Button, color);
						string slotLabel = to_string(slot.itemInstId) + "##" + to_string(slotIndex);
						ImGui::Button(slotLabel.c_str(), ImVec2(35, 35));

						// 마우스 올리면 툴팁으로 상세 정보 표시
						if (ImGui::IsItemHovered()) {
							ImGui::BeginTooltip();
							ImGui::Text("Index: %d (x:%d, y:%d)", slotIndex, col, row);
							ImGui::Text("ItemInstID: %d", slot.itemInstId);
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
			for (auto& item : m_inventory)
			{
				string headerLabel = "InstID: " + to_string(item.itemInstId) + " (DefID: " + to_string(item.itemDefId) + ")";
				if (ImGui::CollapsingHeader(headerLabel.c_str()))
				{
					ImGui::BulletText("Type: %d", (int)item.itemType);
					ImGui::BulletText("Rotation: %d", item.rotation);
					ImGui::BulletText("Base Pos: %d, %d", item.baseXY.x, item.baseXY.y);

					if (ImGui::TreeNode("Occupied Cells"))
					{
						for (auto& occ : item.curBase)
							ImGui::Text(" - dx: %d, dy: %d (Global x:%d, y:%d)", occ.dx, occ.dy, item.baseXY.x + occ.dx, item.baseXY.y + occ.dy);
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

	auto db = CItemDB::GetInstance();
	for (_uint i = 0; i < m_inventory.size(); i++)
	{
		if (m_inventory[i].itemType == ITEM_TYPE::EQUIP)
		{
			ItemDef def = db->GetItemByID(m_inventory[i].itemDefId);

			if (EquipDef* equip = get_if<EquipDef>(&def.typeDef))
			{
				int cost = equip->cost;
				_uint installTime = equip->installTime;
				EQUIP_TYPE equipType = equip->equipType;

				if (EquipEngine* engine = get_if<EquipEngine>(&equip->equipTypeEffect))
				{
					stat.engineSpeed += engine->speedKn;
				}
				else if (EquipLight* light = get_if<EquipLight>(&equip->equipTypeEffect))
				{
					stat.lightIntensity += light->lumen;
				}
				else if (EquipRod* rod = get_if<EquipRod>(&equip->equipTypeEffect))
				{
					stat.fishingSpeed += rod->speedPer;
					stat.seaMask |= rod->seaTypeMask;

				}
				/*  else if (Equip_Net* Net = get_if<Equip_Net>(&pEquip->EquipType_Effect))
				  {

				  }
				  else if (Equip_CrapPot* CrapPot = get_if<Equip_CrapPot>(&pEquip->EquipType_Effect))
				  {

				  }*/
			}
			if (m_inventory[i].itemType == ITEM_TYPE::FISH)
			{
				ItemDef def = db->GetItemByID(m_inventory[i].itemDefId);
				if (FishDef* fish = get_if<FishDef>(&def.typeDef))
				{

					if (m_inventory[i].isMutation == false)
					{
						stat.invenMoney += fish->cost;

					}
					else
					{
						stat.invenMoney += m_inventory[i].mutationCaching.mutCost;
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
	int randIndex = { -1 };
	randIndex = static_cast<int>(m_pGameInstance.lock()->Random(0, m_invenSlot.size()-1));
	// 전체 순회 -> 만일 이미 고장난 칸이거나 투명칸이면 다시 렌덤
	while(m_invenSlot[randIndex].isBroken == true || m_invenSlot[randIndex].isLock == true|| (m_invenSlot[randIndex].slotType != SLOT_TYPE::END && m_invenSlot[randIndex].slotType != SLOT_TYPE::ANY))
	{
		randIndex = (m_pGameInstance.lock()->RandomInt(0, m_invenSlot.size()-1));
	}

	// 장비가 있을떄 -> 장비 이것도 그냥 버려버려?? 일단은 버려
	// 물고기가 있을 떄 -> 물고기 버리기 ->칸 x로 변환
	if(m_invenSlot[randIndex].itemInstId!= ID_Absence)
	{
		//ThrowAwayFrom_Inven() // 이건 위치 
		RemoveFrom_Inven(m_invenSlot[randIndex].itemInstId); // 아이디로
	}
		m_invenSlot[randIndex].isBroken = true;

		return;
}

void CInventory::PlaceOn_Inven(ItemInst itemInst, _int baseX, _int baseY)
{
	itemInst.baseXY = _float2{ static_cast<_float>(baseX), static_cast<_float>(baseY) };

	itemInst.curBase.clear();

	// 인밴에 넣기
	m_inventory.push_back(itemInst);
	ItemInst& stored = m_inventory.back();
	// 인밴슬롯에 넣기
	const ItemDef& def = CItemDB::GetInstance()->GetItemByID(itemInst.itemDefId);
	_uint rot = itemInst.rotation % 4;


	for (int i = 0; i < def.itemShape.occ[rot].size(); i++)
	{
		_uint fx = (def.itemShape.occ[rot][i].dx) + baseX;
		_uint fy = (def.itemShape.occ[rot][i].dy) + baseY;
		// finalCells.push_back({ fx,fy });

	  // 락이랑 겹치는지, 밑아이템 하나랑 겹치는지 , 바로 놓을 수 있는지
		//LOG_F(LOG_LEVEL::INFO, "fx:%d fy:%d w:%d h:%d", fx, fy, m_w, m_h);
		m_invenSlot[fy * m_w + fx].itemInstId = itemInst.itemInstId;

		stored.curBase.push_back({ fx,fy });
	}

	m_isDirty = true;

}

ItemInst CInventory::TryMove_Item(_uint baseX, _uint baseY)
{
	ID_uint id = m_invenSlot[baseY * m_w + baseX].itemInstId;

	ItemInst holdInst = RemoveFrom_Inven(id);


	return holdInst;

}

void CInventory::ThrowAwayFrom_Inven(_uint baseX, _uint baseY)
{
	//집고 있을떄랑 / 인밴에 있을떄 둘다 버릴수 있음

	// 집고 있는거 버리는건 컨트롤러에서 (집고 있는 아이템의 관리는 컨트로러)

	ID_uint id = m_invenSlot[baseY * m_w + baseX].itemInstId;

	RemoveFrom_Inven(id);
	m_isDirty = true;

}
void CInventory::ClearHighlightArea()
{
	for (auto& slot : m_invenSlot)
	{
		slot.slotColor = PLACE_COLOR::END;
	}
}
void CInventory::Apply_BaseMask(vector<Slot>& vecSlot)
{


}

void CInventory::Init_BoatUpgrade()
{

	_uint tw = m_boatUpgradeType[0].width = 6;
	_uint th = m_boatUpgradeType[0].height = 8;
	m_boatUpgradeType[0].type.resize(m_w * m_h);
	m_boatUpgradeType[0].type = {
	'O','A','A','L','O','O',
	'O','A','A','A','A','O',
	'A','A','A','A','A','A',
	'R','R','A','A','A','R',
	'R','R','A','A','A','R',
	'A','A','A','A','A','R',
	'O','A','E','E','A','O',
	'O','O','E','E','O','O',
	};

	tw = m_boatUpgradeType[1].width = 7;
	th = m_boatUpgradeType[1].height = 9;
	m_boatUpgradeType[1].type.resize(m_w * m_h);
	m_boatUpgradeType[1].type = {
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

	tw = m_boatUpgradeType[2].width = 9;
	th = m_boatUpgradeType[2].height = 11;
	m_boatUpgradeType[2].type.resize(m_w * m_h);
	m_boatUpgradeType[2].type = {
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

	m_h = m_boatUpgradeType[index].height;
	m_w = m_boatUpgradeType[index].width;

	m_inventory.reserve(m_w * m_h);
	m_invenSlot.resize(m_w * m_h);

	//

	for (int th = 0; th < m_h; th++)
	{
		for (int tw = 0; tw < m_w; tw++)
		{
			_int tmp = static_cast<_int>(m_boatUpgradeType[index].type[th * m_w + tw]);
			switch (tmp)
			{
			case  static_cast<_int>('O'):
				m_invenSlot[th * m_w + tw].isLock = true;
				m_invenSlot[th * m_w + tw].slotType = SLOT_TYPE::END;
				break;
			case static_cast<_int>('A'):
				m_invenSlot[th * m_w + tw].isLock = false;
				m_invenSlot[th * m_w + tw].slotType = SLOT_TYPE::ANY;
				break;
			case static_cast<_int>('E'):
				m_invenSlot[th * m_w + tw].isLock = false;
				m_invenSlot[th * m_w + tw].slotType = SLOT_TYPE::ENGINE;
				break;
			case static_cast<_int>('L'):
				m_invenSlot[th * m_w + tw].isLock = false;
				m_invenSlot[th * m_w + tw].slotType = SLOT_TYPE::LIGHT;
				break;
			case static_cast<_int>('R'):
				m_invenSlot[th * m_w + tw].isLock = false;
				m_invenSlot[th * m_w + tw].slotType = SLOT_TYPE::ROT;
				break;
			case static_cast<_int>('N'):
				m_invenSlot[th * m_w + tw].isLock = false;
				m_invenSlot[th * m_w + tw].slotType = SLOT_TYPE::NET;
				break;
			}
			// 아 비트 플레그 해야 하나...
		}
	}

}

void CInventory::SetHighlightArea(ItemInst& itemInst, _uint baseX, _uint baseY, PLACE_COLOR color)
{
	//const Item_Def& def = CItemDB::GetInstance()->GetItemByID(itemInst.ItemDef_ID);
	//_bool hasOverlap = false;
	//int overlapID = {};
	//int absenceNum = {};
	//_uint ID_First = {};
	//_uint rot = itemInst.Rotation % 4;




	const ItemDef& def = CItemDB::GetInstance()->GetItemByID(itemInst.itemDefId);
	_uint rot = itemInst.rotation % 4;



	for (auto& slot : m_invenSlot)
	{
		slot.slotColor = PLACE_COLOR::END;
	}

	for (int i = 0; i < def.itemShape.occ[rot].size(); i++)
	{

		_uint fx = (def.itemShape.occ[rot][i].dx) + baseX;
		_uint fy = (def.itemShape.occ[rot][i].dy) + baseY;

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
			Slot& slot = m_invenSlot[fy * m_w + fx];
			slot.slotColor = PLACE_COLOR::GREEN;
		}
		break;
		case PLACE_COLOR::ORANGE:
		{
			Slot& slot = m_invenSlot[fy * m_w + fx];
			slot.slotColor = PLACE_COLOR::ORANGE;
		}
		break;

		case PLACE_COLOR::RED:
		{
			Slot& slot = m_invenSlot[fy * m_w + fx];
			slot.slotColor = PLACE_COLOR::RED;
		}
		break;
		}
	}



}

void CInventory::FixAll()
{
	for(auto& it : m_invenSlot)
	{
		it.isBroken = false;
	}
	dynamic_pointer_cast<CPlayerBoat>(m_pGOwner.lock())->SetHPFull();
}

shared_ptr<CInventory> CInventory::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CInventory> instance(new CInventory(device, context), [](CInventory* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInventory");
	}
	return instance;
}


shared_ptr<CComponent> CInventory::Clone(void* arg)
{
	shared_ptr<CInventory> instance(new CInventory(*this), [](CInventory* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CInventory");
		return nullptr;
	}
	return instance;
}
void CInventory::Free()
{
	__super::Free();
}
