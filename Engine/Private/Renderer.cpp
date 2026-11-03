#include "Renderer.h"
#include <chrono>

#include "DInput_Manager.h"
#include "GameInstance.h"
#include "Entity.h"


CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> m_context)
	: m_device(pDevice), m_context(m_context),
	m_gameInstance(CGameInstance::GetInstance())
{

}

CRenderer::~CRenderer() { Free(); };


HRESULT CRenderer::Initialize()
{
	_uint			iNumViewports = { 1 };
	D3D11_VIEWPORT	ViewportDesc{};

	m_context->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_winSize = { ViewportDesc.Width, ViewportDesc.Height };

	/* For.RenderTargets */
	if (FAILED(m_gameInstance.lock()->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Add_RenderTarget(TEXT("Target_Emissive"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	{

		if (FAILED(m_gameInstance.lock()->Add_RenderTarget(TEXT("Target_Blur1"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
			return E_FAIL;

		if (FAILED(m_gameInstance.lock()->Add_RenderTarget(TEXT("Target_Blur2"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
			return E_FAIL;
	}


	/* For.MRTs */
	if (FAILED(m_gameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_gameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_gameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_gameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
		return E_FAIL;

	{
		if (FAILED(m_gameInstance.lock()->Add_MRT(TEXT("MRT_Blur1"), TEXT("Target_Blur1"))))
			return E_FAIL;
		if (FAILED(m_gameInstance.lock()->Add_MRT(TEXT("MRT_Blur2"), TEXT("Target_Blur2"))))
			return E_FAIL;
	}
	m_VIBuffer = CVIBuffer_Rect::Create(m_device, m_context);
	if (nullptr == m_VIBuffer)
		return E_FAIL;

	m_shader = CShader::Create(m_device, m_context, TEXT("../bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements);
	if (nullptr == m_shader)
		return E_FAIL;

	XMStoreFloat4x4(&m_worldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));


#ifdef _DEBUG
	if (FAILED(m_gameInstance.lock()->Ready_RT_Debug(TEXT("Target_Diffuse"), 200.f, 130.f, 400.f, 225.f)))
		return E_FAIL;
	if (FAILED(m_gameInstance.lock()->Ready_RT_Debug(TEXT("Target_Normal"), 200.f, 370.f, 400.f, 225.f)))
		return E_FAIL;
	if (FAILED(m_gameInstance.lock()->Ready_RT_Debug(TEXT("Target_Shade"), 600.f, 130.f, 400.f, 225.f)))
		return E_FAIL;
	if (FAILED(m_gameInstance.lock()->Ready_RT_Debug(TEXT("Target_Emissive"), 600.f, 370.f, 400.f, 225.f)))
		return E_FAIL;
#endif

    Initialize_GpuTiming();
	return S_OK;
}

void CRenderer::Add_RenderGroup(ERenderGroup eRenderGroup, shared_ptr<CEntity> pGameObject)
{
	if(eRenderGroup == ERenderGroup::END|| pGameObject == nullptr)
	{
		MSG_BOX("Failed to Add : Renderer");
		return;
	}
	m_renderObject[ETOI(eRenderGroup)].push_back(pGameObject);

}

void CRenderer::Draw()
{
    const int timingSlot = Begin_GpuTiming();
	

	Render_Priority();

	Render_NonBlend();
	Render_Lights();

	//RenderBloom();
	Render_Combined();

	Render_NonLight();
	

    if (timingSlot >= 0)
        m_context->End(m_gpuTimings[timingSlot].seaStart.Get());
    const auto seaCpuStart = std::chrono::steady_clock::now();
    Render_Sea();
    const double seaCpuMs = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - seaCpuStart).count();
    m_seaCpuMs = m_seaCpuTimingReady ? m_seaCpuMs * 0.9 + seaCpuMs * 0.1 : seaCpuMs;
    m_seaCpuTimingReady = true;
    if (timingSlot >= 0)
        m_context->End(m_gpuTimings[timingSlot].seaEnd.Get());
	Render_Blend();

	Render_UI();



#ifdef _DEBUG
	if (m_gameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_F1))
		m_showColliders = !m_showColliders;

	if (m_showColliders)
	{
		for (auto& pDebugComponent : m_debugComponents)
		{
			pDebugComponent->Render();
		}
	}
	m_debugComponents.clear();

	if (m_gameInstance.lock()->Get_DInput_Manger()->KeyPress(DIK_LCONTROL) == true)
	{
		Render_Debug();
	}
#endif
    End_GpuTiming(timingSlot);
}

void CRenderer::Initialize_GpuTiming()
{
    for (auto& slot : m_gpuTimings)
    {
        D3D11_QUERY_DESC desc = {};
        desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
        if (FAILED(m_device->CreateQuery(&desc, slot.disjoint.GetAddressOf())))
            return;
        desc.Query = D3D11_QUERY_TIMESTAMP;
        if (FAILED(m_device->CreateQuery(&desc, slot.start.GetAddressOf())) ||
            FAILED(m_device->CreateQuery(&desc, slot.end.GetAddressOf())) ||
            FAILED(m_device->CreateQuery(&desc, slot.seaStart.GetAddressOf())) ||
            FAILED(m_device->CreateQuery(&desc, slot.seaEnd.GetAddressOf())))
            return;
    }
    m_gpuTimingEnabled = true;
}

int CRenderer::Begin_GpuTiming()
{
    if (!m_gpuTimingEnabled)
        return -1;

    const unsigned int index = m_gpuTimingIndex;
    m_gpuTimingIndex = (m_gpuTimingIndex + 1) % 4;
    auto& slot = m_gpuTimings[index];
    if (slot.pending)
    {
        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint = {};
        UINT64 start = 0, end = 0, seaStart = 0, seaEnd = 0;
        const HRESULT disjointResult = m_context->GetData(slot.disjoint.Get(), &disjoint, sizeof(disjoint), D3D11_ASYNC_GETDATA_DONOTFLUSH);
        if (disjointResult == S_FALSE)
            return -1;
        if (FAILED(disjointResult))
        {
            m_gpuTimingEnabled = false;
            m_seaGpuTimingReady = false;
            m_gpuTimingReady = false;
            return -1;
        }
        const HRESULT startResult = m_context->GetData(slot.start.Get(), &start, sizeof(start), D3D11_ASYNC_GETDATA_DONOTFLUSH);
        const HRESULT endResult = m_context->GetData(slot.end.Get(), &end, sizeof(end), D3D11_ASYNC_GETDATA_DONOTFLUSH);
        const HRESULT seaStartResult = m_context->GetData(slot.seaStart.Get(), &seaStart, sizeof(seaStart), D3D11_ASYNC_GETDATA_DONOTFLUSH);
        const HRESULT seaEndResult = m_context->GetData(slot.seaEnd.Get(), &seaEnd, sizeof(seaEnd), D3D11_ASYNC_GETDATA_DONOTFLUSH);
        if (FAILED(startResult) || FAILED(endResult) || FAILED(seaStartResult) || FAILED(seaEndResult))
        {
            m_gpuTimingEnabled = false;
            m_seaGpuTimingReady = false;
            m_gpuTimingReady = false;
            return -1;
        }
        if (startResult == S_FALSE || endResult == S_FALSE || seaStartResult == S_FALSE || seaEndResult == S_FALSE)
            return -1;
        if (!disjoint.Disjoint && disjoint.Frequency != 0 && end >= start)
        {
            const double ms = double(end - start) * 1000.0 / double(disjoint.Frequency);
            m_gpuRenderMs = m_gpuTimingReady ? m_gpuRenderMs * 0.9 + ms * 0.1 : ms;
            m_gpuTimingReady = true;
        }
        else
            m_gpuTimingReady = false;
        if (!disjoint.Disjoint && disjoint.Frequency != 0 && seaEnd >= seaStart)
        {
            const double ms = double(seaEnd - seaStart) * 1000.0 / double(disjoint.Frequency);
            m_seaGpuMs = m_seaGpuTimingReady ? m_seaGpuMs * 0.9 + ms * 0.1 : ms;
            m_seaGpuTimingReady = true;
        }
        else
            m_seaGpuTimingReady = false;
        slot.pending = false;
    }
    m_context->Begin(slot.disjoint.Get());
    m_context->End(slot.start.Get());
    return static_cast<int>(index);
}

void CRenderer::End_GpuTiming(int index)
{
    if (index < 0)
        return;
    auto& slot = m_gpuTimings[index];
    m_context->End(slot.end.Get());
    m_context->End(slot.disjoint.Get());
    slot.pending = true;
}

void CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_renderObject[ETOI(ERenderGroup::PRIORITY)])
	{
		if (pRenderObject != nullptr)
			pRenderObject->Render();
	}

	m_renderObject[ETOI(ERenderGroup::PRIORITY)].clear();
}

void CRenderer::Render_NonBlend()
{

	/* Diffuse + Normal */
	if (FAILED(m_gameInstance.lock()->Begin_MRT(TEXT("MRT_GameObjects"))))
		return;




	for (auto& pRenderObject : m_renderObject[ETOI(ERenderGroup::NONBLEND)])
	{
		if (pRenderObject != nullptr)
			pRenderObject->Render();
	}

	m_renderObject[ETOI(ERenderGroup::NONBLEND)].clear();


	m_gameInstance.lock()->End_MRT();
}

void CRenderer::Render_Blend()
{

	auto& vec = m_renderObject[ETOI(ERenderGroup::BLEND)];

	_vector vCamPos = XMLoadFloat4(m_gameInstance.lock()->Get_CamPositon());


	sort(vec.begin(), vec.end(),
		[&](const shared_ptr<CEntity>& a, const shared_ptr<CEntity>& b)
		{
			_vector posA = a->Get_WorldPos();
			_vector posB = b->Get_WorldPos();

			float distA = XMVectorGetX(XMVector3LengthSq(posA - vCamPos));
			float distB = XMVectorGetX(XMVector3LengthSq(posB - vCamPos));

			return distA > distB; 
		});

	
	for (auto& obj : vec)
	{
		if (obj)
			obj->Render();
	}

	vec.clear();
	/*for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::BLEND)])
	{
			pRenderObject->Render();
	}

	m_RenderObject[ETOI(RENDERGROUP::BLEND)].clear();*/
}

void CRenderer::Render_Lights()
{
	//shade
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_context->PSSetShaderResources(0, 16, nullSRV);
	m_context->VSSetShaderResources(0, 16, nullSRV);
	m_context->GSSetShaderResources(0, 16, nullSRV);

	if(FAILED(m_gameInstance.lock()->Begin_MRT(TEXT("MRT_LightAcc"))))
		return;

	m_context->OMSetDepthStencilState(nullptr, 0);

	if (FAILED(m_shader->Bind_Matrix("g_WorldMatrix", &m_worldMatrix)))
		return;


	if (FAILED(m_shader->Bind_Matrix("g_ViewMatrix", &m_viewMatrix)))
		return;


	if (FAILED(m_shader->Bind_Matrix("g_ProjMatrix", &m_projMatrix)))
		return;

	if (FAILED(m_shader->Bind_Matrix("g_ViewMatrixInverse", m_gameInstance.lock()->Get_InverseTransfrom(ED3DTS::VIEW))))
		return;

	if (FAILED(m_shader->Bind_Matrix("g_ProjMatrixInverse", m_gameInstance.lock()->Get_InverseTransfrom(ED3DTS::PROJ))))
		return;


	if (FAILED(m_gameInstance.lock()->Bind_RT_ShaderResource(m_shader, "g_NormalTexture", TEXT("Target_Normal"))))
		return;

	if (FAILED(m_gameInstance.lock()->Bind_RT_ShaderResource(m_shader, "g_DepthTexture", TEXT("Target_Depth"))))
		return;

	if (FAILED(m_shader->Bind_RawValue("g_vCamPosition", m_gameInstance.lock()->Get_CamPositon(), sizeof(_float4))))
		return;
	auto fFar = m_gameInstance.lock()->Get_Far();
	if (FAILED(m_shader->Bind_RawValue("g_Far",& fFar, sizeof(_float))))
		return;

	m_VIBuffer->Bind_Resources();

	if (FAILED(m_gameInstance.lock()->Render_Lights(m_shader, m_VIBuffer)))
		return;

	m_gameInstance.lock()->End_MRT();

}

void CRenderer::Render_Combined()
{



	//ID3D11ShaderResourceView* nullSRV[16] = {};
	//m_pContext->PSSetShaderResources(0, 16, nullSRV);

	if (FAILED(m_shader->Bind_Matrix("g_WorldMatrix", &m_worldMatrix)))
		return;


	if (FAILED(m_shader->Bind_Matrix("g_ViewMatrix", &m_viewMatrix)))
		return;


	if (FAILED(m_shader->Bind_Matrix("g_ProjMatrix", &m_projMatrix)))
		return;
	if (FAILED(m_shader->Bind_Matrix("g_ViewMatrixInverse", m_gameInstance.lock()->Get_InverseTransfrom(ED3DTS::VIEW))))
		return;

	if (FAILED(m_shader->Bind_Matrix("g_ProjMatrixInverse", m_gameInstance.lock()->Get_InverseTransfrom(ED3DTS::PROJ))))
		return;
	if (FAILED(m_shader->Bind_RawValue("g_vCamPosition", m_gameInstance.lock()->Get_CamPositon(), sizeof(_float4))))
		return;
	auto fFar = m_gameInstance.lock()->Get_Far();
	if (FAILED(m_shader->Bind_RawValue("g_Far", &fFar, sizeof(_float))))
		return;
	//float tod01 = m_pGameInstance.lock()->Get_TOD01();
	//
	//if (FAILED(m_pShader->Bind_RawValue("g_fTOD01", &tod01, sizeof(float))))
	//	return E_FAIL;
	//auto diffuse = CSky_Controller::GetInstance()->Get_Diffuse();
	auto diffuse = m_gameInstance.lock()->Get_SkyColor();
	//
	if (FAILED(m_shader->Bind_RawValue("g_SkyColor", &diffuse, sizeof(_float3))))
		return ;

	if (FAILED(m_gameInstance.lock()->Bind_RT_ShaderResource(m_shader, "g_DepthTexture", TEXT("Target_Depth"))))
		return;
	if (FAILED(m_gameInstance.lock()->Bind_RT_ShaderResource(m_shader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
		return;
	if (FAILED(m_gameInstance.lock()->Bind_RT_ShaderResource(m_shader, "g_ShadeTexture", TEXT("Target_Shade"))))
		return;
	if (FAILED(m_gameInstance.lock()->Bind_RT_ShaderResource(m_shader, "g_EmissiveTexture", TEXT("Target_Emissive"))))
		return;
	//if (FAILED(m_pGameInstance.lock()->Bind_RT_ShaderResource(m_pShader, "g_BloomTexture", TEXT("Target_Blur2"))))
	//	return;

	m_shader->Begin(ETOI(EDeferred::COMBINED));

	m_VIBuffer->Bind_Resources();
	m_VIBuffer->Render();


}
void CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_renderObject[ETOI(ERenderGroup::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		
	}

	m_renderObject[ETOI(ERenderGroup::NONLIGHT)].clear();
}

void CRenderer::Render_Sea()
{

	for (auto& pRenderObject : m_renderObject[ETOI(ERenderGroup::SEA)])
	{
		if (pRenderObject != nullptr)
			pRenderObject->Render();
	}

	m_renderObject[ETOI(ERenderGroup::SEA)].clear();
}

void CRenderer::Render_UI()
{
	
	for (auto& pRenderObject : m_renderObject[ETOI(ERenderGroup::UI)])
	{
		if(pRenderObject != nullptr)
			pRenderObject->Render();
	}


	m_renderObject[ETOI(ERenderGroup::UI)].clear();
}

void CRenderer::RenderBloom()
{

	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_context->PSSetShaderResources(0, 16, nullSRV);
	m_context->VSSetShaderResources(0, 16, nullSRV);
	m_context->GSSetShaderResources(0, 16, nullSRV);

	if (FAILED(m_gameInstance.lock()->Begin_MRT(TEXT("MRT_Blur1"),false)))
		return;


	if (FAILED(m_gameInstance.lock()->Bind_RT_ShaderResource(m_shader, "g_InputTexture", TEXT("Target_Emissive"))))
		return;


	_float4x4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());

	//m_pShader->Bind_Matrix("g_WorldMatrix", &identity);
	//m_pShader->Bind_Matrix("g_ViewMatrix", &identity);
	//m_pShader->Bind_Matrix("g_ProjMatrix", &identity);
	if (FAILED(m_shader->Bind_Matrix("g_WorldMatrix", &m_worldMatrix)))
		return;


	if (FAILED(m_shader->Bind_Matrix("g_ViewMatrix", &m_viewMatrix)))
		return;


	if (FAILED(m_shader->Bind_Matrix("g_ProjMatrix", &m_projMatrix)))

	if (FAILED(m_shader->Bind_RawValue("g_WinSize", &m_winSize, sizeof(_float2))))
		return;

	m_shader->Begin(4);

	m_VIBuffer->Bind_Resources();


	m_VIBuffer->Render();

	//if (FAILED(m_pGameInstance.lock()->Render_Lights(m_pShader, m_pVIBuffer)))
	//	return;
	m_gameInstance.lock()->End_MRT();
	//////////////////////////////////////////////////////
	ID3D11ShaderResourceView* nullSRV2[16] = {};
	m_context->PSSetShaderResources(0, 16, nullSRV2);
	m_context->VSSetShaderResources(0, 16, nullSRV);
	m_context->GSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_gameInstance.lock()->Begin_MRT(TEXT("MRT_Blur2"),false)))
		return;
	
	if (FAILED(m_gameInstance.lock()->Bind_RT_ShaderResource(m_shader, "g_InputTexture", TEXT("Target_Blur1"))))
		return;

      identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());

	//m_pShader->Bind_Matrix("g_WorldMatrix", &identity);
	//m_pShader->Bind_Matrix("g_ViewMatrix", &identity);
	//m_pShader->Bind_Matrix("g_ProjMatrix", &identity);
	if (FAILED(m_shader->Bind_Matrix("g_WorldMatrix", &m_worldMatrix)))
		return;


	if (FAILED(m_shader->Bind_Matrix("g_ViewMatrix", &m_viewMatrix)))
		return;


	if (FAILED(m_shader->Bind_Matrix("g_ProjMatrix", &m_projMatrix)))


	if (FAILED(m_shader->Bind_RawValue("g_WinSize", &m_winSize, sizeof(_float2))))
		return;

	m_shader->Begin(5);

	m_VIBuffer->Bind_Resources();


	m_VIBuffer->Render();

	m_gameInstance.lock()->End_MRT();
}
#ifdef _DEBUG
void CRenderer::Render_Debug()
{



	if (FAILED(m_shader->Bind_Matrix("g_ViewMatrix", &m_viewMatrix)))
		return;

	if (FAILED(m_shader->Bind_Matrix("g_ProjMatrix", &m_projMatrix)))
		return;

	if (FAILED(m_gameInstance.lock()->Render_RT_Debug(m_VIBuffer, m_shader, TEXT("MRT_GameObjects"))))
		return;

	if (FAILED(m_gameInstance.lock()->Render_RT_Debug(m_VIBuffer, m_shader, TEXT("MRT_LightAcc"))))
		return;
//	if (FAILED(m_pGameInstance.lock()->Render_RT_Debug(m_pVIBuffer, m_pShader, TEXT("MRT_Blur2"))))
//		return;
//	if (FAILED(m_pGameInstance.lock()->Render_RT_Debug(m_pVIBuffer, m_pShader, TEXT("MRT_Blur1"))))
//		return;
}
#endif

#ifdef _DEBUG
void CRenderer::Add_DebugenderGroup(shared_ptr<CComponent> pDebugComponent)
{
	m_debugComponents.push_back(pDebugComponent);

}
#endif

unique_ptr<CRenderer> CRenderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> m_context)
{
	unique_ptr<CRenderer> pInstance (new CRenderer(pDevice, m_context));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
	}
	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();


	for(int i =0; i < ETOI(ERenderGroup::END); i++)
	{
		//for (auto& it : m_RenderObject[i])
		//	Safe_Release(it);
		m_renderObject[i].clear();
	}

}
