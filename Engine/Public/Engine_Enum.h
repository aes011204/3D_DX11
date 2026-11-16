#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class EWinMode { FULL, WIN };

	enum class EPrototype { GAMEOBJECT, COMPONENT};

	enum class ERenderGroup { PRIORITY, NONBLEND, BLEND, NONLIGHT, SEA, UI, END };

	enum class EUILayer { OVERRIDE, WINDOW, STACK, HUD, END };

	enum class ETransformState { RIGHT, UP, LOOK, POSITION, END };

	enum class ED3DTS {VIEW,PROJ, END };

	enum class ESaveType { GAMEOBJECT, UI, END }; 

	enum class EMouseButton { LBUTTON, RBUTTON, WHEEL, END };

	enum class EMouseMove { X, Y, WHEEL, END };

	enum class ELightType { DIRECTIONAL, POINT, END };

	enum class ELogLevel { INFO, WARNING, ERR };

	enum class EModelType { NONANIM, ANIM, END };

	enum class EColliderType { AABB, OBB, SPHERE, END };

	enum class EDeferred { DEBUG, DIRECTIONAL, POINT, COMBINED,  BLUR_H, BLUR_V,END };


	enum ECollisionLayer
	{
		NONE = 0,
		PLAYER = 1 << 0, 
		MONSTER = 1 << 1,
		MONSTERATT = 1 << 2 ,
		TERRAIN = 1 << 3,
		TRIGGER = 1 << 4,
		ITEM = 1 << 5,
		UI = 1 << 6,
		FISH = 1 << 7,
		ALL= 0xffffffff,
		END 
	};
	using CollisionMask = unsigned __int32;

	enum class EButtonState
	{
		NORMAL,// 아무것도 안할떄
		SELECT,
		HOVER,// 호버
		CLICK,// 클릭
		DISABLE,// 사라질때
		INABLE,// 나타날때

	};
	enum class EUIState
	{
		ACTIVE,
		INACTIVE,
		DISABLE,
		END

	};

}
#endif // Engine_Enum_h__
