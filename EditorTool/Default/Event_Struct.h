#pragma once
#include "Editor_Define.h"

namespace Engine { class CEntity; }

namespace Editor
{
    struct EvtSelectEntity
    {
        shared_ptr<Engine::CEntity> Entity;
    };

    struct EvtClearSelection {};
}