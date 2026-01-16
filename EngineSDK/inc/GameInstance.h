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
    void Update_Engine(float fTimeDelta);


   
public:  /* For.Graphic_Device */
    HRESULT Clear_Buffers(const _float4* pClearColor);
    HRESULT Bind_BackBufferRenderTarget(HWND hwnd);
    HRESULT Present();

    HRESULT Resize(_uint g_RsizeWidth, _uint g_RsizeHeight);;

public: /* For.TimerManager*/
    HRESULT Add_Timer(const _wstring& strTimeTag);
    _float Compute_TimeDelta(const _wstring& strTimeTag);

private:
    class CGraphic_Device* m_pGraphic_Device = { nullptr };
    class CTimer_Manager* m_pTimer_Manager = { nullptr };
public:
    virtual void Free() override;
};

NS_END