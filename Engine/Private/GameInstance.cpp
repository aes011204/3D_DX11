#include "GameInstance.h"

#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Prototype_Manager.h"
#include "GameObject.h"
#include "Renderer.h"
#include "EventBus.h"
#include "UI_Manager.h"
#include "DInput_Manager.h"
#include "Data_Manager.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "Camera_Manager.h"
#include "Picking_Manager.h"
#include "Font_Manager.h"
#include "Collison_Manager.h"
#include "TimeOfDay.h"
#include "Target_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{
}
HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ComPtr<ID3D11Device>& ppDevice, _Out_ ComPtr<ID3D11DeviceContext>& ppContext)
{
	srand((unsigned int)time(nullptr));
	// 그래픽디바이스를 초기화 한다
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iViewportWidth, EngineDesc.iViewportHeight, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	// 타이머 매니져를 생성해 둔다
	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;
	// 레벨 매니져를 생성해 둔다
	m_pLevel_Manager = CLevel_Manager::Create(EngineDesc.iEditorLevel, EngineDesc.iMaxLevelNum);
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;
	// 프로토 타입매니져를 생성해 둔다
	m_pProto_Manager = CPrototype_Manager::Create(EngineDesc.iMaxLevelNum);
	if (nullptr == m_pProto_Manager)
		return E_FAIL;
	Push_ManagerClass(L"Proto_Manager", m_pProto_Manager.get());

	// 오브젝트 매니져를 생성해 둔다
	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iMaxLevelNum);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pCamera_Manager = CCamera_Manager::Create(EngineDesc.iMaxLevelNum);
if (nullptr == m_pCamera_Manager)
	return E_FAIL;


	// 렌더러 전에
	m_pTarget_Manager = CTarget_Manager::Create(ppDevice.Get(), ppContext.Get());
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	// 렌더러를 생성해 둔다
	m_Renderer = CRenderer::Create(ppDevice, ppContext);
	if (nullptr == m_Renderer)
		return E_FAIL;

	// uimanager 생성해 둔다
	m_UI_Manager = CUI_Manager::Create(EngineDesc.iViewportWidth, EngineDesc.iViewportHeight);
	if (nullptr == m_UI_Manager)
		return E_FAIL;
	Push_ManagerClass(L"UI_Manager", m_UI_Manager.get());
	
	// dInputManager 생성해 둔다
	m_pDInput_Manager = CDInput_Manager::Create(EngineDesc.hInst, EngineDesc.hWnd);
	if (nullptr == m_pDInput_Manager)
		return E_FAIL;
	Push_ManagerClass(L"DInput_Manager", m_pDInput_Manager.get());

	// CEventBus 생성해 둔다
	m_pEventBus = CEventBus::Create();
	if (nullptr == m_pEventBus)
		return E_FAIL;

	// CData_Manager 생성해 둔다
	m_pData_Manager = CData_Manager::Create(EngineDesc.iEditorLevel);
	if (nullptr == m_pEventBus)
		return E_FAIL;

	// m_pPipeLine 생성해 둔다
	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	// m_Light_Manager 생성해 둔다
	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;
	Push_ManagerClass(L"Light_Manager", m_pLight_Manager.get());


	//m_pCamera_Manager = CCamera_Manager::Create(EngineDesc.iMaxLevelNum);
	//if (nullptr == m_pCamera_Manager)
	//	return E_FAIL;

	m_pPicking_Manager = CPicking_Manager::Create(ppDevice.Get(), ppContext.Get());
	if (nullptr == m_pPicking_Manager)
		return E_FAIL;


	m_pFont_Manager = CFont_Manager::Create(ppDevice.Get(), ppContext.Get());
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pTimeOfDay = CTimeOfDay::Create();
	if (nullptr == m_pTimeOfDay)
		return E_FAIL;
	Push_ManagerClass(L"TimeOfDay", m_pTimeOfDay.get());

	m_pCollision_Manager = CCollision_Manager::Create(ppDevice.Get(), ppContext.Get());
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;



	return S_OK;
}

void CGameInstance::SetImguiContext(ImGuiContext* imgContext)
{
	ImGui::SetCurrentContext(imgContext);
}

//ImGuiContext* CGameInstance::GetContext()
//{
//	return m_pImgui_Manager->GetContext();
//}
void CGameInstance::Update_Engine(float fTimeDelta)
{
	m_pDInput_Manager->Update_InputDev();

	m_pTimeOfDay->Update(fTimeDelta);

	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pCamera_Manager->Apply_To_PipeLine();

	m_pPipeLine->Update();

	m_pPicking_Manager->Update();

	m_pObject_Manager->Update(fTimeDelta);
	m_UI_Manager->Update(fTimeDelta);

	m_pCollision_Manager->Update();

	m_pObject_Manager->Late_Update(fTimeDelta);
	m_UI_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);


}

void CGameInstance::Draw()
{
	m_Renderer->Draw();

	m_pLevel_Manager->Render();

	m_UI_Manager->Render();

}

void CGameInstance::Clear_Resources(_uint iLevelIndex)
{

	if (FAILED(m_pObject_Manager->Clear_Layers(iLevelIndex)))
	{
		MSG_BOX("failed to Clear Resourse");
		return;
	}
	if(FAILED(m_pProto_Manager->Clear_Prototype(iLevelIndex)))
	{
		MSG_BOX("failed to Clear Resourse");
		return;
	}

	m_UI_Manager->Detach_All();

	//m_pLight_Manager->clear_light();

	if (FAILED(m_pCamera_Manager->Clear_Camera(iLevelIndex)))
	{
		MSG_BOX("failed to Clear Resourse");
		return;
	}

	return;
}

_float CGameInstance::Random(_float fMin, _float fMax)
{
	return fMin + (static_cast<_float>(rand()) / RAND_MAX) * (fMax - fMin);
}


HRESULT CGameInstance::Resize(_uint g_RsizeWidth, _uint g_RsizeHeight)
{

	return m_pGraphic_Device->Resize(g_RsizeWidth, g_RsizeHeight);;
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimeTag)
{

	if (FAILED(m_pTimer_Manager->Add_Timer(strTimeTag)))
		return E_FAIL;

	return S_OK;
}

_float CGameInstance::Compute_TimeDelta(const _wstring& strTimeTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimeTag);
}

HRESULT CGameInstance::Change_Level(_uint iNewLevelIndex, shared_ptr<CLevel> pNewLevel)
{


	return m_pLevel_Manager->Change_Level(iNewLevelIndex, pNewLevel);


}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, shared_ptr<CBase> pPrototype)
{
	return m_pProto_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

shared_ptr<CBase> CGameInstance::Clone_Prototype(PROTOTYPE ePrototy, _uint iLevelIndex, const _wstring& strPrototypeTag,
	void* pArg)
{

	shared_ptr<CBase> tmp = m_pProto_Manager->Clone_Prototype(ePrototy, iLevelIndex, strPrototypeTag, pArg);
	if (tmp == nullptr)
		return nullptr;
	tmp->SetDefaultNameFromThisType();
	return  tmp;
}

//shared_ptr<CBase> CGameInstance::Get_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
//{
//	return m_pProto_Manager->Find_Prototype(iLevelIndex, strPrototypeTag);
//}


shared_ptr<CGameObject> CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
                                                      _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}

shared_ptr<CGameObject> CGameInstance::Get_GameObject(_uint iLayerLevelIndex,const _wstring& strLayerTag, _uint GObjIndex)
{
	return m_pObject_Manager->Get_GameObject(iLayerLevelIndex,strLayerTag, GObjIndex);
}


void CGameInstance::Add_RenderGroup(RENDERGROUP eRenderGroup, shared_ptr<CEntity> pNTT)
{
	m_Renderer->Add_RenderGroup(eRenderGroup, pNTT);
}


HRESULT CGameInstance::Clear_Buffers(const _float4* pClearColor)
{
	if (FAILED(m_pGraphic_Device->Clear_BackBuffer_View(pClearColor)))
		return E_FAIL;
	if (FAILED(m_pGraphic_Device->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}
HRESULT CGameInstance::Bind_BackBufferRenderTarget(HWND hwnd)
{
	return m_pGraphic_Device->Bind_BackBufferRenderTarget(hwnd);
}
HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();;
}

const map<const _wstring, shared_ptr<CLayer>>& CGameInstance::Get_GameObjects(_uint levelIndex) const
{
	return m_pObject_Manager->Get_GameObjects(levelIndex);
}


_uint CGameInstance::Get_Current_LevelIdx()
{
	return m_pLevel_Manager->Get_CurLevelIdx();
}

CUI_Manager* CGameInstance::Get_UI_Manager() const
{
	{ return m_UI_Manager.get(); }
}
CGraphic_Device* CGameInstance::Get_GraphicDevice() const 
{ 
	return m_pGraphic_Device.get(); 
}

const map<_wstring, CBase*>& CGameInstance::Get_ManagerClass() const
{
	return m_ManagerForImgui;
}
void CGameInstance::Push_ManagerClass(_wstring strKey, CBase* ManagerClass)
{
	m_ManagerForImgui.emplace(strKey, ManagerClass);
}

void CGameInstance::UI_Push(UI_LAYER layer, wstring name, _bool isOnActive, void* pArg)
{
	m_UI_Manager->Push(layer, name, isOnActive, pArg);
}
void CGameInstance::UI_Pop(UI_LAYER layer, wstring type)
{
	m_UI_Manager->Pop(layer, type);

}
void CGameInstance::UI_Detach_All()
{
	m_UI_Manager->Detach_All();
}
void CGameInstance::UI_InsertToPool(wstring UIType, shared_ptr<CUI> UI)
{
	m_UI_Manager->InsertToPool(UIType, UI);
}
Rect CGameInstance::Get_WinSize()
{
	return m_UI_Manager->Get_WinSize();
}

shared_ptr<CUI> CGameInstance::Find_UI_InCurLevel(UI_LAYER layer, wstring type)
{
	return m_UI_Manager->Find_UI_InCurLevel(layer, type);
}

CEventBus* CGameInstance::Get_EventBus()
{
	return m_pEventBus.get(); ;
}
//const vector<shared_ptr<CUI>>& CGameInstance::GetUIList(UI_LAYER layer)
//{
//	return m_UI_Manager->GetUIList(layer);
//}
//
//const unordered_map<wstring, shared_ptr<CUI>>& CGameInstance::GetUIPool()
//{
//	return m_UI_Manager->GetUIPool();

//}

void CGameInstance::Set_MousePos(float x, float y)
{
	m_pDInput_Manager->Set_MousePos(x, y);
}

_float2 CGameInstance::Get_MousePos()
{
	return m_pDInput_Manager->Get_MousePos();

}

bool CGameInstance::ClearMap(SAVETYPE eDATATYPE)
{
	return m_pData_Manager->ClearMap(eDATATYPE);
}

bool CGameInstance::Load(SAVETYPE eDATATYPE, const string& fileName)
{
	return m_pData_Manager->Load(eDATATYPE, fileName);
}

bool CGameInstance::Save(SAVETYPE eDATATYPE, const string& fileName)
{
	return m_pData_Manager->Save(eDATATYPE, fileName);

}

const _float4x4* CGameInstance::Get_Transform(D3DTS eTransformState) const
{
	return m_pPipeLine->Get_Transfrom(eTransformState);
}

const _float4x4* CGameInstance::Get_InverseTransfrom(D3DTS eTransformState) const
{
	return m_pPipeLine->Get_InverseTransfrom(eTransformState);
}

const _float4* CGameInstance::Get_CamPositon() const
{
	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_Transform(D3DTS eTransformState, _fmatrix TransformStateMatrix)
{
	return m_pPipeLine->Set_Transform(eTransformState, TransformStateMatrix);
}

HRESULT CGameInstance::Bind_CamPosition(shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return m_pPipeLine->Bind_CamPosition(pShader, pConstantName);
}

HRESULT CGameInstance::Bind_TransformMatrix(D3DTS eTransformState, shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return m_pPipeLine->Bind_TransformMatrix(eTransformState, pShader, pConstantName);
}

HRESULT CGameInstance::Bind_TransformMatrix_Inverse(D3DTS eTransformState, shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return m_pPipeLine->Bind_TransformMatrix_Inverse(eTransformState, pShader, pConstantName);
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

shared_ptr<CLight> CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

void CGameInstance::Set_LightDesc(_uint iIndex, LIGHT_DESC Desc)
{
	m_pLight_Manager->Set_LightDesc(iIndex, Desc);
}

void CGameInstance::Clear_Light()
{
	return m_pLight_Manager->clear_light();
}

_bool CGameInstance::Compute_HeightOnTerrain(_fvector pPos, _float* Out, _wstring layerTag, _uint TerrainIndex)
{


	return m_pPicking_Manager-> Compute_HeightOnTerrain(  layerTag, TerrainIndex, pPos, Out);
}

_bool CGameInstance::Picking_Terrain(_wstring layerTag, _uint TerrainIndex, _float3* Out)
{

	return m_pPicking_Manager->Picking_Terrain(layerTag, TerrainIndex, Out);
}

HRESULT CGameInstance::Add_Camera(_uint camLevel, _wstring key, shared_ptr<CCamera> cam)
{
	return m_pCamera_Manager->Add_Camera(camLevel, key, cam);
}




_bool CGameInstance::Change_Camera(_wstring key)
{
	return m_pCamera_Manager->Change_Camera(key);
}

void CGameInstance::CAM_Manger_OnGui()
{
	m_pCamera_Manager->OnGui();
}

float CGameInstance::Get_Far()
{
	return m_pCamera_Manager->Get_Far();
}

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

void CGameInstance::Font_Draw(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor)
{
	m_pFont_Manager->Draw(strFontTag, pText, vPosition, vColor);
}

_float2 CGameInstance::Measure_String(const _wstring& strFontTag, const _tchar* pText)
{
	return m_pFont_Manager->Measure_String(strFontTag, pText);

}

void CGameInstance::ComputeTime(_uint& iDay, _float& fHour, _float& fMinute, _float& fSecond)
{
	m_pTimeOfDay->ComputeTime(iDay, fHour, fMinute, fSecond);
}

_float CGameInstance::Get_TOD01()
{
	return m_pTimeOfDay->Get_TOD01();
}

void CGameInstance::Add_Collider(shared_ptr<CCollider> coll)
{
	m_pCollision_Manager->Add_Collider(coll);
}

HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Bind_RT_ShaderResource(shared_ptr<CShader> pShader, const _char* pConstantName, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Bind_ShaderResource(pShader, pConstantName, strTargetTag);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, bool useDepth)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, useDepth);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_RT_Debug(shared_ptr<CVIBuffer_Rect> pVIBuffer, shared_ptr < CShader> pShader, const _wstring& strMRTTag)
{
	return m_pTarget_Manager->Render(pVIBuffer, pShader, strMRTTag);
}
#endif

HRESULT CGameInstance::Render_Lights(shared_ptr<CShader> pShader, shared_ptr< CVIBuffer_Rect> pVIBuffer)
{
	return m_pLight_Manager->Render_Light(pShader, pVIBuffer);
}
#ifdef _DEBUG
void CGameInstance::Add_DebugenderGroup(shared_ptr<CComponent> pDebugComponent)
{
	m_Renderer->Add_DebugenderGroup(pDebugComponent);
}
#endif


void CGameInstance::Free()
{
	__super::Free();

	m_ManagerForImgui.clear();

	
	m_pPicking_Manager.reset();
	m_pCamera_Manager.reset();
	m_pFont_Manager.reset();
	m_pTimeOfDay.reset();
	m_pCollision_Manager.reset();
	m_pTarget_Manager.reset();
	m_pObject_Manager.reset();
	m_pProto_Manager.reset();
	m_pLevel_Manager.reset();
	m_pTimer_Manager.reset();
	m_pEventBus.reset();
	m_Renderer.reset();
	m_UI_Manager.reset();
	m_pData_Manager.reset();
	m_pPipeLine.reset();
	m_pLight_Manager.reset();


	m_pGraphic_Device.reset();
	


//	m_pImgui_Manager.reset();

}
