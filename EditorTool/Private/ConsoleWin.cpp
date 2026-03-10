#include"ConsoleWin.h"
#include"Log_Manager.h"

CConsoleWin::CConsoleWin()
    :CEditor_Win("Console")
{
}

CConsoleWin::~CConsoleWin()
{
    Free();
}


void CConsoleWin::Render()
{

    auto pLogMgr = CLog_Manager::GetInstance();
    if (nullptr == pLogMgr) return;

    if (ImGui::Begin("Console Log"))
    {
        if (ImGui::Button("Clear")) pLogMgr->Clear();
        ImGui::Separator();

        ImGui::BeginChild("LogScrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        auto& vecLogs = pLogMgr->Get_Logs();
        for (const auto& log : vecLogs)
        {
            ImVec4 vColor = { 1.f, 1.f, 1.f, 1.f }; // INFO

            if (log.eLevel ==LOG_LEVEL::WARNING)
                vColor = { 1.f, 1.f, 0.f, 1.f }; // WARNING (노란색)
            else if (log.eLevel == LOG_LEVEL::ERR)
                vColor = { 1.f, 0.f, 0.f, 1.f }; // ERR (빨간색)

            ImGui::TextUnformatted(log.strMessage.c_str());
        }

        // 최하단 자동 스크롤
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
    }
    ImGui::End();
}
void CConsoleWin::Free()
{
}

shared_ptr<CConsoleWin> CConsoleWin::Create()
{
	return make_shared<CConsoleWin>();
}
