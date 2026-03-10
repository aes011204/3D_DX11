#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN };

	enum class PROTOTYPE { GAMEOBJECT, COMPONENT};

	enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, UI, END };

	enum class UI_LAYER { OVERRIDE, WINDOW, STACK, HUD, END };

	enum class STATE { RIGHT, UP, LOOK, POSITION, END };

	enum class D3DTS {VIEW,PROJ, END };

	enum class SAVETYPE { GAMEOBJECT, UI, END }; 

	enum class DIMB { LBUTTON, RBUTTON, WHEEL, END };

	enum class DIMM { X, Y, WHEEL, END };

	enum class LIGHT { DIRECTIONAL, POINT, END };

	enum class LOG_LEVEL { INFO, WARNING, ERR };

	//// Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
	//enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	//enum INFO {	INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };

	//enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

	//enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

	//enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };

	//enum MOUSEMOVESTATE {	DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };



	enum class BUTTON_STATE
	{
		NORMAL,// 아무것도 안할떄
		SELECT,
		HOVER,// 호버
		CLICK,// 클릭
		DISABLE,// 사라질때
		INABLE,// 나타날때

	};
	enum class UI_STATE
	{
		ACTIVE,
		INACTIVE,
		DISABLE,
		END

	};

}
#endif // Engine_Enum_h__
