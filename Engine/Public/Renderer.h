#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameInstance;
class CEntity;


class CRenderer :
    public CBase
{
private:
    CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> m_pContext);
public:
	virtual ~CRenderer();

public:
    HRESULT Initialize();
    void Add_RenderGroup(RENDERGROUP eRenderGroup, shared_ptr<class CEntity> p_GameObject);
    void Draw();
#ifdef _DEBUG
    void Add_DebugenderGroup(shared_ptr < class  CComponent > pDebugComponent);
#endif
private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<CGameInstance> m_pGameInstance = { };

   // ComPtr <ID3D11DepthStencilState> m_pDepthDisableState = { nullptr };

    vector<shared_ptr<CEntity>> m_RenderObject[ETOI(RENDERGROUP::END)];

private:
    _float4x4 m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
    shared_ptr<class CVIBuffer_Rect> m_pVIBuffer = { nullptr };
    shared_ptr<class CShader> m_pShader = { nullptr };
#ifdef _DEBUG
    list<shared_ptr<class CComponent>>				m_DebugComponents;
#endif

private:
    void Render_Priority();
    void Render_NonBlend();
    void Render_Sea();
    void Render_NonLight();
    void Render_Blend();
    void Render_Lights();
    void Render_Combined();
    void Render_UI();


private:
#ifdef _DEBUG

    void Render_Debug();

#endif
public:
    static unique_ptr<CRenderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> m_pContext);
    void Free()override;

};

NS_END