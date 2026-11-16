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
	HRESULT Initialize(_uint numLevels);
	void Priority_Update(_float timeDelta);
	void Update(_float timeDelta);
	void Late_Update(_float timeDelta);
	HRESULT Clear_Layers(_uint levelIndex);

	shared_ptr<CGameObject> Add_GameObject(_uint prototypeLevelIndex, const _wstring& prototypeTag, _uint layerLevelIndex, const _wstring& layerTag, void* arg);
	shared_ptr<CGameObject> Get_GameObject(_uint levelIndex, const _wstring& layerTag, _uint gameObjIndex);
	/// IMGUI
	virtual void Update_Gui();

	const map<const _wstring, shared_ptr<CLayer>>& Get_GameObjects(_uint levelIndex) const;
	///
private:
	_uint m_numLevel = {};
	// map<_wstring, list<shared_ptr<class CGameObject>>>* m_pLayers = { nullptr };
	map<const _wstring, shared_ptr<CLayer>>* m_layers = { nullptr };
	typedef map<const _wstring, shared_ptr<CLayer>> LAYERS;

	// 또 동적 배열로 레벨별로 분리해서 보관, 그 안에 const _wstring, class shared_ptr<CLayer> 이걸로 레이어로 분리

	weak_ptr<class CGameInstance> m_gameInstance = {};

public:
	class shared_ptr<CLayer> Find_Layer(_uint levelIndex, const _wstring& layerTag);

public:
	static unique_ptr<CObject_Manager> Create(_uint numLevels);
	void Free()override;

};

NS_END