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
    void Add_RenderGroup(RENDERGROUP eRenderGroup,class shared_ptr<CEntity> p_GameObject);
    void Draw();

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<CGameInstance> m_pGameInstance = { };

   // ComPtr <ID3D11DepthStencilState> m_pDepthDisableState = { nullptr };

    vector<shared_ptr<CEntity>> m_RenderObject[ETOI(RENDERGROUP::END)];


private:
    void Render_Priority();
    void Render_NonBlend();
    void Render_Sea();
    void Render_Blend();
    void Render_UI();

public:
    static unique_ptr<CRenderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> m_pContext);
    void Free()override;

};

NS_END