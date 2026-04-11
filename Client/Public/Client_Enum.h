#pragma once
#include  "Engine_Define.h"
#include  "Client_Define.h"


namespace Engine
{
	class CGameObject;
	class CTexture;
}

namespace Client
{
	enum class LOCATIONSTATE;

	// 아이템 관련 
	enum class ITEM_TYPE { FISH, EQUIP, MATERIAL, PURSUIT, TRINKET, END };
	enum class TIME { DAY, NIGHT, END };
    enum class FRESHNESS { FRESH, STALE, ROTTING, INFECTED, END };
    enum class SLOT_TYPE { ANY, ENGINE, LIGHT, ROT, NET, END };
    enum class EQUIP_TYPE { ENGINE, LIGHT, ROD, NET , POT ,CRAP ,END };

	enum class SEA_TYPE : _ubyte { COASTAL, SHALLOW, OCEANIC, ABYSSAL, HADAL, MANGROVE, VOLCANIC, ICE, CRAB, END };
	// 비트 플레그로쓸거임 //uint8_t : “하나 선택” → 8비트로 충분
    using SEA_MASK = _uint; // uint16_t (_ushort) : 16개 까지 가능 // 여러 개 ON/OFF 집합” → 16비트로 넉넉하게 // _uint 32 비트로 해도됨 
    constexpr SEA_MASK Set_SeaType_Mask(SEA_TYPE SeaType) { return SEA_MASK{ 1u } << static_cast<uint32_t>(SeaType);
    } // 1u = unsigned 1 // << 비트 연산 왼쪽으로 한칸씩 이동

}



namespace Client
{
    using ID_uint = _ushort;
    
    static constexpr ID_uint ID_Absence = 0;

	// 아이템 관련

    struct OccCell { _uint dx, dy; };

    struct Shape
    {
        _uint Width , Height = { 0 };
        vector<_ubyte> Shape_Mask = {}; // 0,1 로만 할건데 bool 문제생길수있음 // ex) [ 111, 101 ] 
        vector<OccCell> Occ[4] = {}; // uint16_t //Shape_Mask 의 좌표 캐싱 // 회전도 같이 캐싱 // ex) {(0,0),(1,0),(2,0),(0,1),(0,2)}
        
    };

     struct Mutation
     {
         ID_uint Species_ID = { ID_Absence }; ID_uint Mutation_ID = { ID_Absence };
         _string MutName = {}; _string MutDesc = {}; _string MutTexturePath = {};
         _float MutCost = { 0 };
         shared_ptr<Engine::CTexture> pTexture = { nullptr };
     };

    struct Fish_Def
    {
        float Cost = { 0 }; TIME FishTime = TIME::END; SEA_MASK SeaType_Mask = { 0 };
        _bool IsRod = false; _bool IsTrawl = false; _bool  IsPot = false;

        vector<Mutation> vec_Mutation = {}; // array 는 현제 몇개가 있는지 카운트하는기능이 없어서 struct로 만들어서 / 근데 난 귀찬
    };

		struct Equip_Engine { _uint Research_Lv = { 0 }; _uint Speed_Kn = { 0 }; };
        struct Equip_Light { _uint Lumen = { 0 }; _uint Range = { 0 }; };
        struct Equip_Rod {_uint Research_Lv = { 0 }; SEA_MASK SeaType_Mask = { 0 }; _uint Speed_Per = { 0 }; };
        struct Equip_Net {_uint Research_Lv = { 0 }; SEA_MASK SeaType_Mask = { 0 }; _uint Cap_SizeX, Cap_SizeY = { 0 };
        	_uint Yield = { 0 }; _uint LastFor = { 0 }; };
        struct Equip_CrapPot { _uint Research_Lv = { 0 }; _uint Cap_SizeX, Cap_SizeY = { 0 };
        	_uint Yield = { 0 }; _uint LastFor = { 0 };
        };

    struct Equip_Def
    {
        float Cost = { 0 }; _uint InstallTime = { 0 }; EQUIP_TYPE EquipType = EQUIP_TYPE::END;
        variant<Equip_Engine, Equip_Light, Equip_Rod, Equip_Net, Equip_CrapPot> EquipType_Effect;
    };


    struct Material_Def { float Cost = { 0 }; };
    struct Trinket_Def { float Cost = { 0 }; };


    struct Fish_Inst { FRESHNESS freshness = FRESHNESS::FRESH; ID_uint mutation_ID = { ID_Absence }; _float size = {}; };
    struct Equip_Inst { bool IsBroken = false; };

    /// 아이템 인스턴스 
    struct Item_Inst
    {
        ID_uint ItemDef_ID = { ID_Absence }; 
        ID_uint ItemInst_ID = { ID_Absence };
        ITEM_TYPE ItemType = ITEM_TYPE::END; // 이건 그냥 캐싱용으로 두자

        bool IsMutaion = { false };
        Mutation MutaionCashing = {};


        shared_ptr<Engine::CTexture> pCashingTexture = nullptr;


        _float2 BaseXY = {};

        vector<OccCell> CurBase = {};
        _uint Rotation = { 0 };

        variant<monostate, Fish_Inst, Equip_Inst>  TypeDef;

    };


    /// 아이템 정의
    struct Item_Def
	{
		ID_uint ItemID = { ID_Absence }; // 이제 id 는 0 을 아무것도 없는 가짜 상태로
		ITEM_TYPE ItemType = ITEM_TYPE::END;
		_string ItemName = {};
		_string ItemDesc = {};
        _string TexturePath = {};
        
    	shared_ptr<Engine:: CTexture> pTexture = nullptr;

        Shape ItemShape = {};
        _uint SizeNum = { 0 };

        variant<monostate, Fish_Def, Equip_Def, Material_Def, Trinket_Def>  TypeDef;

	};
    
    static wstring freshToWstr(FRESHNESS freshness)
    {

        switch (freshness) {
        case FRESHNESS::FRESH: return L"신선한";
        case FRESHNESS::ROTTING: return L"퀴퀴함";
        case FRESHNESS::INFECTED: return  L"감염된";
        default: return L"UNKNOWN";
        }
    }
    static const wstring GetSeaTypeName(SEA_TYPE Type)
    {
        switch (Type)
        {
        case SEA_TYPE::COASTAL:
            return L"COASTAL";
        case SEA_TYPE::SHALLOW:
            return L"SHALLOW";
        case SEA_TYPE::OCEANIC:
            return L"OCEANIC";
        case SEA_TYPE::ABYSSAL:
            return L"ABYSSAL";
        case SEA_TYPE::HADAL:
            return L"HADAL";
        case SEA_TYPE::MANGROVE:
            return L"MANGROVE";
        case SEA_TYPE::VOLCANIC:
            return L"VOLCANIC";
        case SEA_TYPE::ICE:
            return L"ICE";
        case SEA_TYPE::CRAB:
            return L"CRAB";
        case SEA_TYPE::END:
            return L"END";
        default:
            return L"UNKNOWN";
        }
    }

    static wstring OcceanToWstr(SEA_MASK sea)
    {
        wstring result = L"";

        for (_uint i = 0; i < ETOI(SEA_TYPE::END); ++i)
        {
            SEA_MASK typeMask = 1u << i;

            if (sea & typeMask)
            {
                if (!result.empty())
                    result += L" | ";

                result += GetSeaTypeName(static_cast<SEA_TYPE>(i));
            }
        }
        return result;
    }

    static wstring GetBroken(_bool IsBroken)
    {

        switch (IsBroken)
    	{
        case true: return L"사용 불가";
        case false: return L"사용가능";
       
        }
    }
}


namespace Client
{
    enum class PLACE_COLOR { RED, ORANGE, GREEN ,END};
    /// 슬롯
    struct Slot
    {
        ID_uint ItemInst_ID = { ID_Absence };
        SLOT_TYPE slotType = SLOT_TYPE::ANY;
        _bool IsLock = { false };
        _bool IsBroken = { false };
        PLACE_COLOR Slot_Color = PLACE_COLOR::END;
    };

}


namespace Client
{
    // 다이어로그

 // 선택지 구조체
    struct DialogueChoice
    {
        wstring leftText;           // 왼쪽 선택지 ("살해")
        wstring rightText;          // 오른쪽 선택지 ("노예 ㄱㄱ")
        _int leftNextId;
        _int rightNextId;
    };

    // 대사 한 줄
    struct DialogueLine
    {
        _int   lineIndex;           // 라인 순서 (0, 1, 2...)
        wstring speaker;             // 말하는 놈 ("보스", "플레이어", "")
        wstring text;                // 대사 내용
        _bool  hasChoices;          // 선택지가 있는 라인인지
        _int  NextID;          // 선택지가 있는 라인인지

        DialogueChoice choice;           // 선택지가 있을 때

        _float leftPadding = 0.f;

        string voicePath;
    };

    // 대화 묶음 (하나의 대화 시퀀스)
    struct Dialogue
    {
        string dialogueId;               // 대화 ID ("boss1_intro", "boss1_battle")
        wstring speaker;                  // 말하는 놈
        shared_ptr<Engine::CTexture> Texture_Back_Path = nullptr;
        shared_ptr<Engine::CTexture> Texture_Char_Path = nullptr;
        vector<DialogueLine> lines;      // 대사 라인들
    };

}
namespace Client
{
    //ui

    enum class EUI
    {
        Test,
        Test1,
    };

    enum class INVENTYPE {PLAYER, CHEST, SHOP, END};


    struct Evt_InvenPlayerInit_Data
    {
        weak_ptr<class CInventory> Inven_ptr = {};
    };

    struct Evt_MouseToIndex_Data
    {
        _bool IsOnSlot = { false };
        _uint x = {};
        _uint y = {};
    };

    //struct Evt_ShipStats
    //{
    //    _uint BoatSpeed = {};
    //    _uint FishingSpeed = {};
    //    _uint Light = {};
    //    SEA_MASK SeaMask = {};
    //};
    
    struct Evt_AddMoney
    {
        _float money={};
    };

    struct Evt_GetFish
    {
        _uint DefID = { ID_Absence };
        Fish_Inst fishInst = {};
    };

    struct Evt_ToolTip
    {
        _bool isHold = {};
        Item_Inst itemInst = {};
        LOCATIONSTATE locationState = {};
    };

    enum class HUD { SKILL, COMPASS , TOP, BOAT, ETC, END };

    enum class NPC {MAYOR, SHIPWRIGHT, FISHMONGER, TRADER, END};

    enum class LOCATIONSTATE { REPAIR_SHOP, SHOP, SHOP_INVEN, FISHING, SEA, FISH_SHOP, VILLAGE, END };

	enum class E_PLAYERSTATE {REPAIR_SHOP, FISH_SHOP, FISHING, SEA, STORAGE , VILLAGE ,END };


    struct Wave_Desc
    {
        _float2 dir = {};
        float waveHeight = {};
        float waveLength = {};
        float speed = {};

        _float3 Padding = {}; // 4의 배수로 
    };

    struct Zone
    {
        // 0~1
        float start;
        float end;
        float padding1;
        float padding2;

    };



    enum CAM_MODE { FOLLOW, LERP, STOP, SHACK, END };
    struct CAM_DESC
    {
        virtual ~CAM_DESC() = default;
        CAM_MODE eMode;
        function<void()> OnComplete = nullptr;
    };

   
    struct CAM_LERP_DESC : public CAM_DESC
    {
        CAM_LERP_DESC() = default;

        _float3 vTargetPos;
        _float3 vTargetRot;
        _float  fDuration;

        _float fFov = 0;
        _bool IsLerpTarget = false;
        weak_ptr<Engine::CGameObject> m_Target = {};
    };

    
    struct CAM_FOLLOW_DESC : public CAM_DESC
    {
        CAM_FOLLOW_DESC() = default;

        _float fDistance;      
        _float fPitch;
        _float fYaw;
        _float3 vCurrentPos;
    };

   
    struct CAM_SHAKE_DESC : public CAM_DESC
    {
        _float fIntensity;   
        _float fDuration;    
    };


    struct Evt_ChangeCam
    {
        deque<shared_ptr<CAM_DESC>> commands;
    };

    struct Evt_ShipStat
    {
        _uint EngineSpeed = {};
        _uint LightIntensity = {};
        _uint FishingSpeed = {};
        SEA_MASK SeaMask = {};
        _float InvenMoney = {};
    };


    struct Evt_Demage
    {
        float ShakeTime = {};
        float ShakePower = {};
        float DeAc = {};

        int DemageCount = {};
    };

    struct Evt_Cam_Arrived
    {
        E_PLAYERSTATE playerstate = {};

    };

    struct Evt_Dialogue_Finish
    {

    };

    struct Evt_EndState
    {
        //E_PLAYERSTATE End_playerstate = {};
    };


    struct Evt_ChangeState
    {
        E_PLAYERSTATE playerstate = {};
    };

    
        struct Evt_FishingData
    {
        int Fish_ID;
    };

}