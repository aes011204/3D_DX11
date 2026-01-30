#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CRenderer :
    public CBase
{
private:
    CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* m_pContext);
    virtual ~CRenderer() = default;

public:
    HRESULT Initialize();
    void Add_RenderGroup(RENDERGROUP eRenderGroup,class CGameObject* p_GameObject);
    void Draw();

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

    list<class CGameObject*> m_RenderObject[ETOI(RENDERGROUP::END)];

private:
    void Render_Priority();
    void Render_NonBlend();
    void Render_Blend();
    void Render_UI();

public:
    static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* m_pContext);
    void Free()override;

};

NS_END