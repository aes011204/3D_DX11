#pragma once
#include "Entity.h"
#include "UITransform.h"
// ui 그 자체로도 생성 X
//클라에서 생성할용도의 버튼 이미지 등은 이 클래스를 상속 받아 엔진에 만들예정
// 특별한 경우만 프로토 타입을 이용 할것임- 보통은 안쓰고 클라패널에서 데이터를 이용해 셋팅
// 클라에서 상속 받을 일 업음 클라는 조립만 / 패널을 이용한 상속은 O

NS_BEGIN(Engine)

//class CUITransform;

class ENGINE_DLL CUI abstract :
    public CEntity
{
public:
        struct UI_DESC : public CUITransform::UITRANSFORM_DESC
    {
            _uint ZOrder = {1};
            _bool NoDISTACH = false;
    };
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

    virtual void UI_Active(); //  UI 활성활시 호출되는 함수//
    void UI_InActive(); // UI 비활성활시 호출되는 함수//
    void Set_UI_Disabled(bool isChangeEvent); // 클릭은 가능하고 근대 활성환ㄴ 안된상태 // UI 가보이는데 클릭은 안되는거 

    void UI_Clear();

    HRESULT Add_Child(shared_ptr<CUI> child, _wstring UITag, _bool KeepWorldRect);

    HRESULT Bind_ShaderResource(shared_ptr<CShader> pShader, const _char* pConstantName, D3DTS eTransformState);

    void Set_Zorder(_uint Z);

    weak_ptr<CUI> Find_Children(_wstring strTag);
    const map<_wstring, weak_ptr<CUI>>& Get_mapChildren() const { return m_mapChildren; }

    _bool Get_NO_DETACH() { return m_NO_DESTACH; }

    void Set_Interactive(_bool b) { m_bInteractable = b; }

    void Save_ToJson(nlohmann::json& j)override;
    void Load_FromJson(nlohmann::json& j)override;

    UI_STATE Get_UIState() { return m_UIState; }
    void Set_ActiveForCustom() { m_UIState = UI_STATE::ACTIVE; m_bEnabled = true; m_bVisible = true; }; // 특이한 경우만 쓰기 커스텀UI_Active();만들떄만 
/// <IMGUI>
    const vector<shared_ptr<CUI>>& GetChildren() const { return m_Children; }
    void OnGui() override;
/// </IMGUI>

protected:
    // ui의 생명주기 정책에 따라 앤진 생명주기 안에서 호출 함
    virtual HRESULT OnInit(void* pArg) { return S_OK; }; // 생성 될떄
    virtual void OnActive() {}; // 불러 올떄마다 해야하는 일 여기서 (정보 새로 셋팅 등) 
    virtual void OnInActive() {}; // 비활 될떄 
    virtual void OnDisabled() {}; //클릭은 가능하고 근대 활성환ㄴ 안된상태           // 보이는데 클릭 안되는거 //돈 없을떄 //  이거는 슬랏에 넣자
    virtual void OnUpdate(const _float& timeDelta) {};
    virtual void OnLateUpdate() {};
    virtual HRESULT OnRender() { return S_OK; };
    virtual void OnClear() {}; // 이건 죽을때 실행되는거

    virtual HRESULT Bind_ShaderResources() { return S_OK; };


    _bool m_bRenderReady = true;
public:
    bool IsLayoutTarget() { return m_bLayoutTarget; }
    void Set_LayoutTarget(_bool layoutTarget) { m_bLayoutTarget = layoutTarget; }
    class shared_ptr<CUITransform> GetUITransform() { return m_pUITransformCom; }

    vector<shared_ptr<class IModifier>> m_behavior; // 인터페이스 클래스
protected:
    vector<shared_ptr<CUI>> m_Children = {};
    weak_ptr<CUI> m_Parent = {  };

    class shared_ptr<CUITransform> m_pUITransformCom = {nullptr};
    _float4x4					m_TransformationMatrices[ETOI(D3DTS::END)];
    bool m_bIsDirty_Zorder = { false };

    // 검색용
    map<_wstring, weak_ptr<CUI>> m_mapChildren = {};
protected: //임시 private으로 들어갈거임
    int m_ZOrder = { 1 };
private:
    UI_STATE m_UIState = { UI_STATE::END };

    bool m_bLayoutTarget = { false };

    bool m_bInitialized = { false };
    bool m_bEnabled = { true }; // “위에 다른 팝업이 떠서 아래 UI가 조금 보이긴 하지만 update는 안하는 상태”
    bool m_bVisible = { true }; // 렌더 여부

    bool m_NO_DESTACH = { false };
protected:

    bool m_bHovered = { false };
    bool m_bInteractable = { true };
public:
    //static shared_ptr<CUI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
   // virtual shared_ptr<CUI> Clone(void* pArg);
	void Free()override;
};

NS_END