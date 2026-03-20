#pragma once
#include "UIButton.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUISlot :
    public CUIButton
{
public:
    struct SLOT_DESC : public CUIButton::UIBUTTON_DESC
    {
        _uint slotType = {};
    };
private:
    explicit CUISlot(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    explicit CUISlot(const CUISlot& prototype);
public:
    virtual ~CUISlot()= default;

public:
    // ui의 생명주기 정책에 따라 앤진 생명주기 안에서 호출 함
    HRESULT OnInit(void* pArg)override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;

    void SetGridIndex(_float2 _GridIndex) { m_GridIndex = _GridIndex; }
    _float2 GetGridIndex() { return m_GridIndex; }

    bool IsAcceptable(_uint itemType)
    {
        if (m_Occupied == false)
        {
            if (m_slotType == itemType || m_slotType == 0)
                return true;
        }
        return false;
    }

private:
    _uint m_slotType;// 클라에서 enum으로 해서 (uint)로 해서 전달

    _float2 m_GridIndex = {};
    bool m_Occupied = false;
    //패널이 bool m_OccupiedTable[Rows][Cols] 같은 2차원 배열을 따로 들고 관리하는 방법도 있음
    shared_ptr<CUI> m_OccupiedItem = nullptr;


public:
    static shared_ptr<CUISlot> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free()override;


};

NS_END