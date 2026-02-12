#include "Selection.h"
#include "Entity.h"
#include "EventBus.h"
#include "EditorInstance.h"
#include "Event_Struct.h"



CSelection::CSelection()
    : m_Type(ESelectType::NONE)
{
}

CSelection::~CSelection()
{
    Free();
}
HRESULT CSelection::Initialize()
{
    if (CEditorInstance::GetInstance()->GetEventBus() == nullptr)
        return E_FAIL;
    // 여기서 본인이 직접 구독합니다.
    CEditorInstance::GetInstance()->GetEventBus()->Subscribe<Editor::EvtSelectEntity>([this](const Editor::EvtSelectEntity& e)
        {
            this->SetEntity(e.Entity);
        });

    CEditorInstance::GetInstance()->GetEventBus()->Subscribe<Editor::EvtClearSelection>([this](const Editor::EvtClearSelection& e)
        {
            this->Clear();
        });
}
shared_ptr<CSelection> CSelection::Create()
{
    shared_ptr<CSelection> pInstance(new CSelection());
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("failed to create :CSelection");
        return nullptr;
    }
    // 나중에 이벤트 버스 Subscribe 같은 거 붙이기 좋음

    return pInstance;
}

void CSelection::Free()
{
    Clear();
}

void CSelection::Clear()
{
    m_Type = ESelectType::NONE;
    m_Entity.reset();
}

void CSelection::SetEntity(const std::shared_ptr<Engine::CEntity>& entity)
{
    if (!entity)
    {
        Clear();
        return;
    }

    m_Type = ESelectType::ENTITY;
    m_Entity = entity;
}

std::shared_ptr<Engine::CEntity> CSelection::GetEntity() const
{
    if (m_Type != ESelectType::ENTITY)
        return nullptr;

    return m_Entity.lock();
}

