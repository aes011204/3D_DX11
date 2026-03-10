#pragma once
#include "Level.h"


class CLevel_Editor :
    public CLevel
{
private:
	CLevel_Editor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CLevel_Editor() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Post_Initialize()override { return S_OK; };
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void OnGui()override;
public:
	static shared_ptr<CLevel_Editor> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;

};

