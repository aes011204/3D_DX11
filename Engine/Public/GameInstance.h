#pragma once
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
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ComPtr<ID3D11Device>& ppDevice, _Out_ ComPtr<ID3D11DeviceContext>& ppContext);
	void Update_Engine(float fTimeDelta);
	void Draw();

	void Clear_Resources(_uint iLevelIndex);

	_float Random(_float fMin, _float fMax);

	void SetImguiContext(ImGuiContext* imgContext);

	bool m_bDebug = {true};
	bool Get_IsDebug() { return m_bDebug; };

public:  /* For.Graphic_Device */
	HRESULT Clear_Buffers(const _float4* pClearColor);
	HRESULT Bind_BackBufferRenderTarget(HWND hwnd);
	HRESULT Present();

public: /* For.IMGUI*/
	HRESULT Resize(_uint g_RsizeWidth, _uint g_RsizeHeight);

public: /* For.TimerManager*/
	HRESULT Add_Timer(const _wstring& strTimeTag);
	_float Compute_TimeDelta(const _wstring& strTimeTag);

public:/* For.levelManager*/
	HRESULT Change_Level(_uint iNewLevelIndex, shared_ptr<class CLevel> pNewLevel);

public: /* For.PrototypeManager*/
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, shared_ptr<CBase> pPrototype);
	shared_ptr<CBase> Clone_Prototype(PROTOTYPE ePrototy, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);;
	//shared_ptr<CBase> Get_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);;

public: /*For.GameObject_Manager*/
	shared_ptr<CGameObject> Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	shared_ptr<CGameObject> Get_GameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint GObjIndex);


public:
	/*For.Renderer*/
	void Add_RenderGroup(RENDERGROUP eRenderGroup, shared_ptr<class CEntity> pNTT);


public: /*For.Editor*/
	const map<const _wstring, shared_ptr<CLayer>>& Get_GameObjects(_uint levelIndex) const;
	_uint Get_Current_LevelIdx();
	//const vector<shared_ptr<CUI>>& GetUIList(UI_LAYER layer);
	//const unordered_map<wstring, shared_ptr<CUI>>& GetUIPool();
	class CUI_Manager* Get_UI_Manager() const;
	class CGraphic_Device* Get_GraphicDevice() const;
	const map<_wstring,CBase*>& Get_ManagerClass()const;
	void Push_ManagerClass(_wstring strKey, CBase* ManagerClass);

public:/*For.UI_Manager*/
	void UI_Push(UI_LAYER layer, wstring name, _bool isOnActive = true, void* pArg = nullptr);
	void UI_Pop(UI_LAYER layer, wstring type); // 레이어에서 넣얶다 뻇다하는건 안씀 
	void UI_Detach_All(); // 씬 전환 할떄 레이어에 있는거 객체를 지우니까 그전에 
	void UI_InsertToPool(wstring UIType, shared_ptr<CUI> UI);
	Rect Get_WinSize();
	shared_ptr<CUI> Find_UI_InCurLevel(UI_LAYER layer, wstring type);

public:/*For.EventBus*/
	CEventBus* Get_EventBus();

public:/*For.DInput_Manager*/
	class CDInput_Manager* Get_DInput_Manger() { return m_pDInput_Manager.get(); }
	void Set_MousePos(float x, float y);
	_float2 Get_MousePos();

public:/*For.Data_Manager*/
	class CData_Manager* Get_Data_Manager() { return m_pData_Manager.get(); }
	bool ClearMap(SAVETYPE eDATATYPE);
	bool Load(SAVETYPE eDATATYPE, const string& fileName);
	bool Save(SAVETYPE eDATATYPE, const string& fileName);

public:/*For.PipeLine*/
	const _float4x4* Get_Transform(D3DTS eTransformState) const;
	const _float4x4* Get_InverseTransfrom(D3DTS eTransformState) const;
	const _float4* Get_CamPositon() const;
	void Set_Transform(D3DTS eTransformState, _fmatrix TransformStateMatrix);
	HRESULT Bind_CamPosition(shared_ptr<class CShader> pShader, const _char* pConstantName);
	HRESULT Bind_TransformMatrix(D3DTS eTransformState, shared_ptr<class CShader> pShader, const _char* pConstantName);
	HRESULT Bind_TransformMatrix_Inverse(D3DTS eTransformState, shared_ptr<class CShader> pShader, const _char* pConstantName);
public:/*For.Light_Manager*/
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);

public:/*For.Picking_Manager*/
	_bool Compute_HeightOnTerrain(_fvector pPos, _float* Out, _wstring layerTag = L"Layer_BackGround", _uint TerrainIndex = 0);
	_bool Picking_Terrain(_wstring layerTag, _uint TerrainIndex, _float3* Out);
public:/*For.Camera_Manager*/
	HRESULT Add_Camera(_uint camLevel, _wstring key, shared_ptr<class CCamera> cam);
	_bool Change_Camera(_wstring key);
	void CAM_Manger_OnGui();

public:/*For.Font_Manager*/
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	void Font_Draw(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = { 1.f,1.f,1.f,1.f });
	_float2 Measure_String(const _wstring& strFontTag, const _tchar* pText);

	public:/*For.TimeOfDay*/
		void ComputeTime(_uint& iDay, _float& fHour, _float& fMinute, _float& fSecond);
		_float Get_TOD01();
	public:/*For.Collison_Mgr*/
		void Add_Collider(shared_ptr<class CCollider> coll);


private:
	unique_ptr<class CGraphic_Device> m_pGraphic_Device = { nullptr };
	unique_ptr<class CTimer_Manager> m_pTimer_Manager = { nullptr };
	unique_ptr<class CLevel_Manager> m_pLevel_Manager = { nullptr };
	unique_ptr<class CPrototype_Manager> m_pProto_Manager = { nullptr };
	unique_ptr<class CObject_Manager> m_pObject_Manager = { nullptr };
	unique_ptr<class CRenderer> m_Renderer = { nullptr };
	unique_ptr<class CUI_Manager> m_UI_Manager = { nullptr };
	unique_ptr<class CDInput_Manager> m_pDInput_Manager = { nullptr };
	unique_ptr<class CData_Manager> m_pData_Manager = { nullptr };
	unique_ptr<class CPipeLine> m_pPipeLine = { nullptr };
	unique_ptr<class CLight_Manager> m_pLight_Manager = { nullptr };
	unique_ptr<class CCamera_Manager> m_pCamera_Manager = { nullptr };
	unique_ptr<class CPicking_Manager> m_pPicking_Manager = { nullptr };
	unique_ptr<class CFont_Manager> m_pFont_Manager = { nullptr };
	unique_ptr<class CTimeOfDay> m_pTimeOfDay = { nullptr };
	unique_ptr<class CCollision_Manager> m_pCollision_Manager = { nullptr };

//	unique_ptr<class CImguiManager> m_pImgui_Manager = { nullptr };

	map<_wstring,CBase*> m_ManagerForImgui = {}; // rawPointer 참조용

	unique_ptr<CEventBus> m_pEventBus;
public:
	virtual void Free() override;
};


NS_END
