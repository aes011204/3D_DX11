#include "InspectorWin.h"
#include "EditorInstance.h"


#include "Entity.h"
#include "Component.h"




CInspectorWin::CInspectorWin()
    : CEditor_Win("Inspector")
{
    m_NameBuf[0] = '\0';
}

_string CInspectorWin::ConvertW2A(const wstring& wstr)
{
    if (wstr.empty()) return "";

    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size <= 0) return "";

    _string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);

    // WideCharToMultiByte는 널 포함 길이를 주니까 마지막 널 제거
    if (!str.empty() && str.back() == '\0')
        str.pop_back();

    return str;
}

void CInspectorWin::Render()
{
    ImGui::Begin("Inspector");

    ESelectType type = CEditorInstance::GetInstance()->GetType();
    if (type == ESelectType::NONE)
    {
        ImGui::TextDisabled("(Nothing Selected)");
        ImGui::End();
        return;
    }

    if (type == ESelectType::ENTITY)
    {
        auto obj = CEditorInstance::GetInstance()->GetEntity();
        if (!obj)
        {
            ImGui::TextDisabled("(Selected Object expired)");
            ImGui::End();
            return;
        }

        RenderEntity(obj);
    }
    else
    {
        ImGui::TextDisabled("(Not Implemented Yet)");
    }

    ImGui::End();
}

void CInspectorWin::RenderEntity(const std::shared_ptr<Engine::CEntity>& obj)
{
    ImGui::TextDisabled("Entity");

    // 선택 대상 바뀌면 버퍼/상태 리셋
    const uint64_t curId = (uint64_t)obj.get();
    if (m_LastSelectedId != curId)
    {
        m_LastSelectedId = curId;
        m_EditingName = false;

        _string curName = ConvertW2A(obj->Get_Name());
        strncpy_s(m_NameBuf, sizeof(m_NameBuf), curName.c_str(), _TRUNCATE);
        m_NameBuf[sizeof(m_NameBuf) - 1] = 0;
    }

    // ---- Name ----
    _string shownName = ConvertW2A(obj->Get_Name());
    if (shownName.empty()) shownName = "Entity";

    ImGui::Text("Name"); ImGui::SameLine();

    if (!m_EditingName)
    {
        ImGui::Selectable(shownName.c_str(), false);

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            m_EditingName = true;
            // 다음 프레임의 InputText에 포커스 주기 위해 예약
            ImGui::SetKeyboardFocusHere();
        }
    }
    else
    {
        ImGui::SetNextItemWidth(-1);

        bool enter = ImGui::InputText("##obj_name", m_NameBuf, sizeof(m_NameBuf),
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

        bool deactivated = ImGui::IsItemDeactivatedAfterEdit();

        // ESC: 취소(원래 이름으로 복구)
        if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            _string reset = ConvertW2A(obj->Get_Name());
            strncpy_s(m_NameBuf, sizeof(m_NameBuf), reset.c_str(), _TRUNCATE);
            m_NameBuf[sizeof(m_NameBuf) - 1] = 0;
            m_EditingName = false;
        }
        else if (enter || deactivated)
        {
            // UTF-8 -> UTF-16 (한글 포함)
            std::wstring w;
            if (m_NameBuf[0] != '\0')
            {
                int wlen = MultiByteToWideChar(CP_UTF8, 0, m_NameBuf, -1, nullptr, 0);
                if (wlen > 1)
                {
                    w.resize(wlen - 1);
                    MultiByteToWideChar(CP_UTF8, 0, m_NameBuf, -1, &w[0], wlen);
                }
            }

            if (!w.empty())
                obj->Set_Name(w);

            m_EditingName = false;
        }
    }

    // ---- Object extra gui ----
    ImGui::Separator();
    ImGui::PushID((void*)obj.get());
    obj->OnGui();
    ImGui::PopID();

    // ---- Components ----
    ImGui::Separator();
    ImGui::TextDisabled("Components");

    const auto& comps = obj->Get_ComponentMap();
    if (comps.empty())
    {
        ImGui::TextDisabled("(No Components)");
        return;
    }

    for (auto& kv : comps)
    {
        auto comp = kv.second;  // shared_ptr 복사(안전)
        if (!comp) continue;

        _string compName = ConvertW2A(comp->Get_Name());
        if (compName.empty()) compName = "Component";

        char labelBuf[256];
        sprintf_s(labelBuf, "%s##%p", compName.c_str(), comp.get());

        if (ImGui::CollapsingHeader(labelBuf, ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushID((void*)comp.get());
            comp->OnGui();
            ImGui::PopID();
        }
    }
}


std::shared_ptr<CInspectorWin> CInspectorWin::Create()
{
    auto p = std::shared_ptr<CInspectorWin>(new CInspectorWin());
    if (FAILED(p->Initialize()))
        return nullptr;
    return p;
}

void CInspectorWin::Free()
{
    // 지금은 리소스 없음. 필요하면 여기서 정리.
}