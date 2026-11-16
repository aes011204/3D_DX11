#pragma once
#include "Editor_Define.h"



namespace Client
{
	class CInventory_Controller;
}

namespace Engine { class CEntity; }

namespace Engine
{
    struct EvtSelectEntity
    {
        shared_ptr<Engine::CEntity> entity;
    };

    struct EvtClearSelection {};


    struct EvtCloneProto
    {
        //shared_ptr<Engine::CBase> Cloned;
        _wstring tag = {};
        _uint level = {};
    };
    struct DragDropProto
    {
        //shared_ptr<Engine::CBase> Cloned;
        wchar_t tag[256] = {};
        _uint level = {};
    };

   
    struct EvtTerrainPicking
    {
        _bool isOnPicking = false;
        _wstring tag = {};
        _uint level = {};
    };
    struct EvtControllerPoiner
    {
        Client::CInventory_Controller* controlPointer = {};
    };
    

}