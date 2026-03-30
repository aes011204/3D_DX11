#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND hWnd;
        HINSTANCE hInst;
        WINMODE eWinMode;
        unsigned int iMaxLevelNum;
        unsigned int iEditorLevel;
		unsigned int iViewportWidth;
		unsigned int iViewportHeight;
	}ENGINE_DESC;


    typedef struct tagVertexPosition
    {
        XMFLOAT3			vPosition;

        static const _uint iNumElements = { 1 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
        {
            {"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            
        };
    }VTXPOS;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexcoord;

        static const _uint iNumElements = { 2 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
        {
            {"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"TEXCOORD", 0 , DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  }
        };
	}VTXTEX;

    typedef struct tagVertexCube
    {
        XMFLOAT3			vPosition;
        XMFLOAT3			vTexcoord;

        static const _uint iNumElements = { 2 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
        {
            {"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"TEXCOORD", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  }
        };
    }VTXCUBE;


    typedef struct tagVertexPositionNormalTexcoord
    {
        XMFLOAT3			vPosition;
        XMFLOAT3			vNormal;
        XMFLOAT2			vTexcoord;

        static const _uint iNumElements = { 3 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
        {
            {"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"NORMAL", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"TEXCOORD", 0 , DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  }
        };
    }VTXNORTEX;

    typedef struct tagVertexMesh
    {
        XMFLOAT3			vPosition;
        XMFLOAT3			vNormal;
        XMFLOAT3			vTangent;
        XMFLOAT2			vTexcoord;

        static const _uint iNumElements = { 4 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
        {
            {"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"NORMAL", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"TANGENT", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"TEXCOORD", 0 , DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0  }
        };
    }VTXMESH;

    typedef struct tagVertexAnimMesh
    {
        XMFLOAT3			vPosition;
        XMFLOAT3			vNormal;
        XMFLOAT3			vTangent;
        XMFLOAT2			vTexcoord;

        XMUINT4             vBlendIndex;
        XMFLOAT4            vBlendWeight;

        static const _uint iNumElements = { 6 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
        {
            {"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"NORMAL", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"TANGENT", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"TEXCOORD", 0 , DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0  },
			{ "BLENDINDEX", 0 ,DXGI_FORMAT_R32G32B32A32_UINT , 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            {"BLENDWEIGHT", 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0  }
        };
    }VTXANIMMESH;

    
    struct Rect
    {
        float x = 0.f;
        float y = 0.f;
        float w = 0.f;
        float h = 0.f;

        float Left() const { return x; };
        float Top()  const { return y; };
        float Right()  const { return  w; };
        float Bottom()  const { return h; };

        Vector2 Pos() const { return { x,y }; };
        Vector2 Size() const { return { w - x, h - y }; };

        // 히트 박스 테스트용
        bool Contains(float _x, float _y)
        {
            return (_x >= Left() && _x <= Right() && _y >= Top() && _y <= Bottom());
        }
    };

    struct LIGHT_DESC
    {
        LIGHT eType;

        XMFLOAT4 vDirection;
        XMFLOAT4 vPosition;
        float fRange;

        XMFLOAT4 vDiffuse;
        XMFLOAT4 vAmbient;
        XMFLOAT4 vSpecular;
     
    };

    struct KEYFRAME
    {
        XMFLOAT3 vScale;
        XMFLOAT4 vRotation;
        XMFLOAT3 vTranslation;

        float fTrackPosition;
    };



    struct NINESLICE_DESC
    {
        _float2 TexOriginalSize = {};
        _float2 TexCustomSize = {};
        _float2 UISize = {};
        _float4 PxSliceLRTB = {};
        _float TileScale = {1.f};

    };

    struct LAYOUT_DESC
    {
        int m_Row = { 5 };
        int m_Col = { 5 };
        float m_SlotSize = { 20.f };
        _float2 m_Spacing = {0.f, 0.f };
        _float2 m_Padding = { 0.f, 0.f };

        _float2 m_Offset = { 0.f, 0.f };
    };
}


#endif // Engine_Struct_h__
