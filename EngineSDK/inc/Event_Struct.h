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
    struct DragDropProto
    {
        //shared_ptr<Engine::CBase> Cloned;
        wchar_t szTag[256] = {};
        _uint iLevel = {};
    };

   
    struct EvtTerrainPicking
    {
        _bool isOnPicking = false;
        _wstring Tag = {};
        _uint iLevel = {};
    };
}