#pragma once
#include "Entity.h"

// ui 그 자체로도 생성 X
//클라에서 생성할용도의 버튼 이미지 등은 이 클래스를 상속 받아 엔진에 만들예정
// 특별한 경우만 프로토 타임을 이용 할것임
// 클라에서 상속 받을 일 업음 클라는 조립만 / 패널을 이용한 상속은 O

NS_BEGIN(Engine)

class CUITransform;

class ENGINE_DLL CUI abstract :
    public CEntity
{
protected:
    CUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI(const CUI& prototype);
public:
    virtual ~CUI() = default;

public:
    // 이건 자식에서 구현 X
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    //virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta,bool& bMouseHold);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

    void UI_Active(); //  UI 활성활시 호출되는 함수//
    void UI_InActive(); // UI 비활성활시 호출되는 함수//
    void Set_UI_Disabled(bool isChangeEvent); // UI 가보이는데 클릭은 안되는거 

    void UI_Clear();

    HRESULT Add_Child(shared_ptr<CUI> child, _bool KeepWorldRect);


/// <IMGUI>
    const vector<shared_ptr<CUI>>& GetChildren() const { return m_Children; }
/// </IMGUI>

protected:
    // ui의 생명주기 정책에 따라 앤진 생명주기 안에서 호출 함
    virtual void OnInit() {}; // 생성 될떄
    virtual void OnActive() {}; // 불러 올떄마다 해야하는 일 여기서 (정보 새로 셋팅 등) 
    virtual void OnInActive() {}; // 비활 될떄 
    virtual void OnDisabled() {}; // 보이는데 클릭 안되는거 //돈 없을떄 
    virtual void OnUpdate(const _float& timeDelta) {};
    virtual void OnLateUpdate() {};
    virtual void OnRender() {};
    virtual void OnClear() {}; // 이건 죽을때 실행되는거

public:
    bool IsLayoutTarget() { return m_bLayoutTarget; }
    class shared_ptr<CUITransform> GetUITransform() { return m_UITransform; }

protected:
    vector<shared_ptr<CUI>> m_Children = {};
    weak_ptr<CUI> m_Parent = {  };

    class shared_ptr<CUITransform> m_UITransform = {nullptr};

private:
    int m_bZOrder = { 1 };

    bool m_bEnabled = { true }; // “위에 다른 팝업이 떠서 아래 UI가 조금 보이긴 하지만 update는 안하는 상태”
    bool m_bVisible = { true }; // 렌더 여부
    bool m_bInteractable = { true };

    bool m_bLayoutTarget = { false };

    bool m_bInitialized = { false };

protected:
    bool m_bHovered = { false };
public:
    //static shared_ptr<CUI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    //virtual shared_ptr<CUI> Clone(void* pArg);
	void Free()override;
};

NS_END