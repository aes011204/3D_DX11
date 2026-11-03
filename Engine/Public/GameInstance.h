#pragma once
#include "Light.h"
#include "Prototype_Manager.h"


NS_BEGIN(Engine)

class CLayer;
class CUI;
class CObject_Manager;
class CEventBus;
class CGameObject;


class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
public:
	virtual ~CGameInstance();

public:
	HRESULT Initialize_Engine(const EngineDesc& engineDesc, _Out_ ComPtr<ID3D11Device>& ppDevice, _Out_ ComPtr<ID3D11DeviceContext>& ppContext);
	void Update_Engine(float timeDelta);
	void Draw();

    struct FrameTimings
    {
        double updateMs = 0.0;
        double uiPrepareMs = 0.0;
        double renderCpuMs = 0.0;
        double renderGpuMs = 0.0;
        double presentMs = 0.0;
        bool gpuReady = false;
        double seaCpuMs = 0.0;
        double seaGpuMs = 0.0;
        bool seaGpuReady = false;
    };
    const FrameTimings& Get_FrameTimings() const { return m_frameTimings; }

	void Clear_Resources(_uint levelIndex);

	_float Random(_float minValue, _float maxValue);
	_int RandomInt(_int minValue, _int maxValue);

	void SetImguiContext(ImGuiContext* imgContext);

	bool m_isDebug = {true};
	_float3 m_syColor = {};
	bool Get_IsDebug() { return m_isDebug; };

	const _float3& Get_SkyColor() const{return m_syColor;}
	void Set_SkyColor(const _float3& color){m_syColor = color;}
public:  /* For.Graphic_Device */
	HRESULT Clear_Buffers(const _float4* clearColor);
	HRESULT Bind_BackBufferRenderTarget(HWND hwnd);
	HRESULT Present();

public: /* For.IMGUI*/
	HRESULT Resize(_uint g_RsizeWidth, _uint g_RsizeHeight);

public: /* For.TimerManager*/
	HRESULT Add_Timer(const _wstring& timeTag);
	_float Compute_TimeDelta(const _wstring& timeTag);

public:/* For.levelManager*/
	HRESULT Change_Level(_uint newLevelIndex, shared_ptr<class CLevel> newLevel);

public: /* For.PrototypeManager*/
	HRESULT Add_Prototype(_uint levelIndex, const _wstring& prototypeTag, shared_ptr<CBase> prototype);
	shared_ptr<CBase> Clone_Prototype(EPrototype prototy, _uint levelIndex, const _wstring& prototypeTag, void* arg = nullptr);;
	//shared_ptr<CBase> Get_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);;

public: /*For.GameObject_Manager*/
	shared_ptr<CGameObject> Add_GameObject(_uint prototypeLevelIndex, const _wstring& prototypeTag, _uint layerLevelIndex, const _wstring& layerTag, void* arg = nullptr);
	shared_ptr<CGameObject> Get_GameObject(_uint layerLevelIndex, const _wstring& layerTag, _uint gameObjIndex);


public:
	/*For.Renderer*/
	void Add_RenderGroup(ERenderGroup renderGroup, shared_ptr<class CEntity> NTT);
	void Add_DebugenderGroup(shared_ptr<CComponent> debugComponent);

public: /*For.Editor*/
	const map<const _wstring, shared_ptr<CLayer>>& Get_GameObjects(_uint levelIndex) const;
	_uint Get_Current_LevelIdx();
	//const vector<shared_ptr<CUI>>& GetUIList(UI_LAYER layer);
	//const unordered_map<wstring, shared_ptr<CUI>>& GetUIPool();
	class CUI_Manager* Get_UI_Manager() const;
	class CGraphic_Device* Get_GraphicDevice() const;
	const map<_wstring,CBase*>& Get_ManagerClass()const;
	void Push_ManagerClass(_wstring key, CBase* managerClass);

public:/*For.UI_Manager*/
	void UI_Push(EUILayer layer, wstring name, _bool isOnActive = true, void* arg = nullptr);
	void UI_Pop(EUILayer layer, wstring type); // 레이어에서 넣얶다 뻇다하는건 안씀 
	void UI_Detach_All(); // 씬 전환 할떄 레이어에 있는거 객체를 지우니까 그전에 
	void UI_InsertToPool(wstring UIType, shared_ptr<CUI> UI);
	Rect Get_WinSize();
	shared_ptr<CUI> Find_UI_InCurLevel(EUILayer layer, wstring type);

public:/*For.EventBus*/
	CEventBus* Get_EventBus();

public:/*For.DInput_Manager*/
	class CDInput_Manager* Get_DInput_Manger() { return m_DInputManager.get(); }
	void Set_MousePos(float x, float y);
	_float2 Get_MousePos();

public:/*For.Data_Manager*/
	class CData_Manager* Get_Data_Manager() { return m_dataManager.get(); }
	bool ClearMap(ESaveType dataType);
	bool Load(ESaveType dataType, const string& fileName);
	bool Save(ESaveType dataType, const string& fileName);

public:/*For.PipeLine*/
	const _float4x4* Get_Transform(ED3DTS transformState) const;
	const _float4x4* Get_InverseTransfrom(ED3DTS transformState) const;
	const _float4* Get_CamPositon() const;
	void Set_Transform(ED3DTS transformState, _fmatrix transformStateMatrix);
	HRESULT Bind_CamPosition(shared_ptr<class CShader> shader, const _char* constantName);
	HRESULT Bind_TransformMatrix(ED3DTS transformState, shared_ptr<class CShader> shader, const _char* constantName);
	HRESULT Bind_TransformMatrix_Inverse(ED3DTS transformState, shared_ptr<class CShader> shader, const _char* constantName);

public:/*For.Light_Manager*/
	const LightDesc* Get_LightDesc(_uint index);
	shared_ptr<CLight> Add_Light(const LightDesc& lightDesc);
	void Set_LightDesc(_uint index, LightDesc desc);
	void Clear_Light();
public:/*For.Picking_Manager*/
	_bool Compute_HeightOnTerrain(_fvector pos, _float* Out, _wstring layerTag = L"Layer_BackGround", _uint terrainIndex = 0);
	_bool Picking_Terrain(_wstring layerTag, _uint terrainIndex, _float3* Out);

public:/*For.Camera_Manager*/
	HRESULT Add_Camera(_uint camLevel, _wstring key, shared_ptr<class CCamera> cam);
	_bool Change_Camera(_wstring key);
	void CAM_Manger_OnGui();
	float Get_Far();

public:/*For.Font_Manager*/
	HRESULT Add_Font(const _wstring& fontTag , const _tchar* fontFilePath);
	void Font_Draw(const _wstring& fontTag, const _tchar* text, const _float2& position, _fvector color = { 1.f,1.f,1.f,1.f });
	_float2 Measure_String(const _wstring& fontTag, const _tchar* text);

	public:/*For.TimeOfDay*/
		void ComputeTime(_uint& day, _float& hour, _float& minute, _float& second);
		_float Get_TOD01();
		void Set_TOD01(_float tod);
		void Set_TimeScale(_float timeScale);
		_bool Get_IsNight();

	public:/*For.Collison_Mgr*/
		void Add_Collider(shared_ptr<class CCollider> coll);

		public:/*For.Target_Manager_Mgr*/
	HRESULT Add_RenderTarget(const _wstring& targetTag, _uint sizeX, _uint sizeY, DXGI_FORMAT pixelFormat,
	                         const _float4& clearColor);
	HRESULT Bind_RT_ShaderResource(shared_ptr<CShader> shader, const _char* constantName,
	                               const _wstring& targetTag);

	HRESULT Add_MRT(const _wstring& MRTTag, const _wstring& targetTag);
	HRESULT Begin_MRT(const _wstring& MRTTag, bool useDepth = true);
	HRESULT End_MRT();
#ifdef _DEBUG
public:
	HRESULT Ready_RT_Debug(const _wstring& targetTag, _float x, _float y, _float sizeX, _float sizeY);
	HRESULT Render_RT_Debug(shared_ptr<CVIBuffer_Rect> VIBuffer, shared_ptr<CShader> shader, const _wstring& MRTTag);
#endif
public:
	HRESULT Render_Lights(shared_ptr<CShader> shader, shared_ptr< CVIBuffer_Rect> VIBuffer);




public:// SoundManager
	HRESULT Register_Sounds(const vector<SoundAssetDesc>& vecSoundDescs);
	void Update(_float timeDelta);
	void Play_Once(const _wstring& soundTag, _float volumeScale = 1.f);
	void Play_Loop(const _wstring& soundTag, _float volumeScale = 1.f);
	void Stop(const _wstring& soundTag, float fadeOutTime=0);
	void Stop_All();
	void Set_CategoryVolume(ESoundCategory category, _float volume);

private:
    FrameTimings m_frameTimings;
	unique_ptr<class CGraphic_Device> m_graphicDevice = { nullptr };
	unique_ptr<class CTimer_Manager> m_timerManager = { nullptr };
	unique_ptr<class CLevel_Manager> m_levelManager = { nullptr };
	unique_ptr<class CPrototype_Manager> m_protoManager = { nullptr };
	unique_ptr<class CObject_Manager> m_objectManager = { nullptr };
	unique_ptr<class CRenderer> m_renderer = { nullptr };
	unique_ptr<class CUI_Manager> m_UIManager = { nullptr };
	unique_ptr<class CDInput_Manager> m_DInputManager = { nullptr };
	unique_ptr<class CData_Manager> m_dataManager = { nullptr };
	unique_ptr<class CPipeLine> m_pipeLine = { nullptr };
	unique_ptr<class CLight_Manager> m_lightManager = { nullptr };
	unique_ptr<class CCamera_Manager> m_cameraManager = { nullptr };
	unique_ptr<class CPicking_Manager> m_pickingManager = { nullptr };
	unique_ptr<class CFont_Manager> m_fontManager = { nullptr };
	unique_ptr<class CTimeOfDay> m_timeOfDay = { nullptr };
	unique_ptr<class CCollision_Manager> m_collisionManager = { nullptr };
	unique_ptr<class CTarget_Manager> m_targetManager = { nullptr };
//	unique_ptr<class CImguiManager> m_pImgui_Manager = { nullptr };
	unique_ptr<class CSoundManager>m_soundManager = { nullptr };
	map<_wstring,CBase*> m_managerForImgui = {}; // rawPointer 참조용

	unique_ptr<CEventBus> m_eventBus;
public:
	virtual void Free() override;
};


NS_END
