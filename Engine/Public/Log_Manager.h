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
        ELogLevel level;
        std::string message;
    };
public:
    void Add_Log_F(ELogLevel level, const char* format, ...);
    void Add_Log(ELogLevel level, const char* message);
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