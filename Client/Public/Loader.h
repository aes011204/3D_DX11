#pragma once
#include "Client_Define.h"
#include "Base.h"

// 로딩을 위한 쓰레드를 생성한다
// 지정된 레벨을 위한 자원을 로딩 한다

NS_BEGIN(Engine)
class CGameInstance;
NS_END


NS_BEGIN(Client)

class CLoader :
    public CBase
{
private:
    CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLoader() = default;

public:
    HRESULT Initialize(LEVEL eNextLevelID);
    HRESULT Loading();


    _bool Is_Finished() { return m_bFinished; }

#ifdef _DEBUG
    void Print_LoadText();
#endif

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
	// 이거 쓰레드에서 쓰는게 아니라 생성할 객체에 전달하는 용도 // 쓰레드에서 쓰면 큰일남

    CGameInstance* m_pGameInstance = { nullptr };

    HANDLE m_hThread = {};
    CRITICAL_SECTION m_CriticalSection = {};

    LEVEL m_eNextLevelID = { LEVEL::END };
    _tchar m_szLoadingText[MAX_PATH] = {};
    _bool m_bFinished = { false };
private:
    HRESULT Loading_For_LogoLevel();
    HRESULT Loading_For_GamePlayLevel();

public:
    static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
    void Free() override;

};

NS_END