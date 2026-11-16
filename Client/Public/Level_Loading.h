#pragma once
#include "Client_Define.h"
#include "Level.h"

// 1. 로딩 장면을 구성 해주기 위한 객체를 생성해주고 업데이트와 렌더의 반복적인 수행
// 2. 로당 다음에 등장할 레벨을 위한 자원을 로드 해준다 (CLoader) == 로더를 생성해준다
NS_BEGIN(Engine)
class CUI;
NS_END
NS_BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
public:
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(ELevel nextLevelID) ;
	virtual HRESULT Post_Initialize()override;
	virtual void Update(_float timeDelta) override;
	virtual HRESULT Render() override;



private:
	class shared_ptr<class CLoader> m_loader = { nullptr };
	ELevel m_nextLevelID = { ELevel::END };

	//ko m_LoadingUI = {};
private:
	HRESULT Ready_Layer_BackGround(const _tchar* layerTag);
	HRESULT Ready_Layer_UI(const _tchar* layerTag);

public:
	static shared_ptr<CLevel_Loading> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, ELevel eNextLevel);
	virtual void Free() override;
};

NS_END;