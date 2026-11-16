#include "Log_Manager.h"

IMPLEMENT_SINGLETON(CLog_Manager)

CLog_Manager::CLog_Manager()
{
}

CLog_Manager::~CLog_Manager()
{

}
void CLog_Manager::Add_Log_F(ELogLevel level, const char* format, ...) {
    if (nullptr == format) return;

    char szBuf[1024] = { 0 };
    va_list args;
    va_start(args, format);
    vsprintf_s(szBuf, 1024, format, args);
    va_end(args);

    std::lock_guard<std::mutex> lock(m_mutex);
    m_vecLogs.push_back({ level, std::string(szBuf) });

    if (m_vecLogs.size() > 500) m_vecLogs.erase(m_vecLogs.begin());
}

// 2. 단순 문자열 버전 (오버로딩)
void CLog_Manager::Add_Log(ELogLevel level, const char* message) {
    if (nullptr == message) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_vecLogs.push_back({ level, std::string(message) });

    if (m_vecLogs.size() > 500) m_vecLogs.erase(m_vecLogs.begin());
}

void CLog_Manager::Free()
{
    lock_guard<mutex> lock(m_mutex);
    m_vecLogs.clear();
}
