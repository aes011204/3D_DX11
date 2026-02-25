#pragma once

#pragma push_macro("new")
#ifdef new
#undef new
#endif
//// ImGui
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#pragma pop_macro("new")


#include "Engine_Define.h"
//


namespace Editor
{
    enum class ESelectType
    {
        ENTITY,
        ETC,
        NONE
    };

    struct DragProtoTypeDesc
    {
        _wstring strProtoTag = {};
        _uint strProtoLevel = {};
    };

}

using namespace Editor;

