#ifndef Engine_Define_h__
#define Engine_Define_h__

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/Effects.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"
#include "DirectXTK/ScreenGrab.h"

using namespace DirectX; 
// 네임스페이스 선언을 전에 하지 않으면 에러남 / 이건 위에 #include <DirectXMath.h> 있어서 ㄱㅊ 

//Magic_Enum
#include "magic_enum/magic_enum.hpp"

//json
#include "nlohmann/json.hpp"

//fx11
#include "Fx11/d3dx11effect.h"

//assimp
#include "Assimp/scene.h"
#include "Assimp/Importer.hpp"
#include "Assimp/postprocess.h"

//// ImGui
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <ctime>
#include <set>
//#include <numbers>



// DInput

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#pragma warning(disable : 4251)
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// 스마트 포인터
#include <memory>
#include <wrl.h>
using namespace Microsoft::WRL;

#include <stack>
#include <assert.h>
#include <iostream>
#include  <variant>
#include  <queue>
#include <mutex>
#include <cstdarg> 
//
#include <fstream>
#include <iomanip>
#include <format>

#include "DirectXTK/SimpleMath.h"
using namespace DirectX::SimpleMath;

using namespace std;

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

//
#include "Log_Manager.h"
#include "Engine_Helper.h"

//
#include "Converter_Struct.h"

namespace Engine
{
	static const wstring g_strTransformTag = TEXT("Com_Transform");
	static const wstring g_strUITransformTag = TEXT("Com_UITransform");

}

using namespace Engine;


#pragma warning(disable: 4275) // DLL 인터페이스 경고 무시
#pragma warning(disable: 4251) // STL 컨테이너 DLL 수출 경고 무시


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



#endif // Engine_Define_h__
