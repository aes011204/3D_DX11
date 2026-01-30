#pragma once
#include "Base.h"


NS_BEGIN(Engine)
	class ENGINE_DLL CGameInstance final : public CBase
{
    DECLARE_SINGLETON(CGameInstance)

private:
    CGameInstance();
    virtual ~CGameInstance() = default;

public:
    HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
    ImGuiContext* GetContext();
    void Update_Engine(float fTimeDelta);
    void Draw();

    void Clear_Resources(_uint iLevelIndex);

   
public:  /* For.Graphic_Device */
    HRESULT Clear_Buffers(const _float4* pClearColor);
    HRESULT Bind_BackBufferRenderTarget(HWND hwnd);
    HRESULT Present();

public: /* For.IMGUI*/
    HRESULT Resize(_uint g_RsizeWidth, _uint g_RsizeHeight);;

public: /* For.TimerManager*/
    HRESULT Add_Timer(const _wstring& strTimeTag);
    _float Compute_TimeDelta(const _wstring& strTimeTag);

public:/* For.levelManager*/
    HRESULT Change_Level(_uint iNewLevelIndex, class CLevel* pNewLevel);

public: /* For.PrototypeManager*/
    HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
    CBase* Clone_Prototype(PROTOTYPE ePrototy, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);;


public: /*For.GameObject_Manager*/
    HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);


public: /*For.Renderer*/
    void Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);


private:
    class CGraphic_Device* m_pGraphic_Device = { nullptr };
    class CTimer_Manager* m_pTimer_Manager = { nullptr };
    class CLevel_Manager* m_pLevel_Manager = { nullptr };
    class CPrototype_Manager* m_pProto_Manager = { nullptr };
    class CObject_Manager* m_pObject_Manager = { nullptr };
    class CRenderer* m_Renderer = { nullptr };

    class CImguiManager* m_pImgui_Manager = { nullptr };

public:
    void Release_Engine();
    virtual void Free() override;
};


NS_END
