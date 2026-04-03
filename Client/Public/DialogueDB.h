#pragma once

#include "Base.h"
#include "Client_Enum.h"

NS_BEGIN(Client)

class CDialogueDB : public CBase
{
    DECLARE_SINGLETON(CDialogueDB)
private:
    explicit CDialogueDB();
    virtual ~CDialogueDB() override;

public:
    HRESULT     Ready_DialogueDB();
    HRESULT     Load_DialogueData(const string& fileName);

    shared_ptr<Dialogue>   GetDialogueById(const string& dialogueId);
    wstring     Utf8ToWstring(const string& str);

private:
    vector< shared_ptr<Dialogue>>            m_Dialogues;
    unordered_map<string, _int> m_DialogueIndex;

private:
    void Free() override;

};

NS_END