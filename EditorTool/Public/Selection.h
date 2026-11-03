#pragma once
#include "Editor_Define.h"

namespace Engine
{
    class CEntity;
    class ETC;
    class CEventBus;
}

NS_BEGIN(Editor)

class CSelection
{
private:
    CSelection();

public:
    ~CSelection();

    HRESULT Initialize();

public:
    static shared_ptr<CSelection> Create();
    void Free();

public:
    void Clear();

    void SetEntity(const shared_ptr<CEntity>& entity);
    shared_ptr<CEntity> GetEntity() const;

    bool HasSelection() const { return m_type != ESelectType::NONE; }
    ESelectType GetType() const { return m_type; }

private:
    ESelectType m_type = ESelectType::NONE;
    weak_ptr<CEntity> m_entity;
};

NS_END
