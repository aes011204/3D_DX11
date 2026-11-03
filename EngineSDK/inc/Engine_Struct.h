#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND hWnd;
        HINSTANCE hInst;
        EWinMode winMode;
        unsigned int maxLevelNum;
        unsigned int editorLevel;
		unsigned int viewportWidth;
		unsigned int viewportHeight;
	}EngineDesc;


    typedef struct tagVertexPosition
    {
        XMFLOAT3			position;

        static const _uint iNumElements = { 1 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
        {
            {"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            
        };
    }VTXPOS;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3			position;
		XMFLOAT2			texcoord;

        static const _uint iNumElements = { 2 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
        {
            {"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"TEXCOORD", 0 , DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  }
        };
	}VTXTEX;

    typedef struct tagVertexCube
    {
        XMFLOAT3			position;
        XMFLOAT3			texcoord;

        static const _uint iNumElements = { 2 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
        {
            {"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"TEXCOORD", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  }
        };
    }VTXCUBE;


    typedef struct tagVertexPositionNormalTexcoord
    {
        XMFLOAT3			position;
        XMFLOAT3			normal;
        XMFLOAT2			texcoord;

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
        XMFLOAT3			position;
        XMFLOAT3			normal;
        XMFLOAT3			tangent;
        XMFLOAT2			texcoord;

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
        XMFLOAT3			position;
        XMFLOAT3			normal;
        XMFLOAT3			tangent;
        XMFLOAT2			texcoord;

        XMUINT4             blendIndex;
        XMFLOAT4            blendWeight;

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


    typedef struct tagVertexParticleInstance
    {
        XMFLOAT4			right;
        XMFLOAT4			up;
        XMFLOAT4			look;
        XMFLOAT4			translation;
        XMFLOAT2			lifeTime;
    }VTXPARTICLE_INSTANCE;

    typedef struct tagVertexParticleRectInstanceDesc
    {

        static const _uint iNumElements = { 7 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
        {
            {"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
            {"TEXCOORD", 0 , DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },

        	{"TEXCOORD", 1 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1  },
            {"TEXCOORD", 2 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1  },
            {"TEXCOORD", 3 ,DXGI_FORMAT_R32G32B32A32_FLOAT , 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            {"TEXCOORD", 4 ,DXGI_FORMAT_R32G32B32A32_FLOAT , 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            {"TEXCOORD", 5 , DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1  }
        };
    }VTXPARTICLE_RECTINSTANCE_DESC;

    typedef struct tagVertexParticlePointInstanceDesc
    {
        static const unsigned int		iNumElements = { 6 };

        static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
   
            { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "WORLD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

        	{ "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        };
    }VTXPARTICLE_POINTINSTANCE_DESC;





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

    struct LightDesc
    {
        ELightType type;

        XMFLOAT4 direction;
        XMFLOAT4 position;
        float range;

        XMFLOAT4 diffuse;
        XMFLOAT4 ambient;
        XMFLOAT4 specular;
     
    };

    struct KeyFrame
    {
        XMFLOAT3 scale;
        XMFLOAT4 rotation;
        XMFLOAT3 translation;

        float trackPosition;
    };



    struct NineSliceDesc
    {
        _float2 texOriginalSize = {};
        _float2 texCustomSize = {};
        _float2 UISize = {};
        _float4 pxSliceLRTB = {};
        _float tileScale = {1.f};

    };

    struct LayoutDesc
    {
        int row = { 5 };
        int col = { 5 };
        float slotSize = { 20.f };
        _float2 spacing = {0.f, 0.f };
        _float2 padding = { 0.f, 0.f };

        _float2 offset = { 0.f, 0.f };
    };



    enum class ESoundCategory : _ubyte
    {
        BGM,
        SFX,
        UI,
        VOICE,
        AMBIENT,
        END
    };

    // 사운드 로드 방식
    enum class ESoundLoadMode : _ubyte
    {
        STATIC,
        STREAM,
        END
    };

    using SoundAssetDesc = struct tSoundAssetDesc
    {
        _wstring soundTag{};
        _wstring assetPath{};
        ESoundCategory category{ ESoundCategory::SFX };
        ESoundLoadMode loadMode{ ESoundLoadMode::STATIC };
        _float defaultVolume{ 1.f };
    };


}


#endif // Engine_Struct_h__
