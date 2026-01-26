#include "Loader.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY ThreadMain(void* pArg)
{
	// pArg로 pLoader 주소를 받아옴 근데  void* 형이라서 형변환후 사용 가능
	CLoader* pLoader = static_cast<CLoader*>(pArg);
	if (nullptr == pLoader)
		return 1;

	// 쓰레드를 이용해서 로딩 클래스에 있는 로딩 함수를 돌린다
	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	// 실제 자원을 로드해주기 위한 스레드를 형성한다
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

	if(0 == m_hThread)
	{
		MSG_BOX("Failed to Created : m_hThread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading()
{
	// 다른 쓰레드가 공유하는 메모리영역(임계영역) 접근시 막아주는(대기로 바꿔주는) 함수 (락)
	EnterCriticalSection(&m_CriticalSection);

	HRESULT hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL::GMAEPLAYE:
		hr = Loading_For_GamePlayLevel();
		break;
	default:
		hr = E_FAIL;
	}

	LeaveCriticalSection(&m_CriticalSection);// 언락 - 이제 다른쓰레드도 접근가능

	return hr;
}

HRESULT CLoader::Loading_For_LogoLevel()
{

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	for (size_t i = 0; i < 99999999; i++)
		_int iData = 10;

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩 중 입니다."));
	for (size_t i = 0; i < 99999999; i++)
		_int iData = 10;

	lstrcpy(m_szLoadingText, TEXT("사운드를 로딩 중 입니다."));
	for (size_t i = 0; i < 99999999; i++)
		_int iData = 10;

	lstrcpy(m_szLoadingText, TEXT("모델를 로딩 중 입니다."));
	for (size_t i = 0; i < 99999999; i++)
		_int iData = 10;

	lstrcpy(m_szLoadingText, TEXT("객체원형를 로딩 중 입니다."));
	for (size_t i = 0; i < 99999999; i++)
		_int iData = 10;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_bFinished = true;
	
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	for (size_t i = 0; i < 99999999; i++)
		_int iData = 10;

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩 중 입니다."));
	for (size_t i = 0; i < 99999999; i++)
		_int iData = 10;

	lstrcpy(m_szLoadingText, TEXT("사운드를 로딩 중 입니다."));
	for (size_t i = 0; i < 99999999; i++)
		_int iData = 10;

	lstrcpy(m_szLoadingText, TEXT("모델를 로딩 중 입니다."));
	for (size_t i = 0; i < 99999999; i++)
		_int iData = 10;

	lstrcpy(m_szLoadingText, TEXT("객체원형를 로딩 중 입니다."));
	for (size_t i = 0; i < 99999999; i++)
		_int iData = 10;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_bFinished = true;

	return S_OK;

}

#ifdef _DEBUG
void CLoader::Print_LoadText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}
#endif

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	//쓰레드 사용하고 있음 갑자기 끝나면 무한루프로 쓰레드 끝날떄까지 기다리고 다 끝나면 지운다
	WaitForSingleObject(m_hThread, INFINITE); 
	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
