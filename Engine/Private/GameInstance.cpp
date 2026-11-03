#include "GameInstance.h"
#include <chrono>

namespace
{
    using PerfClock = std::chrono::steady_clock;
    void RecordMs(double& average, PerfClock::time_point start)
    {
        const double ms = std::chrono::duration<double, std::milli>(PerfClock::now() - start).count();
        average = average == 0.0 ? ms : average * 0.9 + ms * 0.1;
    }
}

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
#include "SoundManager.h"
#include "TimeOfDay.h"
#include "Target_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{
}
HRESULT CGameInstance::Initialize_Engine(const EngineDesc& engineDesc, _Out_ ComPtr<ID3D11Device>& ppDevice, _Out_ ComPtr<ID3D11DeviceContext>& ppContext)
{
	srand((unsigned int)time(nullptr));
	// 그래픽디바이스를 초기화 한다
	m_graphicDevice = CGraphic_Device::Create(engineDesc.hWnd, engineDesc.winMode, engineDesc.viewportWidth, engineDesc.viewportHeight, ppDevice, ppContext);
	if (nullptr == m_graphicDevice)
		return E_FAIL;
	// 타이머 매니져를 생성해 둔다
	m_timerManager = CTimer_Manager::Create();
	if (nullptr == m_timerManager)
		return E_FAIL;
	// 레벨 매니져를 생성해 둔다
	m_levelManager = CLevel_Manager::Create(engineDesc.editorLevel, engineDesc.maxLevelNum);
	if (nullptr == m_levelManager)
		return E_FAIL;
	// 프로토 타입매니져를 생성해 둔다
	m_protoManager = CPrototype_Manager::Create(engineDesc.maxLevelNum);
	if (nullptr == m_protoManager)
		return E_FAIL;
	Push_ManagerClass(L"Proto_Manager", m_protoManager.get());

	// 오브젝트 매니져를 생성해 둔다
	m_objectManager = CObject_Manager::Create(engineDesc.maxLevelNum);
	if (nullptr == m_objectManager)
		return E_FAIL;

	m_cameraManager = CCamera_Manager::Create(engineDesc.maxLevelNum);
	if (nullptr == m_cameraManager)
		return E_FAIL;
    Push_ManagerClass(L"Camera_Manager", m_cameraManager.get());


	// 렌더러 전에
	m_targetManager = CTarget_Manager::Create(ppDevice.Get(), ppContext.Get());
	if (nullptr == m_targetManager)
		return E_FAIL;

	// 렌더러를 생성해 둔다
	m_renderer = CRenderer::Create(ppDevice, ppContext);
	if (nullptr == m_renderer)
		return E_FAIL;

	// uimanager 생성해 둔다
	m_UIManager = CUI_Manager::Create(engineDesc.viewportWidth, engineDesc.viewportHeight);
	if (nullptr == m_UIManager)
		return E_FAIL;
	Push_ManagerClass(L"UI_Manager", m_UIManager.get());

	// dInputManager 생성해 둔다
	m_DInputManager = CDInput_Manager::Create(engineDesc.hInst, engineDesc.hWnd);
	if (nullptr == m_DInputManager)
		return E_FAIL;
	Push_ManagerClass(L"DInput_Manager", m_DInputManager.get());

	// CEventBus 생성해 둔다
	m_eventBus = CEventBus::Create();
	if (nullptr == m_eventBus)
		return E_FAIL;

	// CData_Manager 생성해 둔다
	m_dataManager = CData_Manager::Create(engineDesc.editorLevel);
	if (nullptr == m_eventBus)
		return E_FAIL;

	// m_pPipeLine 생성해 둔다
	m_pipeLine = CPipeLine::Create();
	if (nullptr == m_pipeLine)
		return E_FAIL;

	// m_Light_Manager 생성해 둔다
	m_lightManager = CLight_Manager::Create();
	if (nullptr == m_lightManager)
		return E_FAIL;
	Push_ManagerClass(L"Light_Manager", m_lightManager.get());


	//m_pCamera_Manager = CCamera_Manager::Create(EngineDesc.iMaxLevelNum);
	//if (nullptr == m_pCamera_Manager)
	//	return E_FAIL;

	m_pickingManager = CPicking_Manager::Create(ppDevice.Get(), ppContext.Get());
	if (nullptr == m_pickingManager)
		return E_FAIL;


	m_fontManager = CFont_Manager::Create(ppDevice.Get(), ppContext.Get());
	if (nullptr == m_fontManager)
		return E_FAIL;

	m_timeOfDay = CTimeOfDay::Create();
	if (nullptr == m_timeOfDay)
		return E_FAIL;
	Push_ManagerClass(L"TimeOfDay", m_timeOfDay.get());

	m_collisionManager = CCollision_Manager::Create(ppDevice.Get(), ppContext.Get());
	if (nullptr == m_collisionManager)
		return E_FAIL;

	m_soundManager = CSoundManager::Create();

	if (nullptr == m_collisionManager)
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
void CGameInstance::Update_Engine(float timeDelta)
{
    const auto updateStart = PerfClock::now();
	m_DInputManager->Update_InputDev();

	m_timeOfDay->Update(timeDelta);

	m_objectManager->Priority_Update(timeDelta);

	m_cameraManager->Apply_To_PipeLine();

	m_pipeLine->Update();

	m_pickingManager->Update();

	m_objectManager->Update(timeDelta);
	m_UIManager->Update(timeDelta);

	m_collisionManager->Update();

	m_cameraManager->Update_AfterTarget(timeDelta);
	m_cameraManager->Apply_To_PipeLine();
	m_pipeLine->Update();
	m_pickingManager->Update();

	m_objectManager->Late_Update(timeDelta);

	m_UIManager->Late_Update(timeDelta);

	m_levelManager->Update(timeDelta);
	m_soundManager->Update_Audio(timeDelta);
    RecordMs(m_frameTimings.updateMs, updateStart);

}

void CGameInstance::Draw()
{
    const auto prepareStart = PerfClock::now();
	m_UIManager->Prepare_Render();
    RecordMs(m_frameTimings.uiPrepareMs, prepareStart);
    const auto renderStart = PerfClock::now();
	m_renderer->Draw();

	m_levelManager->Render();

	m_UIManager->Render();
    RecordMs(m_frameTimings.renderCpuMs, renderStart);
    m_frameTimings.renderGpuMs = m_renderer->Get_GpuRenderMs();
    m_frameTimings.gpuReady = m_renderer->Has_GpuTiming();
    m_frameTimings.seaCpuMs = m_renderer->Get_SeaCpuMs();
    m_frameTimings.seaGpuMs = m_renderer->Get_SeaGpuMs();
    m_frameTimings.seaGpuReady = m_renderer->Has_SeaGpuTiming();

}

void CGameInstance::Clear_Resources(_uint levelIndex)
{

	if (FAILED(m_objectManager->Clear_Layers(levelIndex)))
	{
		MSG_BOX("failed to Clear Resourse");
		return;
	}
	if (FAILED(m_protoManager->Clear_Prototype(levelIndex)))
	{
		MSG_BOX("failed to Clear Resourse");
		return;
	}

	m_UIManager->Detach_All();

	//m_pLight_Manager->clear_light();

	if (FAILED(m_cameraManager->Clear_Camera(levelIndex)))
	{
		MSG_BOX("failed to Clear Resourse");
		return;
	}

	return;
}

_float CGameInstance::Random(_float minValue, _float maxValue)
{
	return minValue + (static_cast<_float>(rand()) / RAND_MAX) * (maxValue - minValue);
}

_int CGameInstance::RandomInt(_int minValue, _int maxValue)
{
	if (minValue > maxValue)
		swap(minValue, maxValue);

	return minValue + rand() % (maxValue - minValue + 1);
}
HRESULT CGameInstance::Resize(_uint g_RsizeWidth, _uint g_RsizeHeight)
{

	return m_graphicDevice->Resize(g_RsizeWidth, g_RsizeHeight);;
}

HRESULT CGameInstance::Add_Timer(const _wstring& timeTag)
{

	if (FAILED(m_timerManager->Add_Timer(timeTag)))
		return E_FAIL;

	return S_OK;
}

_float CGameInstance::Compute_TimeDelta(const _wstring& timeTag)
{
	return m_timerManager->Compute_TimeDelta(timeTag);
}

HRESULT CGameInstance::Change_Level(_uint newLevelIndex, shared_ptr<CLevel> newLevel)
{


	return m_levelManager->Change_Level(newLevelIndex, newLevel);


}

HRESULT CGameInstance::Add_Prototype(_uint levelIndex, const _wstring& prototypeTag, shared_ptr<CBase> prototype)
{
	return m_protoManager->Add_Prototype(levelIndex, prototypeTag, prototype);
}

shared_ptr<CBase> CGameInstance::Clone_Prototype(EPrototype prototy, _uint levelIndex, const _wstring& prototypeTag,
	void* arg)
{

	shared_ptr<CBase> tmp = m_protoManager->Clone_Prototype(prototy, levelIndex, prototypeTag, arg);
	if (tmp == nullptr)
		return nullptr;
	tmp->SetDefaultNameFromThisType();
	return  tmp;
}

//shared_ptr<CBase> CGameInstance::Get_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
//{
//	return m_pProto_Manager->Find_Prototype(iLevelIndex, strPrototypeTag);
//}


shared_ptr<CGameObject> CGameInstance::Add_GameObject(_uint prototypeLevelIndex, const _wstring& prototypeTag,
	_uint layerLevelIndex, const _wstring& layerTag, void* arg)
{
	return m_objectManager->Add_GameObject(prototypeLevelIndex, prototypeTag, layerLevelIndex, layerTag, arg);
}

shared_ptr<CGameObject> CGameInstance::Get_GameObject(_uint layerLevelIndex, const _wstring& layerTag, _uint gameObjIndex)
{
	return m_objectManager->Get_GameObject(layerLevelIndex, layerTag, gameObjIndex);
}


void CGameInstance::Add_RenderGroup(ERenderGroup renderGroup, shared_ptr<CEntity> NTT)
{
	m_renderer->Add_RenderGroup(renderGroup, NTT);
}


HRESULT CGameInstance::Clear_Buffers(const _float4* clearColor)
{
	if (FAILED(m_graphicDevice->Clear_BackBuffer_View(clearColor)))
		return E_FAIL;
	if (FAILED(m_graphicDevice->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}
HRESULT CGameInstance::Bind_BackBufferRenderTarget(HWND hwnd)
{
	return m_graphicDevice->Bind_BackBufferRenderTarget(hwnd);
}
HRESULT CGameInstance::Present()
{
    const auto presentStart = PerfClock::now();
    const HRESULT result = m_graphicDevice->Present();
    RecordMs(m_frameTimings.presentMs, presentStart);
    return result;
}

const map<const _wstring, shared_ptr<CLayer>>& CGameInstance::Get_GameObjects(_uint levelIndex) const
{
	return m_objectManager->Get_GameObjects(levelIndex);
}


_uint CGameInstance::Get_Current_LevelIdx()
{
	return m_levelManager->Get_CurLevelIdx();
}

CUI_Manager* CGameInstance::Get_UI_Manager() const
{
	{ return m_UIManager.get(); }
}
CGraphic_Device* CGameInstance::Get_GraphicDevice() const
{
	return m_graphicDevice.get();
}

const map<_wstring, CBase*>& CGameInstance::Get_ManagerClass() const
{
	return m_managerForImgui;
}
void CGameInstance::Push_ManagerClass(_wstring key, CBase* managerClass)
{
	m_managerForImgui.emplace(key, managerClass);
}

void CGameInstance::UI_Push(EUILayer layer, wstring name, _bool isOnActive, void* arg)
{
	m_UIManager->Push(layer, name, isOnActive, arg);
}
void CGameInstance::UI_Pop(EUILayer layer, wstring type)
{
	m_UIManager->Pop(layer, type);

}
void CGameInstance::UI_Detach_All()
{
	m_UIManager->Detach_All();
}
void CGameInstance::UI_InsertToPool(wstring UIType, shared_ptr<CUI> UI)
{
	m_UIManager->InsertToPool(UIType, UI);
}
Rect CGameInstance::Get_WinSize()
{
	return m_UIManager->Get_WinSize();
}

shared_ptr<CUI> CGameInstance::Find_UI_InCurLevel(EUILayer layer, wstring type)
{
	return m_UIManager->Find_UI_InCurLevel(layer, type);
}

CEventBus* CGameInstance::Get_EventBus()
{
	return m_eventBus.get(); ;
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
	m_DInputManager->Set_MousePos(x, y);
}

_float2 CGameInstance::Get_MousePos()
{
	return m_DInputManager->Get_MousePos();

}

bool CGameInstance::ClearMap(ESaveType dataType)
{
	return m_dataManager->ClearMap(dataType);
}

bool CGameInstance::Load(ESaveType dataType, const string& fileName)
{
	return m_dataManager->Load(dataType, fileName);
}

bool CGameInstance::Save(ESaveType dataType, const string& fileName)
{
	return m_dataManager->Save(dataType, fileName);

}

const _float4x4* CGameInstance::Get_Transform(ED3DTS transformState) const
{
	return m_pipeLine->Get_Transfrom(transformState);
}

const _float4x4* CGameInstance::Get_InverseTransfrom(ED3DTS transformState) const
{
	return m_pipeLine->Get_InverseTransfrom(transformState);
}

const _float4* CGameInstance::Get_CamPositon() const
{
	return m_pipeLine->Get_CamPosition();
}

void CGameInstance::Set_Transform(ED3DTS transformState, _fmatrix transformStateMatrix)
{
	return m_pipeLine->Set_Transform(transformState, transformStateMatrix);
}

HRESULT CGameInstance::Bind_CamPosition(shared_ptr<class CShader> shader, const _char* constantName)
{
	return m_pipeLine->Bind_CamPosition(shader, constantName);
}

HRESULT CGameInstance::Bind_TransformMatrix(ED3DTS transformState, shared_ptr<class CShader> shader, const _char* constantName)
{
	return m_pipeLine->Bind_TransformMatrix(transformState, shader, constantName);
}

HRESULT CGameInstance::Bind_TransformMatrix_Inverse(ED3DTS transformState, shared_ptr<class CShader> shader, const _char* constantName)
{
	return m_pipeLine->Bind_TransformMatrix_Inverse(transformState, shader, constantName);
}

const LightDesc* CGameInstance::Get_LightDesc(_uint index)
{
	return m_lightManager->Get_LightDesc(index);
}

shared_ptr<CLight> CGameInstance::Add_Light(const LightDesc& lightDesc)
{
	return m_lightManager->Add_Light(lightDesc);
}

void CGameInstance::Set_LightDesc(_uint index, LightDesc desc)
{
	m_lightManager->Set_LightDesc(index, desc);
}

void CGameInstance::Clear_Light()
{
	return m_lightManager->clear_light();
}

_bool CGameInstance::Compute_HeightOnTerrain(_fvector pos, _float* Out, _wstring layerTag, _uint terrainIndex)
{


	return m_pickingManager->Compute_HeightOnTerrain(layerTag, terrainIndex, pos, Out);
}

_bool CGameInstance::Picking_Terrain(_wstring layerTag, _uint terrainIndex, _float3* Out)
{

	return m_pickingManager->Picking_Terrain(layerTag, terrainIndex, Out);
}

HRESULT CGameInstance::Add_Camera(_uint camLevel, _wstring key, shared_ptr<CCamera> cam)
{
	return m_cameraManager->Add_Camera(camLevel, key, cam);
}




_bool CGameInstance::Change_Camera(_wstring key)
{
	return m_cameraManager->Change_Camera(key);
}

void CGameInstance::CAM_Manger_OnGui()
{
	m_cameraManager->OnGui();
}

float CGameInstance::Get_Far()
{
	return m_cameraManager->Get_Far();
}

HRESULT CGameInstance::Add_Font(const _wstring& fontTag, const _tchar* fontFilePath)
{
	return m_fontManager->Add_Font(fontTag, fontFilePath);
}

void CGameInstance::Font_Draw(const _wstring& fontTag, const _tchar* text, const _float2& position, _fvector color)
{
	m_fontManager->Draw(fontTag, text, position, color);
}

_float2 CGameInstance::Measure_String(const _wstring& fontTag, const _tchar* text)
{
	return m_fontManager->Measure_String(fontTag, text);

}

void CGameInstance::ComputeTime(_uint& day, _float& hour, _float& minute, _float& second)
{
	m_timeOfDay->ComputeTime(day, hour, minute, second);
}

_float CGameInstance::Get_TOD01()
{
	return m_timeOfDay->Get_TOD01();
}

void CGameInstance::Set_TOD01(_float tod)
{
	return m_timeOfDay->Set_TOD01(tod);
}

void CGameInstance::Set_TimeScale(_float timeScale)
{
	m_timeOfDay->Set_TimeScale(timeScale);
}

_bool CGameInstance::Get_IsNight()
{
	return m_timeOfDay->Get_IsNight();
}


void CGameInstance::Add_Collider(shared_ptr<CCollider> coll)
{
	m_collisionManager->Add_Collider(coll);
}

HRESULT CGameInstance::Add_RenderTarget(const _wstring& targetTag, _uint sizeX, _uint sizeY, DXGI_FORMAT pixelFormat, const _float4& clearColor)
{
	return m_targetManager->Add_RenderTarget(targetTag, sizeX, sizeY, pixelFormat, clearColor);
}

HRESULT CGameInstance::Bind_RT_ShaderResource(shared_ptr<CShader> shader, const _char* constantName, const _wstring& targetTag)
{
	return m_targetManager->Bind_ShaderResource(shader, constantName, targetTag);
}

HRESULT CGameInstance::Add_MRT(const _wstring& MRTTag, const _wstring& targetTag)
{
	return m_targetManager->Add_MRT(MRTTag, targetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& MRTTag, bool useDepth)
{
	return m_targetManager->Begin_MRT(MRTTag, useDepth);
}

HRESULT CGameInstance::End_MRT()
{
	return m_targetManager->End_MRT();
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring& targetTag, _float x, _float y, _float sizeX, _float sizeY)
{
	return m_targetManager->Ready_Debug(targetTag, x, y, sizeX, sizeY);
}

HRESULT CGameInstance::Render_RT_Debug(shared_ptr<CVIBuffer_Rect> VIBuffer, shared_ptr < CShader> shader, const _wstring& MRTTag)
{
	return m_targetManager->Render(VIBuffer, shader, MRTTag);
}
#endif

HRESULT CGameInstance::Render_Lights(shared_ptr<CShader> shader, shared_ptr< CVIBuffer_Rect> VIBuffer)
{
	return m_lightManager->Render_Light(shader, VIBuffer);
}
#ifdef _DEBUG
void CGameInstance::Add_DebugenderGroup(shared_ptr<CComponent> debugComponent)
{
	m_renderer->Add_DebugenderGroup(debugComponent);
}
#endif
HRESULT CGameInstance::Register_Sounds(const vector<SoundAssetDesc>& vecSoundDescs)
{
	if (!m_soundManager)
		return E_FAIL;

	return m_soundManager->Register_Sounds(vecSoundDescs);
}
void CGameInstance::Play_Once(const _wstring& soundTag, _float volumeScale)
{
	if (!m_soundManager)
		return;

	m_soundManager->Play_Once(soundTag, volumeScale);
}
void CGameInstance::Play_Loop(const _wstring& soundTag, _float volumeScale)
{
	if (!m_soundManager)
		return;

	m_soundManager->Play_Loop(soundTag, volumeScale);
}
void CGameInstance::Stop(const _wstring& soundTag, float fadeOutTime)
{
	if (!m_soundManager)
		return;

	m_soundManager->Stop_Sound(soundTag,  fadeOutTime);
}
void CGameInstance::Stop_All()
{
	if (!m_soundManager)
		return;

	m_soundManager->Stop_AllSounds();
}
void CGameInstance::Set_CategoryVolume(ESoundCategory category, _float volume)
{
	if (!m_soundManager)
		return;

	m_soundManager->Set_SoundCategoryVolume(category, volume);
}

void CGameInstance::Free()
{
	__super::Free();

	m_managerForImgui.clear();


	m_pickingManager.reset();
	m_cameraManager.reset();
	m_fontManager.reset();
	m_timeOfDay.reset();
	m_collisionManager.reset();
	m_targetManager.reset();
	m_objectManager.reset();
	m_protoManager.reset();
	m_levelManager.reset();
	m_timerManager.reset();
	m_eventBus.reset();
	m_renderer.reset();
	m_UIManager.reset();
	m_dataManager.reset();
	m_pipeLine.reset();
	m_lightManager.reset();

	m_soundManager->Free();
	m_soundManager.reset();

	m_graphicDevice.reset();



	//	m_pImgui_Manager.reset();

}
