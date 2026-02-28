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

//#include "../../EditorTool/Public/ImguiManager.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{
}
HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ComPtr<ID3D11Device>& ppDevice, _Out_ ComPtr<ID3D11DeviceContext>& ppContext)
{
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

	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pPipeLine->Update();

	m_pObject_Manager->Update(fTimeDelta);
	m_UI_Manager->Update(fTimeDelta);

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

	return;
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

//shared_ptr<CBase> CGameInstance::Clone_Prototype(shared_ptr<CBase> pPrototype, void* pArg)
//{
//	shared_ptr<CBase> tmp = m_pProto_Manager->Clone_Prototype(pPrototype,pArg);
//	if (tmp == nullptr)
//		return nullptr;
//	tmp->SetDefaultNameFromThisType();
//	return  tmp;
//
//}

shared_ptr<CGameObject> CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
	_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}


//HRESULT CGameInstance::Add_GameObject(shared_ptr<CBase> pClonedInst,
//	_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
//{
//
//	//shared_ptr<CGameObject>pGameObject = dynamic_pointer_cast<CGameObject>(Clone_Prototype(pClonedInst, pArg));
//	//if (nullptr == pGameObject)
//	//	return E_FAIL;
//	return m_pObject_Manager->Add_GameObject(pClonedInst, iLayerLevelIndex, strLayerTag, pArg);
//}
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

map<const _wstring, shared_ptr<CLayer>> CGameInstance::Get_GameObjects(_uint levelIndex)
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

void CGameInstance::UI_Push(UI_LAYER layer, wstring name, void* pArg)
{
	m_UI_Manager->Push(layer, name, pArg);
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

const _float4x4* CGameInstance::Get_Transfrom(D3DTS eTransformState) const
{
	return m_pPipeLine->Get_Transfrom(eTransformState);
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

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}



void CGameInstance::Free()
{
	__super::Free();

	m_ManagerForImgui.clear();
	

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
