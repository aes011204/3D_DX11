#pragma once
#include  "Engine_Define.h"
#include  "Client_Define.h"


namespace Client
{
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
        vector<OccCell> Occ[3] = {}; // uint16_t //Shape_Mask 의 좌표 캐싱 // 회전도 같이 캐싱 // ex) {(0,0),(1,0),(2,0),(0,1),(0,2)}
        
    };

     struct Mutation
     {
         ID_uint Species_ID = { ID_Absence }; ID_uint Mutation_ID = { ID_Absence };
         _string MutName = {}; _string MutDesc = {}; _string MutTexturePath = {};
         _float MutCost = { 0 };
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


    struct Fish_Inst { FRESHNESS freshness = FRESHNESS::END; ID_uint mutation_ID = { ID_Absence }; };
    struct Equip_Inst { bool IsBroken = false; };

    /// 아이템 인스턴스 
    struct Item_Inst
    {
        ID_uint ItemDef_ID = { ID_Absence }; 
        ID_uint ItemInst_ID = { ID_Absence };
        ITEM_TYPE ItemType = ITEM_TYPE::END; // 이건 그냥 캐싱용으로 두자

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

        Shape ItemShape = {};
        _uint SizeNum = { 0 };

        variant<monostate, Fish_Def, Equip_Def, Material_Def, Trinket_Def>  TypeDef;

	};

}


namespace Client
{
    /// 슬롯
    struct Slot
    {
        ID_uint ItemInst_ID = { ID_Absence };
        SLOT_TYPE slotType = SLOT_TYPE::END;
        _bool IsLock = { true };
        _bool IsBroken = { false };
    };

    enum class PLACE_COLOR { RED, ORANGE, GREEN ,END};
}


namespace Client
{
    // 다이어로그

 // 선택지 구조체
    struct DialogueChoice
    {
        string leftText;           // 왼쪽 선택지 ("살해")
        string rightText;          // 오른쪽 선택지 ("노예 ㄱㄱ")
        string leftNextId;
        string rightNextId;
    };

    // 대사 한 줄
    struct DialogueLine
    {
        _int   lineIndex;           // 라인 순서 (0, 1, 2...)
        string speaker;             // 말하는 놈 ("보스", "플레이어", "")
        string text;                // 대사 내용
        _bool  hasChoices;          // 선택지가 있는 라인인지

        DialogueChoice choice;           // 선택지가 있을 때

        _float leftPadding = 0.f;

        string voicePath;
    };

    // 대화 묶음 (하나의 대화 시퀀스)
    struct Dialogue
    {
        string dialogueId;               // 대화 ID ("boss1_intro", "boss1_battle")
        string speaker;                  // 말하는 놈

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
}