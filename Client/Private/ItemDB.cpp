
#include "ItemDB.h"
#include "GameInstance.h"
#include "Texture.h"




IMPLEMENT_SINGLETON(CItemDB)

CItemDB::CItemDB() : m_gameInstance(CGameInstance::GetInstance())
{
}

HRESULT CItemDB::Initialize()
{
	if (FAILED(Load_ItemData("Item_Fish.json")))
		return E_FAIL;
	if (FAILED(Load_ItemData("Item_Equipment.json")))
		return E_FAIL;
	if (FAILED(Load_ItemData("Item_Material.json")))
		return E_FAIL;
	return S_OK;
}

const ItemDef& CItemDB::GetItemByID(_uint itemId)
{
	return m_vecItemDefs[GetIndexByID(itemId)];
}

HRESULT CItemDB::Load_ItemData(const string& fileName)
{
	ifstream file(fileName, ios_base::in);

	if (!file.is_open())
	{
		MSG_BOX("itemJson Load Failed");
		return E_FAIL;
	}

	nlohmann::json j;

	//// BOM 제거 처리
	//char c;
	//file.read(&c, 1);

	//if (static_cast<unsigned char>(c) != 0xEF) // BOM의 첫 바이트가 0xEF가 아니면
	//{
	//    // BOM 없음 → 다시 스트림 처음으로 이동
	//    file.seekg(0);
	//}
	//else
	//{
	//    // BOM 존재 → 나머지 2바이트 제거
	//    char c2, c3;
	//    file.read(&c2, 1);
	//    file.read(&c3, 1);
	//    // 이제 f는 JSON 본문 위치에 정확히 있음
	//}

	// 파일 읽기
	try
	{
		file >> j;  // 여기서 문법 이상하면 바로 예외 발생
	}
	catch (const nlohmann::json::parse_error& e)
	{
		MessageBoxA(nullptr, e.what(), "JSON Parse Error", MB_OK);
		return E_FAIL;
	}


	for (auto& node : j["Items"])
	{
		ItemDef itemDef; // 구조체 지역으로 생성

		itemDef.itemId = node.value("ItemID", 0);//node["ItemID"].get<int> 이렇게 해도되긴 함

		{
			string str = node.value("ItemType", "END");
			auto enumResult = magic_enum::enum_cast<ITEM_TYPE>(str, magic_enum::case_insensitive);

			// 
			itemDef.itemType = enumResult.value();
		}

		itemDef.itemName = node.value("ItemName", "NO_DATA");
		itemDef.itemDesc = node.value("ItemDesc", "NO_DATA");
		itemDef.texturePath = node.value("TexturePath", "NO_DATA");

		if (node.contains("ItemShape"))
		{
			//for (auto& shape : node["ItemShape"])
			//{
			//	
			//auto& node = node["ItemShape"];

			itemDef.itemShape.width = node["ItemShape"].value("Width", 0);
			itemDef.itemShape.height = node["ItemShape"].value("Height", 0);

			//vector<vector<_ubyte>> mask = node["Shape_Mask"];

			itemDef.itemShape.shapeMask.resize(itemDef.itemShape.width * itemDef.itemShape.height);

			for (int h = 0; h < itemDef.itemShape.height; h++)
			{
				for (int w = 0; w < itemDef.itemShape.width; w++)
				{
					int mask = node["ItemShape"]["Shape_Mask"][h][w].get<int>(); // 일단 int로 받고 나중에 형변환
					if (mask != 1) { mask = 0; }
					itemDef.itemShape.shapeMask[h * itemDef.itemShape.width + w] = static_cast<_ubyte>(mask);

				}

			}
			// 이건 변환 함수 필요함

	/*			item_def.ItemShape.Occ[0] =
				item_def.ItemShape.Occ[1] =
				item_def.ItemShape.Occ[2] =
				item_def.ItemShape.Occ[3] =*/
				//}
			Compute_Occ(itemDef.itemShape, itemDef.itemShape.width, itemDef.itemShape.height);
		}
		itemDef.sizeNum = node.value("SizeNum", 0);
		//item_def.ItemShape.width * item_def.ItemShape.Height;// 이거하면 특이한 모양 불가 네모만 가능

		switch (itemDef.itemType)
		{
		case ITEM_TYPE::FISH:
		{
			FishDef fishDef;
			Parse_Fish_Def(fishDef, node);
			itemDef.typeDef = fishDef;
			break;
		}
		case ITEM_TYPE::EQUIP:
		{
			EquipDef equipDef;
			Parse_Equip_Def(equipDef, node);
			itemDef.typeDef = equipDef;
			break;
		}
		case ITEM_TYPE::MATERIAL:
		{
			MaterialDef materialDef;
			materialDef.cost = node.value("Cost", 0);
			itemDef.typeDef = materialDef;
			break;
		}
		case ITEM_TYPE::PURSUIT:
			// 없음 그냥 monostate로 ㄱ
			break;
		case ITEM_TYPE::TRINKET:
		{
			TrinketDef trinketDef;
			trinketDef.cost = node.value("Cost", 0);
			itemDef.typeDef = trinketDef;
			break;
		}
		}

		// 일단 이러면 복사된 택스쳐의 얕은 복사인데 일단 문제는 없을듯 문제 생기면 수정
		itemDef.texture = dynamic_pointer_cast<CTexture>(m_gameInstance.lock()->
			Clone_Prototype(PROTOTYPE::COMPONENT, ETOI(LEVEL::STATIC), S2W(itemDef.texturePath)));




		// 마지막 vec에 넣기
		m_vecItemDefs.push_back(itemDef);
		m_mapItemId.emplace(itemDef.itemId, m_vecItemDefs.size() - 1);

	}


	return S_OK;

}

void CItemDB::Parse_Fish_Def(FishDef& fishDef, const nlohmann::json& node)
{

	fishDef.cost = node.value("Cost", 0);

	_string str = node.value("FishTime", "END");
	TIME timeEnum = magic_enum::enum_cast<TIME>(str).value();
	fishDef.fishTime = timeEnum;

	fishDef.seaTypeMask = BitFlag_SeaType(node, "SeaType_Mask");
	//for (const string& str : node["SeaType_Mask"]) //.get<_string>() 이건 안됨 배열로 저장 되어 있는거라
	//{
	//	if (str == "COASTAL") { fish_def.SeaType_Mask |= SEA_MASK(SEA_TYPE::COASTAL); }
	//	else if (str == "SHALLOW") { fish_def.SeaType_Mask |= SEA_MASK(SEA_TYPE::SHALLOW); }
	//	else if (str == "OCEANIC") { fish_def.SeaType_Mask |= SEA_MASK(SEA_TYPE::OCEANIC); }
	//	else if (str == "ABYSSAL") { fish_def.SeaType_Mask |= SEA_MASK(SEA_TYPE::ABYSSAL); }
	//	else if (str == "HADAL") { fish_def.SeaType_Mask |= SEA_MASK(SEA_TYPE::HADAL); }
	//	else if (str == "MANGROVE") { fish_def.SeaType_Mask |= SEA_MASK(SEA_TYPE::MANGROVE); }
	//	else if (str == "VOLCANIC") { fish_def.SeaType_Mask |= SEA_MASK(SEA_TYPE::VOLCANIC); }
	//	else if (str == "ICE") { fish_def.SeaType_Mask |= SEA_MASK(SEA_TYPE::ICE); }
	//	else if (str == "CRAB") { fish_def.SeaType_Mask |= SEA_MASK(SEA_TYPE::CRAB); }
	//}

	if (node.contains("Mutation"))
	{
		fishDef.vecMutation.resize(node["Mutation"].size());
		for (int i = 0; i < node["Mutation"].size(); i++)
		{
			auto& mut = node["Mutation"][i];
			fishDef.vecMutation[i].mutationId = i + 1;
			fishDef.vecMutation[i].speciesId = node.value("ItemID", 0);

			fishDef.vecMutation[i].mutName = mut.value("MutName", "a");
			fishDef.vecMutation[i].mutDesc = mut.value("MutDesc", "a");
			fishDef.vecMutation[i].mutTexturePath = mut.value("MutTexturePath", "a");
			fishDef.vecMutation[i].mutCost = mut.value("MutCost", 0);


			fishDef.vecMutation[i].texture = dynamic_pointer_cast<CTexture>(m_gameInstance.lock()->
				Clone_Prototype(PROTOTYPE::COMPONENT, ETOI(LEVEL::STATIC), S2W(fishDef.vecMutation[i].mutTexturePath)));

		}
	}

	fishDef.isRod = node.value("IsRod", false);
	fishDef.isTrawl = node.value("IsTrawl", false);
	fishDef.isPot = node.value("IsPot", false);
}
void CItemDB::Parse_Equip_Def(EquipDef& equipDef, const nlohmann::json& node)
{

	equipDef.cost = node.value("Cost", 0);
	equipDef.installTime = node.value("InstallTime", 0);


	_string str = node.value("EquipType", "END");
	EQUIP_TYPE typeEnum = magic_enum::enum_cast<EQUIP_TYPE>(str).value();
	equipDef.equipType = typeEnum;

	auto& effNode = node["EquipType_Effect"];

	switch (equipDef.equipType)
	{
	case EQUIP_TYPE::ENGINE:
	{
		EquipEngine effect;
		effect.researchLv = effNode.value("Research_Lv", 0);
		effect.speedKn = effNode.value("Speed_Kn", 0);
		equipDef.equipTypeEffect = effect;

		break;
	}
	case EQUIP_TYPE::LIGHT:
	{
		EquipLight effect;
		effect.lumen = effNode.value("Lumen", 0);
		effect.range = effNode.value("Range", 0);
		equipDef.equipTypeEffect = effect;

		break;
	}
	case EQUIP_TYPE::ROD:
	{
		EquipRod effect;
		effect.researchLv = effNode.value("Research_Lv", 0);
		effect.seaTypeMask = BitFlag_SeaType(effNode, "SeaType_Mask");
		effect.speedPer = effNode.value("Speed_Per", 0);
		equipDef.equipTypeEffect = effect;

		break;
	}
	case EQUIP_TYPE::NET:
	{
		EquipNet effect;
		effect.researchLv = effNode.value("Research_Lv", 0);
		effect.seaTypeMask = BitFlag_SeaType(effNode, "SeaType_Mask");
		effect.capSizeX = effNode.value("Cap_SizeX", 0);
		effect.capSizeY = effNode.value("Cap_SizeY", 0);
		effect.yield = effNode.value("Yield", 0);
		effect.lastFor = effNode.value("LastFor", 0);

		equipDef.equipTypeEffect = effect;

		break;
	}
	case EQUIP_TYPE::CRAP:
	{
		EquipCrabPot effect;
		effect.researchLv = effNode.value("Research_Lv", 0);
		effect.capSizeX = effNode.value("Cap_SizeX", 0);
		effect.capSizeY = effNode.value("Cap_SizeY", 0);
		effect.yield = effNode.value("Yield", 0);
		effect.lastFor = effNode.value("LastFor", 0);

		equipDef.equipTypeEffect = effect;

		break;
	}
	}
}

SEA_MASK CItemDB::BitFlag_SeaType(const nlohmann::json& node, const string str)
{
	SEA_MASK mask = 0;
	if (!node.contains(str) || !node.at(str).is_array())
		return mask;
	for (const auto& v : node.at(str)) //.get<_string>() 이건 안됨 배열로 저장 되어 있는거라
	{
		string name = v.get<std::string>();


		if (name == "COASTAL") { mask |= Set_SeaType_Mask(SEA_TYPE::COASTAL); }
		else if (name == "SHALLOW") { mask |= Set_SeaType_Mask(SEA_TYPE::SHALLOW); }
		else if (name == "OCEANIC") { mask |= Set_SeaType_Mask(SEA_TYPE::OCEANIC); }
		else if (name == "ABYSSAL") { mask |= Set_SeaType_Mask(SEA_TYPE::ABYSSAL); }
		else if (name == "HADAL") { mask |= Set_SeaType_Mask(SEA_TYPE::HADAL); }
		else if (name == "MANGROVE") { mask |= Set_SeaType_Mask(SEA_TYPE::MANGROVE); }
		else if (name == "VOLCANIC") { mask |= Set_SeaType_Mask(SEA_TYPE::VOLCANIC); }
		else if (name == "ICE") { mask |= Set_SeaType_Mask(SEA_TYPE::ICE); }
		else if (name == "CRAB") { mask |= Set_SeaType_Mask(SEA_TYPE::CRAB); }
	}
	return mask;
}

void CItemDB::Compute_Occ(Shape& shape, _uint w, _uint h)
{
	shape.occ[0].clear();
	shape.occ[1].clear();
	shape.occ[2].clear();
	shape.occ[3].clear();

	_uint _w = { w };
	_uint _h = { h };
	for (_uint y = 0; y < shape.height; y++)
	{
		for (_uint x = 0; x < shape.width; x++)
		{
			if (shape.shapeMask[y * w + x] == 1)
			{
				shape.occ[0].push_back({ x,y });

			}
		}
	}

	// 공식 90도 회전 = [x,y]->[h-1-y,x]
	//Rotation 90
	_w = h; _h = w;

	// 90도 회전: (x, y) -> (h-1-y, x) 
	for (auto& pos : shape.occ[0])
		shape.occ[1].push_back({ h - 1 - pos.dy, pos.dx });

	// 180도 회전: (x, y) -> (w-1-y, x) 
	for (auto& pos : shape.occ[1])
		shape.occ[2].push_back({ w - 1 - pos.dy, pos.dx });

	// 270도 회전: (x, y) -> (h-1-y, x) 
	for (auto& pos : shape.occ[2])
		shape.occ[3].push_back({ h - 1 - pos.dy, pos.dx });

}


wstring CItemDB::Utf8ToWstring(const string& str)
{
	if (str.empty())
		return wstring();

	_int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

	wstring result(sizeNeeded, 0);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], sizeNeeded);

	if (!result.empty() && result.back() == L'\0')
		result.pop_back();

	return result;
}

_uint CItemDB::GetIndexByID(_uint id)
{
	auto it = m_mapItemId.find(id);
	assert(it != m_mapItemId.end());
	return it->second;
}

//shared_ptr<CData_Manager> CData_Manager::Create()
//{
//	shared_ptr<CData_Manager> pInstance ( new CData_Manager());
//	if (FAILED(pInstance->Initialize()))
//	{
//		MSG_BOX("FAILED CREATE DATA_MANAGER");
//	}
//
//	return pInstance;
//
//}

void CItemDB::Free()
{
	__super::Free();
}
