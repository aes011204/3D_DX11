#pragma once

#include "Base.h"
#include "Client_Enum.h"

namespace Engine
{
	class CGameInstance;
}

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

   void Set_PendingDialogue(list<string> dialogueIds, _int startid = 0) { m_nextIds = dialogueIds; };
    list<string>& Get_PendingDialogue() { return m_nextIds; }

private:
     weak_ptr<CGameInstance> m_gameInstance = {};
    vector< shared_ptr<Dialogue>> m_dialogues;
    unordered_map<string, _int> m_dialogueIndex;


    list<string> m_nextIds;

private:
    void Free() override;

};

NS_END