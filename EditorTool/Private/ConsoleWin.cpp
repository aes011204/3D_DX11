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

    auto logMgr = CLog_Manager::GetInstance();
    if (nullptr == logMgr) return;

    if (ImGui::Begin("Console Log"))
    {
        if (ImGui::Button("Clear")) logMgr->Clear();
        ImGui::Separator();

        ImGui::BeginChild("LogScrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        auto& vecLogs = logMgr->Get_Logs();
        for (const auto& log : vecLogs)
        {
            ImVec4 color = { 1.f, 1.f, 1.f, 1.f }; // INFO

            if (log.level ==ELogLevel::WARNING)
                color = { 1.f, 1.f, 0.f, 1.f }; // WARNING (노란색)
            else if (log.level == ELogLevel::ERR)
                color = { 1.f, 0.f, 0.f, 1.f }; // ERR (빨간색)

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(log.message.c_str());
            ImGui::PopStyleColor();
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
