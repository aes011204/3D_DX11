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
    Render_SaveLoadPanel();
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

void  CMapTool::Render_SaveLoadPanel()
{
    static const string kFolder = "../../Client/Bin/Resources/Data/MapData/";

    static vector<string> s_files;
    static int  s_selected = -1;
    static char s_newFileName[128] = "Stage1.json";

    // 최초 1회 로딩
    if (s_files.empty())
        s_files = ListJsonFiles(kFolder);

    ImGui::Text("MapData Folder:");
    ImGui::TextDisabled("%s", kFolder.c_str());

    ImGui::Separator();

    // 상단 버튼들
    if (ImGui::Button("Refresh"))
    {
        s_files = ListJsonFiles(kFolder);
        if (s_selected >= (int)s_files.size()) s_selected = -1;
    }


    ImGui::Separator();

    // 파일 리스트
    ImGui::Text("Files (*.json)");
    ImVec2 listSize(0, 220);

    if (ImGui::BeginListBox("##MapDataFiles", listSize))
    {
        for (int i = 0; i < (int)s_files.size(); ++i)
        {
            const bool isSelected = (s_selected == i);
            if (ImGui::Selectable(s_files[i].c_str(), isSelected))
                s_selected = i;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    // 선택된 파일 표시
    string selectedFile = (s_selected >= 0 && s_selected < (int)s_files.size()) ? s_files[s_selected] : "";
    string selectedFullPath = selectedFile.empty() ? "" : (kFolder + selectedFile);

    ImGui::Separator();

    // Load
    {
        bool canLoad = !selectedFullPath.empty();

        //if (!canLoad) ImGui::BeginDisabled();

        if (ImGui::Button("Load Selected"))
        {
            if (!canLoad)
            {
                CGameInstance::GetInstance()->ClearMap(SAVETYPE::GAMEOBJECT);
            };

            // Load는 fullPath를 넣는 게 편함 (네 Load_ObjData가 ifstream(fileName)이라서)
            if (false==(CGameInstance::GetInstance()->Load(SAVETYPE::GAMEOBJECT,selectedFile)))
            {
                // MSG_BOX는 DataManager 안에서 이미 띄우지만, 필요하면 여기서도 추가 처리 가능
            }
        }
        //if (!canLoad) ImGui::EndDisabled();
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
        if (false == (CGameInstance::GetInstance()->Save(SAVETYPE::GAMEOBJECT, fn)))
        {
            // 실패 처리
        }

        // 저장 후 리스트 갱신
        s_files = ListJsonFiles(kFolder);

        // 저장한 파일 선택해주기(편의)
        auto it = find(s_files.begin(), s_files.end(), fn);
        if (it != s_files.end())
            s_selected = (int)distance(s_files.begin(), it);
    }

    ImGui::SameLine();

    // 선택 파일 덮어쓰기 버튼
    {
        bool canOverwrite = !selectedFile.empty();
        if (!canOverwrite) ImGui::BeginDisabled();

        if (ImGui::Button("Overwrite Selected"))
        {
            // 선택된 파일명으로 저장(덮어쓰기)
            if (false == (CGameInstance::GetInstance()->Save(SAVETYPE::GAMEOBJECT, selectedFile)))
            {
                // 실패 처리
            }
            s_files = ListJsonFiles(kFolder);
        }

        if (!canOverwrite) ImGui::EndDisabled();
    }

    // 선택 파일 삭제까지 넣고 싶으면 여기 추가 가능
}

shared_ptr<CMapTool> CMapTool::Create()
{
    return make_shared <CMapTool>();
}
