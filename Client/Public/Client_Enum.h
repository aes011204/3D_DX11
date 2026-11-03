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
	class CMiniGame;
	class CInventory;
	class CMiniGame_Logic;
	class CFish;
	enum class ELocationState;

	// 아이템 관련 
	enum class EItemType { FISH, EQUIP, MATERIAL, PURSUIT, TRINKET, END };
	enum class EDayTime { DAY, NIGHT, END };
    enum class EFreshness { FRESH, STALE, ROTTING, INFECTED, END };
    enum class ESlotType { ANY, ENGINE, LIGHT, ROT, NET, END };
    enum class EEquipType { ROD,  ENGINE, NET , LIGHT, POT, CRAP, END };

	enum class ESeaType : _ubyte { COASTAL, SHALLOW, OCEANIC, ABYSSAL, HADAL, MANGROVE, VOLCANIC, ICE, CRAB, END };
	// 비트 플레그로쓸거임 //uint8_t : “하나 선택” → 8비트로 충분
    using SEA_MASK = _uint; // uint16_t (_ushort) : 16개 까지 가능 // 여러 개 ON/OFF 집합” → 16비트로 넉넉하게 // _uint 32 비트로 해도됨 
    constexpr SEA_MASK Set_SeaType_Mask(ESeaType SeaType) { return SEA_MASK{ 1u } << static_cast<uint32_t>(SeaType);
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
        _uint width , height = { 0 };
        vector<_ubyte> shapeMask = {}; // 0,1 로만 할건데 bool 문제생길수있음 // ex) [ 111, 101 ] 
        vector<OccCell> occ[4] = {}; // uint16_t //Shape_Mask 의 좌표 캐싱 // 회전도 같이 캐싱 // ex) {(0,0),(1,0),(2,0),(0,1),(0,2)}
        
    };

     struct Mutation
     {
         ID_uint speciesId = { ID_Absence }; ID_uint mutationId = { ID_Absence };
         _string mutName = {}; _string mutDesc = {}; _string mutTexturePath = {};
         _float mutCost = { 0 };
         shared_ptr<Engine::CTexture> texture = { nullptr };
     };

    struct FishDef
    {
        float cost = { 0 }; EDayTime fishTime = EDayTime::END; SEA_MASK seaTypeMask = { 0 };
        _bool isRod = false; _bool isTrawl = false; _bool  isPot = false;

        vector<Mutation> vecMutation = {}; // array 는 현제 몇개가 있는지 카운트하는기능이 없어서 struct로 만들어서 / 근데 난 귀찬
    };

		struct EquipEngine { _uint researchLv = { 0 }; _uint speedKn = { 0 }; };
        struct EquipLight { _uint lumen = { 0 }; _uint range = { 0 }; };
        struct EquipRod {_uint researchLv = { 0 }; SEA_MASK seaTypeMask = { 0 }; _uint speedPer = { 0 }; };
        struct EquipNet {_uint researchLv = { 0 }; SEA_MASK seaTypeMask = { 0 }; _uint capSizeX, capSizeY = { 0 };
        	_uint yield = { 0 }; _uint lastFor = { 0 }; };
        struct EquipCrabPot { _uint researchLv = { 0 }; _uint capSizeX, capSizeY = { 0 };
        	_uint yield = { 0 }; _uint lastFor = { 0 };
        };

    struct EquipDef
    {
        float cost = { 0 }; _uint installTime = { 0 }; EEquipType equipType = EEquipType::END;
        variant<EquipEngine, EquipLight, EquipRod, EquipNet, EquipCrabPot> equipTypeEffect;
    };


    struct MaterialDef { float cost = { 0 }; };
    struct TrinketDef { float cost = { 0 }; };


    struct FishInst { EFreshness freshness = EFreshness::FRESH; ID_uint mutationId = { ID_Absence }; _float size = {}; };
    struct EquipInst { bool isBroken = false; };

    /// 아이템 인스턴스 
    struct ItemInst
    {
        ID_uint itemDefId = { ID_Absence }; 
        ID_uint itemInstId = { ID_Absence };
        EItemType itemType = EItemType::END; // 이건 그냥 캐싱용으로 두자

        bool isMutation = { false };
        Mutation mutationCaching = {};


        shared_ptr<Engine::CTexture> cachingTexture = nullptr;


        _float2 baseXY = {};

        vector<OccCell> curBase = {};
        _uint rotation = { 0 };

        variant<monostate, FishInst, EquipInst>  typeDef;

    };


    /// 아이템 정의
    struct ItemDef
	{
		ID_uint itemId = { ID_Absence }; // 이제 id 는 0 을 아무것도 없는 가짜 상태로
		EItemType itemType = EItemType::END;
		_string itemName = {};
		_string itemDesc = {};
        _string texturePath = {};
        
    	shared_ptr<Engine:: CTexture> texture = nullptr;

        Shape itemShape = {};
        _uint sizeNum = { 0 };

        variant<monostate, FishDef, EquipDef, MaterialDef, TrinketDef>  typeDef;

	};
    
    static wstring freshToWstr(EFreshness freshness)
    {

        switch (freshness) {
        case EFreshness::FRESH: return L"신선한";
        case EFreshness::ROTTING: return L"퀴퀴함";
        case EFreshness::INFECTED: return  L"감염된";
        default: return L"UNKNOWN";
        }
    }
    static const wstring GetSeaTypeName(ESeaType Type)
    {
        switch (Type)
        {
        case ESeaType::COASTAL:
            return L"COASTAL";
        case ESeaType::SHALLOW:
            return L"SHALLOW";
        case ESeaType::OCEANIC:
            return L"OCEANIC";
        case ESeaType::ABYSSAL:
            return L"ABYSSAL";
        case ESeaType::HADAL:
            return L"HADAL";
        case ESeaType::MANGROVE:
            return L"MANGROVE";
        case ESeaType::VOLCANIC:
            return L"VOLCANIC";
        case ESeaType::ICE:
            return L"ICE";
        case ESeaType::CRAB:
            return L"CRAB";
        case ESeaType::END:
            return L"END";
        default:
            return L"UNKNOWN";
        }
    }

    static wstring OcceanToWstr(SEA_MASK sea)
    {
        wstring result = L"";

        for (_uint i = 0; i < ETOI(ESeaType::END); ++i)
        {
            SEA_MASK typeMask = 1u << i;

            if (sea & typeMask)
            {
                if (!result.empty())
                    result += L" | ";

                result += GetSeaTypeName(static_cast<ESeaType>(i));
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
    enum class EPlaceColor { RED, ORANGE, GREEN ,END};
    /// 슬롯
    struct Slot
    {
        ID_uint itemInstId = { ID_Absence };
        ESlotType slotType = ESlotType::ANY;
        _bool isLock = { false };
        _bool isBroken = { false };
        EPlaceColor slotColor = EPlaceColor::END;
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

    enum class EUiType
    {
        Test,
        Test1,
    };

    enum class EInvenType { PLAYER, CHEST, SHOP,STORAGE, END, NONE };


    struct Evt_InvenPlayerInitData
    {
        weak_ptr<class CInventory> invenPtr = {};
    };

    enum class EShopTab { ROT, ENGINE, NET, LIGHT, END , NONE};
    struct Evt_InvenStorageInitData
    {
        EInvenType invenType = { EInvenType ::NONE};
        EShopTab shopTabType = {  };
        weak_ptr<class CInventory> invenPtr = {};
    };



    struct Evt_InvenShopInitData
    {
        EShopTab shopTabType = {  };
        weak_ptr<class CInventory> invenPtr = {};
    };
    struct  Evt_MouseToIndexData
    {
        _bool IsOnSlot = { false };
        _uint x = {};
        _uint y = {};

        _bool IsPlayer = {};

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
        _float money = {};
    };

    struct Evt_GetFish
    {
        _uint DefID = { ID_Absence };
        FishInst fishInst = {};
    };

    struct Evt_ItemSold
    {
        ID_uint itemInstId = ID_Absence;
    };

    struct Evt_ItemHovered
    {
        _bool isHold = {false};
        ItemInst itemInst = {};
        ELocationState locationState = {};
        _bool IsPlayer = {};
    };

    enum class EHud { SKILL, COMPASS, TOP, BOAT, ETC, END };

    enum class ENpc { MAYOR, SHIPWRIGHT, FISHMONGER, TRADER, END };

    enum class ELocationState { REPAIR_SHOP, SHOP, SHOP_INVEN, FISHING, SEA, FISH_SHOP, VILLAGE, END };

    enum class EPlayerState { REPAIR_SHOP, FISH_SHOP, FISHING, SEA, STORAGE, VILLAGE,INTERACT, END };




    struct WaveDesc
    {
        _float2 dir = {};
        float waveHeight = {};
        float waveLength = {};
        float speed = {};

        _float3 padding = {}; // 4의 배수로 
    };

    struct Zone
    {
        // 0~1
        float start;
        float end;
        float padding1;
        float padding2;

    };



    enum class ECamMode { FOLLOW, LERP, STOP, SHAKE, END };
    struct CamDesc
    {
        virtual ~CamDesc() = default;
        ECamMode eMode;
        function<void()> OnComplete = nullptr;
    };


    struct CamLerpDesc : public CamDesc
    {
        CamLerpDesc() = default;

        _float3 vTargetPos;
        _float3 vTargetRot;
        _float  fDuration;

        _float fFov = 0;
        _bool IsLerpTarget = false;
        weak_ptr<Engine::CGameObject> m_Target = {};
    };


    struct CamFollowDesc : public CamDesc
    {
        CamFollowDesc() = default;

        _float fDistance;
        _float fPitch;
        _float fYaw;
        _float3 vCurrentPos;
    };


    struct CamShakeDesc : public CamDesc
    {
        _float fIntensity;
        _float fDuration;
    };


    struct Evt_ChangeCam
    {
        deque<shared_ptr<CamDesc>> commands;
    };

    struct Evt_ShipStat
    {
        _uint engineSpeed = {};
        _uint lightIntensity = {};
        _uint fishingSpeed = {};
        SEA_MASK seaMask = {};
        _float invenMoney = {};
    };


    struct Evt_Damage
    {
        float shakeTime = {};
        float shakePower = {};
        float deAc = {};

        int damageCount = {};
    };

    struct Evt_CamArrived
    {
        EPlayerState playerstate = {};

    };

    struct Evt_DialogueFinish
    {

    };

    struct Evt_EndState
    {
        //E_PLAYERSTATE End_playerstate = {};
    };


    struct Evt_ChangeState
    {
        EPlayerState playerstate = {};
    };


    struct Evt_FishingData
    {
        int Fish_ID;
        weak_ptr<CFish> Fish ={};
        _float RodSpeed = {};
    };


    struct Evt_MiniGame
    {
        _bool IsOnZoon = {};


    };

    struct Evt_BindMiniGameLogic
    {
        shared_ptr<CMiniGame> logic;
    };
    struct Evt_OpenInventory
    {
        weak_ptr<CInventory> inven;
    };
    struct Evt_CloseInventory
    {
    };
    struct Evt_MoveToStorage
    {
    };
    struct Evt_VillageBtn
    {
        _uint type={};
    };
    struct Evt_FixAll
    {
    };
    struct Evt_FixAllDone
    {
    };
    struct Evt_RepairCost
    {
        _uint cost = {};
    };
    struct Evt_ChangeLevel
    {
        _bool IsChange = false;
        _uint level = {};
    };
    struct Evt_BoatLight
    {
        _bool Light = false;
    };

    enum class EInputResult
    {
        NONE,
        SUCCESS,
        FAIL,
        START
    };
    enum class EMiniGameType { BASIC_CIRCLE, DIAMOND, BALL, END };
    enum class EEffType { PLAYER,FISH,END };
}