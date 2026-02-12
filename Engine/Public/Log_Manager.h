#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLog_Manager :
    public CBase
{
    DECLARE_SINGLETON(CLog_Manager)
private:
    CLog_Manager();
public:
    virtual ~CLog_Manager();
public:
    enum class LOG_LEVEL { INFO, WARNING, ERR };
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
    void Free() override;
};

NS_END