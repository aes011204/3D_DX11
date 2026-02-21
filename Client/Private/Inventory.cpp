#include "Inventory.h"
#include "ItemDB.h"

CInventory::CInventory()
{
    w = 9;
    h = 11;
}

HRESULT CInventory::Initialize()
{

    // 아이템 갯수기준
    m_Inventory.reserve(10);

    // 슬롯기준 (최대 인밴)
    m_InvenSlot.resize(w*h);

    Init_BoatUpgrade();
    Upgrade_Boat(m_BoatUpgrade_type[0]); // 젤 처음

    return S_OK;
}

void CInventory::Update(_float fTimeDelta)
{
  
}

Item_Inst CInventory::AddItem(Item_Inst itemInst, _int BaseX, _int BaseY)
{
    // 밑에 아이템 한개랑 바꿔치기 가능 : 주황 / 불가능 : 빨강 / 가능 : 초록
    PLACE_COLOR color = {};
    int instID = CanPlace(itemInst, BaseX, BaseY, color); // 여기서 한번 더 할필요가 있나?
    Item_Inst holdInst = {};

    switch (color)
    {
    case PLACE_COLOR::GREEN:
	    {
            if (instID != ID_Absence)
                MSG_BOX("Wrong Item");

            // 인밴에 바로 들어감
            PlaceOn_Inven(itemInst, BaseX, BaseY);

            return holdInst; // 초기화된 빈인스턴스

	    }
    case PLACE_COLOR::ORANGE:
        {
            // 넣을려고 하는 아이템과 원래 있던 하나의 아이템 스왑
            // 기존 인밴에 있던 inst 제거
			holdInst = RemoveFrom_Inven(instID);
            //
            PlaceOn_Inven(itemInst, BaseX, BaseY);
            //
            return holdInst; // 스왑된 인스턴스 
        }
    case PLACE_COLOR::RED:
        {
        if (instID != -1)
            MSG_BOX("Wrong Item");
    		// 불가능 
        return itemInst; // 원래 받아온 인스턴스 그대로 

        }
    }
            

    // 초록
}

_int CInventory::CanPlace(Item_Inst& itemInst, _uint BaseX, _uint BaseY, PLACE_COLOR& color)
{
    // 해당 아이템의 모양 + BaseX,Y
    const Item_Def& def = CItemDB::GetInstance()->GetItemByID(itemInst.ItemDef_ID);

    int absenceNum = {};
    _uint ID_First = {};

    for (int i = 0;i < def.ItemShape.Occ[itemInst.Rotation].size(); i++)
    {
        _int fx = def.ItemShape.Occ[itemInst.Rotation][i].dx + BaseX;
        _int fy = def.ItemShape.Occ[itemInst.Rotation][i].dy + BaseY;
        // finalCells.push_back({ fx,fy });
  
      // 락이랑 겹치는지, 밑아이템 하나랑 겹치는지 , 바로 놓을 수 있는지

        const Slot& the_Slot = m_InvenSlot[fy * w + fx];
        ID_First = the_Slot.ItemInst_ID;


        if (the_Slot.IsLock== true|| the_Slot.IsBroken == true)
        {
	        //하나라도 락이랑 겹치면 불가 - 빨강
            color = PLACE_COLOR::RED;
        	return -1;
        }

        if(the_Slot.ItemInst_ID == ID_Absence)
        {
            absenceNum++;
        }
        else if (the_Slot.ItemInst_ID != ID_First)
        {
            //다른 종류의 아이템이 2 개 이상 겹쳐 있다 - place 불가 - 빨간
            color = PLACE_COLOR::RED;
            return -1;
        }
    }

	if(absenceNum == def.ItemShape.Occ[itemInst.Rotation].size())
	{
        // 해당 칸 전부 빈칸이면 - 그린
        color = PLACE_COLOR::GREEN;
        return ID_Absence;
	}

  

    // 겹치는 아이템이 있긴한데 동일한 아이템이다 들고 있는거랑 스위치 가능 - 주황
    color = PLACE_COLOR::ORANGE;
    return static_cast<_int>(ID_First);
}

Item_Inst CInventory::RemoveFrom_Inven(int inst_id)
{

    Item_Inst inst = {};
   // 인밴에서 지우기
    for (auto it = m_Inventory.begin(); it != m_Inventory.end(); it++) // 하나 지우고 바로 나갈꺼라 ㄱㅊ
    {
	    if((*it).ItemInst_ID == inst_id)
	    {
            inst = ((*it));
            m_Inventory.erase(it);
            break;
	    }
    }

    // 인밴슬롯에서 지우기

  //  for (int i = 0;i < m_InvenSlot.size(); i++)
  //  {
		////전체 순회하면서 해당 인스턴스 아이디 다 지우기
  //      if(m_InvenSlot[i].ItemInst_ID== inst_id)
  //      {
  //          m_InvenSlot[i].ItemInst_ID = 0;// 0 이 아무것도 없음 
  //      }
  //  }

    for(int i = 0; i <inst.CurBase.size(); i++)
    {
        m_InvenSlot[inst.CurBase[i].dy * w + inst.CurBase[i].dx].ItemInst_ID = 0;
    }


    return inst;
}

void CInventory::PlaceOn_Inven(Item_Inst itemInst, _int BaseX, _int BaseY)
{
    // 인밴에 넣기
    m_Inventory.push_back(itemInst);

        // 인밴슬롯에 넣기
    const Item_Def& def = CItemDB::GetInstance()->GetItemByID(itemInst.ItemDef_ID);

    for (int i = 0;i < def.ItemShape.Occ[itemInst.Rotation].size(); i++)
    {
        _uint fx = def.ItemShape.Occ[itemInst.Rotation][i].dx + BaseX;
        _uint fy = def.ItemShape.Occ[itemInst.Rotation][i].dy + BaseY;
        // finalCells.push_back({ fx,fy });

      // 락이랑 겹치는지, 밑아이템 하나랑 겹치는지 , 바로 놓을 수 있는지

        m_InvenSlot[fy * w + fx].ItemInst_ID = itemInst.ItemInst_ID;
        itemInst.CurBase.push_back({ fx,fy });
    }

}

Item_Inst CInventory::TryMove_Item(_uint BaseX, _uint BaseY)
{
    ID_uint id = m_InvenSlot[BaseY * w + BaseX].ItemInst_ID;

    Item_Inst holdInst = RemoveFrom_Inven(id);

    return holdInst;
}

void CInventory::ThrowAwayFrom_Inven(_uint BaseX, _uint BaseY)
{
    //집고 있을떄랑 / 인밴에 있을떄 둘다 버릴수 있음

    // 집고 있는거 버리는건 컨트롤러에서 (집고 있는 아이템의 관리는 컨트로러)

    ID_uint id = m_InvenSlot[BaseY * w + BaseX].ItemInst_ID;

	RemoveFrom_Inven(id);

}

void CInventory::Apply_BaseMask(vector<Slot>& vecSlot)
{


}

void CInventory::Init_BoatUpgrade()
{
    for(int i =0; i < m_MaxBoatLevel; i++)
    {
        m_BoatUpgrade_type[i].resize(w * h);
       
    }
  
    //
    m_BoatUpgrade_type[0] = {
    'O','O','O','A','L','O','O','O','O',
    'O','O','A','A','A','A','O','O','O',
    'O','A','A','A','A','A','A','O','O',
    'O','R','R','A','A','A','R','O','O',
    'O','R','R','A','A','A','R','O','O',
    'O','A','A','A','A','A','R','O','O',
    'O','O','A','E','E','A','O','O','O',
    'O','O','O','E','E','O','O','O','O',
    'O','O','O','O','O','O','O','O','O',
    'O','O','O','O','O','O','O','O','O',
    'O','O','O','O','O','O','O','O','O',
    };

    m_BoatUpgrade_type[1] = {
    'O','O','O','A','L','O','O','O','O',
    'O','O','A','A','A','A','O','O','O',
    'O','A','A','A','A','A','A','O','O',
    'O','R','R','A','A','A','R','O','O',
    'O','R','R','A','A','A','R','O','O',
    'O','A','A','A','A','A','R','O','O',
    'O','O','A','E','E','A','O','O','O',
    'O','O','O','E','E','O','O','O','O',
    'O','O','O','O','O','O','O','O','O',
    'O','O','O','O','O','O','O','O','O',
    'O','O','O','O','O','O','O','O','O',
    };

    m_BoatUpgrade_type[2] = {
    'O','O','O','L','L','L','O','O','O',
    'O','O','A','A','A','A','A','O','O',
    'O','A','A','A','A','A','A','A','O',
    'R','R','A','A','A','A','A','R','R',
    'R','R','A','A','A','A','A','R','R',
    'R','R','A','A','A','A','R','R','R',
    'R','A','A','A','A','A','A','A','R',
    'O','A','A','A','A','A','A','A','O',
    'O','O','A','E','E','A','A','O','O',
    'O','O','E','E','E','A','A','O','O',
    'O','O','E','E','E','A','A','O','O',
    };

}

void CInventory::Upgrade_Boat(vector<_char> upgrade)
{
    //
    for (int th = 0; th < h; th++)
    {
        for (int tw = 0; tw < w; tw++)
        {
            _int tmp = static_cast<_int>(upgrade[th * w + tw]);
            switch (tmp)
            {
            case  static_cast<_int>('O'):
                m_InvenSlot[th * w + tw].IsLock = true;
                m_InvenSlot[th * w + tw].slotType = SLOT_TYPE::END;
                break;
            case static_cast<_int>('A'):
                m_InvenSlot[th * w + tw].IsLock = false;
                m_InvenSlot[th * w + tw].slotType = SLOT_TYPE::ANY;
                break;
            case static_cast<_int>('E'):
                m_InvenSlot[th * w + tw].IsLock = false;
                m_InvenSlot[th * w + tw].slotType = SLOT_TYPE::ENGINE;
                break;
            case static_cast<_int>('L'):
                m_InvenSlot[th * w + tw].IsLock = false;
                m_InvenSlot[th * w + tw].slotType = SLOT_TYPE::LIGHT;
                break;
            case static_cast<_int>('R'):
                m_InvenSlot[th * w + tw].IsLock = false;
                m_InvenSlot[th * w + tw].slotType = SLOT_TYPE::ROT;
                break;
            case static_cast<_int>('N'):
                m_InvenSlot[th * w + tw].IsLock = false;
                m_InvenSlot[th * w + tw].slotType = SLOT_TYPE::NET;
                break;
            }
            // 아 비트 플레그 해야 하나...
        }
    }

}

shared_ptr<CInventory> CInventory::Create()
{
    shared_ptr<CInventory> pInstance ( new CInventory());

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CInventory");
    }
    return pInstance;
}

void CInventory::Free()
{
	__super::Free();
}
