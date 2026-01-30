#ifndef Engine_Define_h__
#define Engine_Define_h__

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX; 
// 네임스페이스 선언을 전에 하지 않으면 에러남 / 이건 위에 #include <DirectXMath.h> 있어서 ㄱㅊ 


//imgui
#include "../../EngineSDK/inc/imgui.h"
#include "../../EngineSDK/inc/imgui_impl_dx11.h"
#include "../../EngineSDK/inc/imgui_impl_win32.h"
#include "../../EngineSDK/inc/ImguiManager.h"

//Magic_Enum
#include "../../Lib/magic_enum/magic_enum.hpp"

//json
#include "../../Lib/nlohmann/json.hpp"

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <ctime>

// 내가 추가한 헤더
#include <stack>
#include <assert.h>
#include <fstream>
#include <iostream>
#include  <variant>
#include  <queue>
//

using namespace std;

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#pragma warning(disable : 4251)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif


using namespace Engine;

#endif // Engine_Define_h__
