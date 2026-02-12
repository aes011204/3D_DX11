#pragma once
#include "Base.h"

/* 1.사본객체들을 보관한다. */
/* 1-1. 레벨별로 구분하여 보관한다. */
/* 1-2. 내가 구분하고 싶은 기준(CLayer)을 두고 나누어 보관한다. */
/* 2.보관하고 있는 객체들의 갱신(업데이트)을 담당한다.*/

NS_BEGIN(Engine)

class CLayer;
class CGameObject;

class CObject_Manager final :
	public CBase
{
private:
	CObject_Manager();
public:
	virtual ~CObject_Manager() ;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Clear_Layers(_uint iLevelIndex);


	/// IMGUI
	virtual void Update_Gui();

	map<const _wstring, shared_ptr<CLayer>> Get_GameObjects(_uint levelIndex);
	///
private:
	_uint m_iNumLevel = {};
	// map<_wstring, list<shared_ptr<class CGameObject>>>* m_pLayers = { nullptr };
	map<const _wstring, shared_ptr<CLayer>>* m_pLayers = { nullptr };
	typedef map<const _wstring, shared_ptr<CLayer>> LAYERS;

	// 또 동적 배열로 레벨별로 분리해서 보관, 그 안에 const _wstring, class shared_ptr<CLayer> 이걸로 레이어로 분리

	weak_ptr<class CGameInstance> m_pGameInstance = {};

public:
	class shared_ptr<CLayer> Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);

public:
	static unique_ptr<CObject_Manager> Create(_uint iNumLevels);
	void Free()override;

};

NS_END