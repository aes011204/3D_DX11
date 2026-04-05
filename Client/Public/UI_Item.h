#pragma once
#include "UIImage.h"
#include "Client_Define.h"
#include "Client_Enum.h"

//인밴이 푸시 되면 아이템도 푸시
// 일단 안보이게 하고 홀드 되면 보이게
NS_BEGIN(Engine)
class CUIImage;
class CUISlot;
NS_END

NS_BEGIN(Client)
class CUI_Item :
    public CUIImage
{
public:
    struct ITEM_DESC : public CUIImage::UIIMAGE_DESC
    {
        
    };
private:
    CUI_Item(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_Item(const CUI_Item& prototype);
public:
    ~CUI_Item() = default;

public:
    void Set_Rotation(_uint rot) { m_HoldItem.Rotation = rot; };

    void HoldItem(Item_Inst HoldItemInst);

    void ReleaseItem();

    Item_Inst& Get_HoldItem() { return m_HoldItem; }

    void Set_HoldItem(const Item_Inst& _item) { m_HoldItem = _item; }

    HRESULT OnInit(void* pArg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;


private:
    bool m_bIsHold = {};
    Item_Inst m_HoldItem = {};
public:
    static shared_ptr<CUI_Item> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual void Free() override;
};

NS_END