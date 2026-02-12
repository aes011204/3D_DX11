#include "Log_Manager.h"

IMPLEMENT_SINGLETON(CLog_Manager)

CLog_Manager::CLog_Manager()
{
}

CLog_Manager::~CLog_Manager()
{

}
void CLog_Manager::Add_Log_F(LOG_LEVEL eLevel, const char* pFormat, ...) {
    if (nullptr == pFormat) return;

    char szBuf[1024] = { 0 };
    va_list args;
    va_start(args, pFormat);
    vsprintf_s(szBuf, 1024, pFormat, args);
    va_end(args);

    std::lock_guard<std::mutex> lock(m_mutex);
    m_vecLogs.push_back({ eLevel, std::string(szBuf) });

    if (m_vecLogs.size() > 500) m_vecLogs.erase(m_vecLogs.begin());
}

// 2. 단순 문자열 버전 (오버로딩)
void CLog_Manager::Add_Log(LOG_LEVEL eLevel, const char* pMessage) {
    if (nullptr == pMessage) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_vecLogs.push_back({ eLevel, std::string(pMessage) });

    if (m_vecLogs.size() > 500) m_vecLogs.erase(m_vecLogs.begin());
}

void CLog_Manager::Free()
{
    lock_guard<mutex> lock(m_mutex);
    m_vecLogs.clear();
}
