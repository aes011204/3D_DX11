
#include "Data_Manager.h"




IMPLEMENT_SINGLETON(CData_Manager)

CData_Manager::CData_Manager()
{
}

HRESULT CData_Manager::Initialize()
{
	if (FAILED(Load_ItemDate("Item_Fish.json")))
		return E_FAIL;
	if (FAILED(Load_ItemDate("Item_Equipment.json")))
		return E_FAIL;
	if (FAILED(Load_ItemDate("Item_Material.json")))
		return E_FAIL;
	return S_OK;
}

const Item_Def& CData_Manager::GetItemByID(_uint ItemID)
{
	return m_vec_ItemDefs[GetIndexByID(ItemID)];
}

HRESULT CData_Manager::Load_ItemDate(const string& fileName)
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
		Item_Def item_def; // 구조체 지역으로 생성

		item_def.ItemID = node.value("ItemID", 0);//node["ItemID"].get<int> 이렇게 해도되긴 함

		{
			string str = node.value("ItemType", "END");
			auto Enum_Result = magic_enum::enum_cast<ITEM_TYPE>(str, magic_enum::case_insensitive);

			// 
			item_def.ItemType = Enum_Result.value();
		}

		item_def.ItemName = node.value("ItemName", "NO_DATA");
		item_def.ItemDesc = node.value("ItemDesc", "NO_DATA");
		item_def.TexturePath = node.value("TexturePath", "NO_DATA");

		if (node.contains("ItemShape"))
		{
			//for (auto& shape : node["ItemShape"])
			//{
			//	
			//auto& node = node["ItemShape"];

			item_def.ItemShape.Width = node["ItemShape"].value("Width", 0);
			item_def.ItemShape.Height = node["ItemShape"].value("Height", 0);

			//vector<vector<_ubyte>> mask = node["Shape_Mask"];

			item_def.ItemShape.Shape_Mask.resize(item_def.ItemShape.Width * item_def.ItemShape.Height);

			for (int h = 0; h < item_def.ItemShape.Height; h++)
			{
				for (int w = 0; w < item_def.ItemShape.Width;w++)
				{
					int mask = node["ItemShape"]["Shape_Mask"][h][w].get<int>(); // 일단 int로 받고 나중에 형변환
					if (mask != 1) { mask = 0; }
					item_def.ItemShape.Shape_Mask[h * item_def.ItemShape.Width + w] = static_cast<_ubyte>(mask);

				}

			}
			// 이건 변환 함수 필요함

	/*			item_def.ItemShape.Occ[0] =
				item_def.ItemShape.Occ[1] =
				item_def.ItemShape.Occ[2] =
				item_def.ItemShape.Occ[3] =*/
			//}

		}
		item_def.SizeNum = node.value("SizeNum", 0);
			//item_def.ItemShape.width * item_def.ItemShape.Height;// 이거하면 특이한 모양 불가 네모만 가능

		switch(item_def.ItemType)
		{
		case ITEM_TYPE::FISH:
			{
				Fish_Def fish_def;
				Parse_Fish_Def(fish_def, node);
				item_def.TypeDef = fish_def;
			break;
			}
		case ITEM_TYPE::EQUIP:
			{
			Equip_Def equip_def;
			Parse_Equip_Def(equip_def, node);
			item_def.TypeDef = equip_def;
			break;
			}
		case ITEM_TYPE::MATERIAL:
		{
			Material_Def material_def;
			material_def.Cost = node.value("Cost", 0);
			item_def.TypeDef = material_def;
			break;
		}
		case ITEM_TYPE::PURSUIT:
			// 없음 그냥 monostate로 ㄱ
			break;
		case ITEM_TYPE::TRINKET:
			{
			Trinket_Def trinket_def;
			trinket_def.Cost = node.value("Cost", 0);
			item_def.TypeDef = trinket_def;
			break;
			}
		}

		// 마지막 vec에 넣기
		m_vec_ItemDefs.push_back(item_def);
		m_map_ItemID.emplace(item_def.ItemID, m_vec_ItemDefs.size() - 1);
		
	}


	return S_OK;

}

void CData_Manager::Parse_Fish_Def(Fish_Def& fish_def, const nlohmann::json& node)
{

		fish_def.Cost = node.value("Cost", 0);

		_string str = node.value("FishTime", "END");
		TIME timeEnum = magic_enum::enum_cast<TIME>(str).value();
		fish_def.FishTime = timeEnum;

		fish_def.SeaType_Mask = BitFlag_SeaType(node, "SeaType_Mask");
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
			fish_def.vec_Mutation.resize(node["Mutation"].size());
			for (int i = 0; i < node["Mutation"].size(); i++)
			{
				auto& mut = node["Mutation"][i];
				fish_def.vec_Mutation[i].Mutation_ID = i + 1;
				fish_def.vec_Mutation[i].Species_ID = node.value("ItemID", 0);

				fish_def.vec_Mutation[i].MutName = mut.value("MutName", "a");
				fish_def.vec_Mutation[i].MutDesc = mut.value("MutDesc", "a");
				fish_def.vec_Mutation[i].MutTexturePath = mut.value("MutTexturePath", "a");
				fish_def.vec_Mutation[i].MutCost = mut.value("MutCost", 0);

			}
		}

		fish_def.IsRod = node.value("IsRod", false);
		fish_def.IsTrawl = node.value("IsTrawl", false);
		fish_def.IsPot = node.value("IsPot", false);
}
void CData_Manager::Parse_Equip_Def(Equip_Def& equip_def, const nlohmann::json& node)
{

	equip_def.Cost = node.value("Cost", 0);
	equip_def.InstallTime = node.value("InstallTime", 0);


	_string str = node.value("EquipType", "END");
	EQUIP_TYPE typeEnum = magic_enum::enum_cast<EQUIP_TYPE>(str).value();
	equip_def.EquipType = typeEnum;

	auto& EffNode = node["EquipType_Effect"];

	switch (equip_def.EquipType)
	{
	case EQUIP_TYPE::ENGINE:
		{
		Equip_Engine effect;
		effect.Research_Lv = EffNode.value("Research_Lv", 0);
		effect.Speed_Kn = EffNode.value("Speed_Kn", 0);
		equip_def.EquipType_Effect = effect;
	
		break;
		}
	case EQUIP_TYPE::LIGHT:
	{
		Equip_Light effect;
		effect.Lumen = EffNode.value("Lumen", 0);
		effect.Range = EffNode.value("Range", 0);
		equip_def.EquipType_Effect = effect;

		break;
	}
	case EQUIP_TYPE::ROD:
	{
		Equip_Rod effect;
		effect.Research_Lv = EffNode.value("Research_Lv", 0);
			effect.SeaType_Mask = BitFlag_SeaType(EffNode, "SeaType_Mask");
		effect.Speed_Per = EffNode.value("Speed_Per", 0);
		equip_def.EquipType_Effect = effect;

		break;
	}
	case EQUIP_TYPE::NET:
	{
		Equip_Net effect;
		effect.Research_Lv = EffNode.value("Research_Lv", 0);
		effect.SeaType_Mask = BitFlag_SeaType(EffNode, "SeaType_Mask");
		effect.Cap_SizeX = EffNode.value("Cap_SizeX", 0);
		effect.Cap_SizeY = EffNode.value("Cap_SizeY", 0);
		effect.Yield = EffNode.value("Yield", 0);
		effect.LastFor = EffNode.value("LastFor", 0);

		equip_def.EquipType_Effect = effect;

		break;
	}
	case EQUIP_TYPE::CRAP:
	{
		Equip_CrapPot effect;
		effect.Research_Lv = EffNode.value("Research_Lv", 0);
		effect.Cap_SizeX = EffNode.value("Cap_SizeX", 0);
		effect.Cap_SizeY = EffNode.value("Cap_SizeY", 0);
		effect.Yield = EffNode.value("Yield", 0);
		effect.LastFor = EffNode.value("LastFor", 0);

		equip_def.EquipType_Effect = effect;

		break;
	}
	}
}

SEA_MASK CData_Manager::BitFlag_SeaType(const nlohmann::json& node,const string str)
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

void CData_Manager::Compute_Occ(Shape& shape, _uint w, _uint h)
{

	_uint _w = {w};
	_uint _h = {h};
	for(_uint y =0; y < shape.Height; y++)
	{
		for(_uint x = 0; x < shape.Width; x++)
		{
			if(shape.Shape_Mask[y*w+x] == 1)
			{
				shape.Occ[0].push_back({ x,y });
				
			}
		}
	}

	// 공식 90도 회전 = [x,y]->[h-1-y,x]
	//Rotation 90
	_w = h;_h = w;
	for (_uint i = 0; i < shape.Occ[0].size(); i++)
	{
		shape.Occ[1].push_back({ _h - 1 - shape.Occ[0][i].dy,shape.Occ[0][i].dx });
	}

	//Rotation 180
	_w = h;_h = w;
	for (_uint i = 0; i < shape.Occ[0].size(); i++)
	{
		shape.Occ[2].push_back({ _h - 1 - shape.Occ[1][i].dy,shape.Occ[1][i].dx });
	}

	//Rotation 270
	_w = h;_h = w;
	for (_uint i = 0; i < shape.Occ[0].size(); i++)
	{
		shape.Occ[3].push_back({ _h - 1 - shape.Occ[2][i].dy,shape.Occ[2][i].dx });
	}
	

}


wstring CData_Manager::Utf8ToWstring(const string& str)
{
	if (str.empty())
		return wstring();

	_int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

	wstring result(size_needed, 0);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size_needed);

	if (!result.empty() && result.back() == L'\0')
		result.pop_back();

	return result;
}

_uint CData_Manager::GetIndexByID(_uint id)
{
	auto it = m_map_ItemID.find(id);
	assert(it != m_map_ItemID.end());
	return it->second;
}

CData_Manager* CData_Manager::Create()
{
	CData_Manager* pInstance = new CData_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("FAILED CREATE DATA_MANAGER");
	}

	return pInstance;

}

void CData_Manager::Free()
{
	__super::Free();
}
