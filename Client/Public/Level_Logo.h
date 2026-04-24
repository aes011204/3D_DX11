#pragma once
#include "Client_Define.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CLevel_Logo() = default;

public:
	HRESULT Ready_Layer_Camera(const wchar_t* str);
	HRESULT Ready_Layer_Wave(const wchar_t* str);
	HRESULT Ready_Lights();

	virtual HRESULT Initialize() override;
	virtual HRESULT Post_Initialize()override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	_bool  m_Flag = { false };
	LEVEL m_NextLevel = {};
	_bool ChangeNextLevel = { false };
	_float m_Acc = {};
public:
	static shared_ptr<CLevel_Logo> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

NS_END;