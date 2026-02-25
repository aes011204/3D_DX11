#include "Data_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#include "UI_Manager.h"
#include "Entity.h"
#include "Engine_Helper.h"


CData_Manager::CData_Manager(): m_pGameInstance(CGameInstance::GetInstance())
{
}
CData_Manager::~CData_Manager() 
{
	Free();
}
HRESULT CData_Manager::Initialize(_uint  EditorLevel)
{
	m_EditorLevel = EditorLevel;
    return S_OK;
}

bool CData_Manager::Reload(const _tchar* Path)
{
	//if (CurLoadedMapPath == nullptr)
	//{
	//	Load_ObjData()
	//}
	//else
	//{

	//if (CurLoadedMapPath == Path)
	//	return true;

	//ClearMap();
	//Load_ObjData();

	//}

	// 이건그냥 임구이에서 하면 될듯


	return true;
}

bool CData_Manager::ClearMap(SAVETYPE eDATATYPE)
{

	if (eDATATYPE == SAVETYPE::END)
		return false;

	_uint iCurLevel = m_pGameInstance.lock()->Get_Current_LevelIdx();
	if (eDATATYPE == SAVETYPE::GAMEOBJECT)
	{
		const auto& layer = m_pGameInstance.lock()->Get_GameObjects(iCurLevel);

		nlohmann::json jObjArray = nlohmann::json::array();

		for (auto& pair : layer)
		{
			_wstring strLayerName = pair.first;

			for (auto& pObj : pair.second->Get_GameObject())
			{
				if (pObj->Get_SaveType() == SAVETYPE::GAMEOBJECT)
				{
					pObj->Mark_Destroy();
				}
			}
		}
	}
	else// ui
	{

	}

	return false;
}

bool CData_Manager::Load(SAVETYPE eDATATYPE, const string& fileName)
{

	if (eDATATYPE == SAVETYPE::END)
		return false;

	if (eDATATYPE == SAVETYPE::GAMEOBJECT)
	{
		Load_ObjData(fileName);
	}
	else// ui
	{
		Load_UIData(fileName);
	}

	return false;
}

bool CData_Manager::Save(SAVETYPE eDATATYPE, const string& fileName)
{
	if (eDATATYPE == SAVETYPE::END)
		return false;

	if (eDATATYPE == SAVETYPE::GAMEOBJECT)
	{
		Save_ObjData(fileName);
	}
	else// ui
	{
		Save_UIData(fileName);
	}

	return false;
}


HRESULT CData_Manager::Load_ObjData(const string& fileName)
{
	const string folderPath = "../../Client/Bin/Resources/Data/MapData/";
	const string fullPath = folderPath + fileName;

	ifstream file(fullPath, ios_base::in);


	if (!file.is_open())
	{
		MSG_BOX("DataJson Load Failed");
		return E_FAIL;
	}

	nlohmann::json j;

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
	
	if (j["Level"] != m_EditorLevel)
	{
		MSG_BOX("This File is not Saved IN Editor");
		return E_FAIL;
	}
	if (j.contains("WorldGameObject"))
	{
		for (auto& jObj : j["WorldGameObject"])
		{
			_uint Level = m_pGameInstance.lock()->Get_Current_LevelIdx();
			shared_ptr<CGameObject> obj = m_pGameInstance.lock()->Add_GameObject(jObj["ProtoSavedLevel"], S2W(jObj["ProtoSavedTag"]), Level, S2W(jObj["LayerName"]));


			// 컴포넌트는 Load_FromJson, Save_ToJson 각각 선언 / object 는 Entity에서 Load_FromJson 만
			obj->Load_FromJson(jObj);// 컴포넌트 초기화 + 컴포넌트 추가 전부 이 안에서 해결

			/*for (auto& pair : obj->Get_ComponentMap())
			{*/
			//pair.first == jObj["Components"]["ComponentTag"]
			//pair.second->Load_FromJson(jObj["Components"]);
		//}
			//for (auto& jCom : j["Components"])
			//{

			//	//obj->Add_Component(jCom["ComProtoLevel"], S2W(jCom["ComProtoTag"]), S2W(jCom["ComponentTag"]),, nullptr);

			//	(obj->Get_ComponentMap().find(S2W(jCom["ComponentTag"])))->second->Load_FromJson(jCom);
			//}
			obj->Set_SaveType(SAVETYPE::GAMEOBJECT);

		}
	}

    return S_OK;
}


HRESULT CData_Manager::Save_ObjData(const string& fileName)
{
	nlohmann::json j;
	_uint iCurLevel = m_pGameInstance.lock()->Get_Current_LevelIdx();
	

	j["Level"] = iCurLevel;
	//obj
	const auto& layer = m_pGameInstance.lock()->Get_GameObjects(iCurLevel);

	nlohmann::json jObjArray = nlohmann::json::array();

	for (auto& pair : layer)
	{
		_wstring strLayerName = pair.first;
		

		for (auto& pObj : pair.second->Get_GameObject())
		{
			if (pObj->IsSavableClass() != true)
				continue;

			nlohmann::json jObj;

			jObj["LayerName"] = W2S(strLayerName);
			

			jObj["ProtoSavedTag"] = W2S(pObj->Get_ProtoTag()); // "ProtoDATATYPE_Cube" 등
			jObj["ProtoSavedLevel"] = pObj->Get_ProtoLevel(); // 
			jObj["ObjName"] = W2S(pObj->Get_Name());   // "Player", "Enemy1" 등

			
			nlohmann::json jComponentArray = nlohmann::json::array();

				for (auto& pair : pObj->Get_ComponentMap())
				{
					nlohmann::json jCom;
					// 컴포넌트에 공통적으로 들어가는거

					jCom["ComProtoTag"] = W2S(pair.second->Get_ProtoTag());
					jCom["ComProtoLevel"] = pair.second->Get_ProtoLevel();



					jCom["ComponentTag"] = W2S(pair.first);
					// 각 컴포넌트 안의 세부내용
					pair.second->Save_ToJson(jCom);

					jComponentArray.push_back(jCom);
				}
			
				jObj["Components"] = jComponentArray;
			//
			jObjArray.push_back(jObj);
		}
	}
	j["WorldGameObject"] = jObjArray;

	string folderPath = "../../Client/Bin/Resources/Data/MapData/";

	// 폴더가 없으면 생성
	if (!filesystem::exists(folderPath)) {
		filesystem::create_directories(folderPath);
	}
	
	string fullPath = folderPath + fileName; //  folderPath + "Stage1.json"

	// 파일 생성
	ofstream file(fullPath);

	if (file.is_open())
	{

		file << setw(4) << j << endl;

		file.close();
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CData_Manager::Load_UIData(const string& fileName)
{
	//ifstream fin(fileName);
	//if (!fin.is_open()) return;

	//nlohmann::json j;
	//fin >> j;

	//_uint LevelIndex = j["CurLevel"];

	//for (const auto& jLayer : j["Layer"])
	//{
	//	const _wstring CurLayer = jLayer["LayerName"];                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             LayerName = jLayer["LayerName"];

	//	for (const auto& jObj : jLayer["GamObject"])
	//	{


	//			if (FAILED(
	//				m_pGameInstance.lock()->Add_GameObject(PROTODATATYPE::GAMEOBJECT, jObj["Level"], jObj["ProtoSavedLevel"],
	//					LevelIndex, CurLayer, nullptr )))
	//			{
	//				return E_FAIL;
	//			};
	//		for (const auto& jCom : jObj["Component"])
	//		{
	//			m_pGameInstance.lock()->Get_GameObjects(LevelIndex).

	//		}
	//	}

	//obj->Set_SaveType(SAVETYPE::UI);
	//}
	return S_OK;
}

HRESULT CData_Manager::Save_UIData(const string& fileName)
{
	//nlohmann::json j;


	////ui
	//const auto& uiMgr = m_pGameInstance.lock()->Get_UI_Manager();

	//for (int i = 0; i < ETOI(UI_LAYER::END);i++)
	//{
	//	auto& uiVec = uiMgr->GetUIList(static_cast<UI_LAYER>(i));
	//	j["LayerName"] = magic_enum::enum_name(static_cast<UI_LAYER>(i));

	//	for (auto& it : uiVec)
	//	{
	//		jObj["ProtoTag"] = obj->Get_ProtoTag(); // "ProtoDATATYPE_Cube" 등
	//		jObj["UITag"] = obj->Get_ObjTag();   // "Player", "Enemy1" 등
	//		obj->Save_ToJson(j);
	//	}

	//}
	return S_OK;
}

unique_ptr<CData_Manager> CData_Manager::Create(_uint  EditorLevel)
{
	unique_ptr<CData_Manager> pInstance(new CData_Manager());

	if (FAILED(pInstance->Initialize(EditorLevel)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
	}
	return pInstance;
}

void CData_Manager::Free()
{
}
