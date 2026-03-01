#include "MapTool.h"
#include "Data_Manager.h"
#include "GameInstance.h"

CMapTool::CMapTool()
	:CEditor_Win("MapTool")
{
}

CMapTool::~CMapTool()
{
}

HRESULT CMapTool::Initialize()
{
	return S_OK;
}

void CMapTool::Update(float fTimeDelta)
{
}

void CMapTool::Render()
{
	ImGui::Begin("MapTool");

	// 상단 탭 바 생성
	if (ImGui::BeginTabBar("MapToolTabs"))
	{
		// 1. 게임 오브젝트 탭
		if (ImGui::BeginTabItem("Game Objects"))
		{
			// 기존 폴더 경로 사용
			static const string kObjFolder = "../../Client/Bin/Resources/Data/MapData/";
			Render_SaveLoadPanel(SAVETYPE::GAMEOBJECT, kObjFolder);
			ImGui::EndTabItem();
		}

		// 2. UI 데이터 탭
		if (ImGui::BeginTabItem("UI Editor Data"))
		{
			// UI 전용 폴더 경로 (구분을 위해 경로 변경)
			static const string kUIFolder = "../../Client/Bin/Resources/Data/UIData/";
			Render_SaveLoadPanel(SAVETYPE::UI, kUIFolder);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

static vector<string> ListJsonFiles(const string& folderPath)
{
	vector<string> out;

	if (!filesystem::exists(folderPath))
		return out;

	for (const auto& entry : filesystem::directory_iterator(folderPath))
	{
		if (!entry.is_regular_file())
			continue;

		const auto ext = entry.path().extension().string(); // ".json"
		if (ext == ".json" || ext == ".JSON")
		{
			// 파일명만(예: Stage1.json)
			out.push_back(entry.path().filename().string());
		}
	}

	sort(out.begin(), out.end());
	return out;
}

void  CMapTool::Render_SaveLoadPanel(SAVETYPE eType, const string& folderPath)
{
	_uint iIdx = ETOI(eType);


	// 최초 1회 로딩
	if (s_files[iIdx].empty())
		s_files[iIdx] = ListJsonFiles(folderPath);

	ImGui::Text("MapData Folder:");
	ImGui::TextDisabled("%s", folderPath.c_str());

	// 상단 버튼들
	if (ImGui::Button("Refresh"))
	{
		s_files[iIdx] = ListJsonFiles(folderPath);
		if (s_selected[iIdx] >= (int)s_files[iIdx].size()) s_selected[iIdx] = -1;
	}


	ImGui::Separator();

	// 파일 리스트
	ImGui::Text("Files (*.json)");
	ImVec2 listSize(0, 220);

	if (ImGui::BeginListBox("##MapDataFiles", listSize))
	{
		for (int i = 0; i < (int)s_files[iIdx].size(); ++i)
		{
			const bool isSelected = (s_selected[iIdx] == i);
			if (ImGui::Selectable(s_files[iIdx][i].c_str(), isSelected))
				s_selected[iIdx] = i;

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	// 선택된 파일 표시
	string selectedFile = (s_selected[iIdx] >= 0 && s_selected[iIdx] < (int)s_files[iIdx].size())
		? s_files[iIdx][s_selected[iIdx]] : "";
	string selectedFullPath = selectedFile.empty() ? "" : (folderPath + selectedFile);

	ImGui::Separator();

	// Load

	{
		bool canLoad = !selectedFullPath.empty();

		if (ImGui::Button("Load Selected"))
		{
			if (canLoad)
			{
				// 2. 기존 맵/UI 데이터 청소
				CGameInstance::GetInstance()->ClearMap(eType);

				// 3. 데이터 로드 (이제 Load 함수로 진입합니다!)
				CGameInstance::GetInstance()->Load(eType, selectedFile);
			}
			else
			{
				MSG_BOX("select file");
			}

		}
	}

	ImGui::Spacing();
	ImGui::Separator();

	// Save
	ImGui::Text("Save As / Overwrite");
	ImGui::InputText("File name", s_newFileName, sizeof(s_newFileName));

	// 확장자 자동 보정(원하면)
	auto EnsureJsonExt = [](string name) -> string
		{
			if (name.size() < 5) return name + ".json";
			string lower = name;
			transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
			if (lower.rfind(".json") == lower.size() - 5)
				return name;
			return name + ".json";
		};

	if (ImGui::Button("Save"))
	{
		string fn = EnsureJsonExt(s_newFileName);

		// Save_ObjData는 내부에서 folderPath + fileName 붙이니까 "파일명만" 넘김
		if (true == (CGameInstance::GetInstance()->Save(eType, fn)))
		{
			// 저장 후 리스트 갱신
			s_files[iIdx] = ListJsonFiles(folderPath);

			// 저장한 파일 선택해주기(편의)
			auto it = find(s_files[iIdx].begin(), s_files[iIdx].end(), fn);
			if (it != s_files[iIdx].end())
				s_selected[iIdx] = (int)distance(s_files[iIdx].begin(), it);
		}

	}

	ImGui::SameLine();

	// 선택 파일 덮어쓰기 버튼
	{
		bool canOverwrite = !selectedFile.empty();
		if (!canOverwrite) ImGui::BeginDisabled();

		if (ImGui::Button("Overwrite Selected"))
		{
			// 선택된 파일명으로 저장(덮어쓰기)
			CGameInstance::GetInstance()->Save(eType, selectedFile);
			
			s_files[iIdx] = ListJsonFiles(folderPath);
		}

		if (!canOverwrite) ImGui::EndDisabled();
	}

}

shared_ptr<CMapTool> CMapTool::Create()
{
	return make_shared <CMapTool>();
}
