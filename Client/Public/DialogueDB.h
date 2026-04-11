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

   void Set_PendingDialogue(list<string> dialogueIds, _int startid = 0) { m_NextIds = dialogueIds; };
    list<string>& Get_PendingDialogue() { return m_NextIds; }

private:
     weak_ptr<CGameInstance> m_pGameInstance = {};
    vector< shared_ptr<Dialogue>> m_Dialogues;
    unordered_map<string, _int> m_DialogueIndex;


    list<string> m_NextIds;

private:
    void Free() override;

};

NS_END