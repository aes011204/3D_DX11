#include "UIImage.h"
#include "Texture.h"
#include "GameInstance.h"
#include "Shader.h"


CUIImage::CUIImage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUIRenderable{pDevice, pContext}
{
}

CUIImage::CUIImage(const CUIImage& prototype)
    :CUIRenderable{ prototype }
{
}

HRESULT CUIImage::OnInit(void* pArg)
{

    m_bInteractable = false;


    UIIMAGE_DESC* pDesc = static_cast<UIIMAGE_DESC*>(pArg);

  /*  if (FAILED(Ready_Components(pDesc->TextureComLevel, pDesc->TextureProtoName)))
        return E_FAIL;*/

    //if (m_pTextureCom)
    //{
    //    // 텍스쳐 컴포넌트 안에서처리
    //    // 텍스처 해상도를 스케일에 대입
    //    m_pUITransformCom->SetSizeDelta(m_pTextureCom->Get_SizeFromSRV(0));
    //}

    //// 일단 초기화 -> CUi로 올림 
    //_float2 orignSize = m_pTextureCom->Get_SizeFromSRV(0);
    //m_SliceDesc.TexSize = orignSize;
    //m_SliceDesc.UISize = _float2(1.f, 1.f);// 어짜피 트렌스폼이 정함 최종 ui 사이즈
    //m_SliceDesc.PxSliceLRTB = _float4(orignSize.x / 3.f, orignSize.x / 3.f, orignSize.y / 3.f, orignSize.y / 3.f);

    //m_bUseNineSlice = pDesc->bUseNineSlice;
  
    //if (pDesc->PxSliceLRTB.x != 0.f && pDesc->PxSliceLRTB.y != 0.f 
    //    && pDesc->PxSliceLRTB.z != 0.f && pDesc->PxSliceLRTB.w != 0.f)
    //{
    //    m_SliceDesc.PxSliceLRTB = (pDesc->PxSliceLRTB);
    //}

    //m_SliceDesc.TileScale = max(1.f, pDesc->TileScale);
    
    return __super::OnInit(pDesc);;
}

void CUIImage::OnActive()
{
    __super::OnActive();
}

void CUIImage::OnInActive()
{
    __super::OnInActive();
}

void CUIImage::OnDisabled()
{
    __super::OnDisabled();
}

void CUIImage::OnUpdate(const _float& timeDelta)
{
    //if (m_bIsDirtyCom) {
    //    RebindCom();      // "바뀐 것"만 한 번 갱신
    //    m_bIsDirtyCom = false;
    //}


  //  m_bInteractable = false;
   /* m_SliceDesc.UISize = m_pUITransformCom->Get_FinalSize();*/

    __super::OnUpdate(timeDelta);

}

void CUIImage::OnLateUpdate()
{

   /* if (m_bUseNineSlice == true)
    {
        m_PassIndex = 1;

    }
    else
    {
        m_PassIndex = 0;
    }*/

    __super::OnLateUpdate();
}

HRESULT CUIImage::OnRender()
{

   /* if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

     if (FAILED(m_pShaderCom->Begin(m_PassIndex)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;*/

    return     __super::OnRender();
  
}

void CUIImage::OnClear()
{
}

//HRESULT CUIImage::Ready_Components(_uint Level, _wstring protoName)
//{
//    if (FAILED(Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),& m_pVIBufferCom, nullptr)))
//        return E_FAIL;
//    if (FAILED(Add_Component(0, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),& m_pShaderCom, nullptr)))
//        return E_FAIL;
//    if (FAILED(Add_Component(Level, /*TEXT(protoName)*/protoName, TEXT("Com_Texture"),& m_pTextureCom, nullptr)))
//        return E_FAIL;
//    if (m_pTextureCom && m_pUITransformCom) {
//        
//        m_pUITransformCom->SetSizeDelta(m_pTextureCom->Get_SizeFromSRV(0));
//    }
//    return S_OK;
//}
//
//HRESULT CUIImage::Bind_ShaderResources()
//{
//    if (FAILED(m_pUITransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
//        return E_FAIL;
//
//    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
//        return E_FAIL;
//
//    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
//        return E_FAIL;
//
//    if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
//        return E_FAIL;
//
//   /* if (m_PassIndex == 1)
//    {
//        if (FAILED(m_pShaderCom->Bind_RawValue("g_TexSize", &m_SliceDesc.TexSize, sizeof(_float2))))
//            return E_FAIL;
//        if (FAILED(m_pShaderCom->Bind_RawValue("g_UISize", &m_SliceDesc.UISize, sizeof(_float2))))
//            return E_FAIL;
//        if (FAILED(m_pShaderCom->Bind_RawValue("g_PxSliceLRTB", &m_SliceDesc.PxSliceLRTB, sizeof(_float4))))
//            return E_FAIL;
//        if (FAILED(m_pShaderCom->Bind_RawValue("g_TileScale", &m_SliceDesc.TileScale, sizeof(_float))))
//            return E_FAIL;
//    }*/
//
//    
//
//    return S_OK;
//}
//
//void CUIImage::RebindCom()
//{
//    // 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
//    m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
//    m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
//    m_pShaderCom = Get_Component<CShader>(L"Com_Shader");
//}

void CUIImage::Save_ToJson(nlohmann::json& j)
{
    __super::Save_ToJson(j);
}

void CUIImage::Load_FromJson(nlohmann::json& j)
{
    __super::Load_FromJson(j);
}

void CUIImage::OnGui()
{
    __super::OnGui();
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
