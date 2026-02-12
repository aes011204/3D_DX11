#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND hWnd;
		WINMODE eWinMode;
		unsigned int iMaxLevelNum;
		unsigned int iViewportWidth;
		unsigned int iViewportHeight;
	}ENGINE_DESC;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexcoord;
	}VTXTEX;



    struct Rect
    {
        float x = 0.f;
        float y = 0.f;
        float w = 0.f;
        float h = 0.f;

        float Left() const { return x; };
        float Top()  const { return y; };
        float Right()  const { return x + w; };
        float Bottom()  const { return y + h; };

        Vector2 Pos() const { return { x,y }; };
        Vector2 Size() const { return { w,h }; };

        // 히트 박스 테스트용
        bool Contains(float _x, float _y)
        {
            return (_x >= Left() && _x <= Right() && _y >= Top() && _y <= Bottom());
        }
    };

    //struct UICanvas
    //{
    //    Rect rect = { 0,0,1280,720 };
    //};
}


#endif // Engine_Struct_h__
