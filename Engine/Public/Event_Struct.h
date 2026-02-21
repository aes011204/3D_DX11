#pragma once
#include "Editor_Define.h"

namespace Engine { class CEntity; }

namespace Engine
{
    struct EvtSelectEntity
    {
        shared_ptr<Engine::CEntity> Entity;
    };

    struct EvtClearSelection {};


    struct EvtCloneProto
    {
        //shared_ptr<Engine::CBase> Cloned;
        _wstring Tag = {};
        _uint iLevel = {};
    };
}