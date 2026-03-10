#pragma once
#include "Base.h"
#include "Log_Manager.h"
NS_BEGIN(Engine)

class ENGINE_DLL CLog_Manager
{
    DECLARE_SINGLETON(CLog_Manager)
private:
    CLog_Manager();
public:
    virtual ~CLog_Manager();
public:
   
    struct LogData {
        LOG_LEVEL eLevel;
        std::string strMessage;
    };
public:
    void Add_Log_F(LOG_LEVEL eLevel, const char* pFormat, ...);
    void Add_Log(LOG_LEVEL eLevel, const char* pMessage);
    const vector<LogData>& Get_Logs() { return m_vecLogs; }
    void Clear() { m_vecLogs.clear(); }
private:

private:
    vector<LogData> m_vecLogs;
    mutex m_mutex;
public:
    void Free();
};

NS_END