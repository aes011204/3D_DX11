#include "UIImage.h"
#include "Texture.h"
#include "GameInstance.h"
#include "Shader.h"

CUIImage::CUIImage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUI{pDevice, pContext}
{
}

CUIImage::CUIImage(const CUIImage& prototype)
    :CUI{prototype}
{
}

HRESULT CUIImage::OnInit(void* pArg)
{

    m_bInteractable = false;


    UIIMAGE_DESC* pDesc = static_cast<UIIMAGE_DESC*>(pArg);

    if (FAILED(Ready_Components(pDesc->TextureComLevel, pDesc->TextureProtoName)))
        return E_FAIL;

    if (m_pTextureCom)
    {
        // 텍스처 해상도를 스케일에 대입
        m_pUITransformCom->SetSizeDelta(m_pTextureCom->Get_SizeFromSRV(0));
    }

    return S_OK;
}

void CUIImage::OnActive()
{
    __super::OnActive();
}

void CUIImage::OnInActive()
{
    __super::OnInActive();
    int a = 9;
}

void CUIImage::OnDisabled()
{
}

void CUIImage::OnUpdate(const _float& timeDelta)
{
  //  m_bInteractable = false;
}

void CUIImage::OnLateUpdate()
{
}

HRESULT CUIImage::OnRender()
{

    if (FAILED(m_pUITransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;


    if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CUIImage::OnClear()
{
}

HRESULT CUIImage::Ready_Components(_uint Level, _wstring protoName)
{
    if (FAILED(Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),& m_pVIBufferCom, nullptr)))
        return E_FAIL;
    if (FAILED(Add_Component(0, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),& m_pShaderCom, nullptr)))
        return E_FAIL;
    if (FAILED(Add_Component(Level, /*TEXT(protoName)*/protoName, TEXT("Com_Texture"),& m_pTextureCom, nullptr)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CUIImage> CUIImage::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CUIImage> pInstance(new CUIImage(pDevice, pContext), [](CUIImage* p) {p->Free();delete(p);});

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUIImage");
    }
    return pInstance;
}

void CUIImage::Free()
{
    __super::Free();
}
