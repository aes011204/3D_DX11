// Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Client.h"

#include "Client_Define.h"
#include "EditorInstance.h"
#include "MainApp.h"
#include "GameInstance.h"

UINT g_RsizeWidth;
UINT g_RsizeHeight;
bool g_bPendingResize = false;


#define MAX_LOADSTRING 100

// 전역 변수:
HWND g_hWnd;
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
WNDCLASSEXW wcex;          // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#endif

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
	//비율이슈
	//SetProcessDPIAware();
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow)) //WIN handle 생성 // 이 이후  create MainApp
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

	MSG msg;


	unique_ptr<CMainApp> pMainApp = CMainApp::Create();
	if (nullptr == pMainApp)
		return FALSE;

	weak_ptr<CGameInstance> GameInstance = CGameInstance::GetInstance();
	if (nullptr == GameInstance.lock())
		return FALSE;

	weak_ptr<CEditorInstance> EditorInstance = CEditorInstance::GetInstance();
	if (nullptr == EditorInstance.lock())
		return FALSE;

	if (FAILED(GameInstance.lock()->Add_Timer(TEXT("Timer_Default"))))
		return FALSE;
	if (FAILED(GameInstance.lock()->Add_Timer(TEXT("Timer_60"))))
		return FALSE;

	_float fTimerAcc = {};

	// 기본 메시지 루프입니다:
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				if (WM_QUIT == msg.message)
					break;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}



		// 창이 최소화되어 있는지 체크 (IsIconic 함수 사용)
		if (IsIconic(g_hWnd))
		{
			Sleep(10); // CPU 점유율 방지
			continue;  // 최소화 상태면 Update/Render 아예 안 함
		}

		if (g_bPendingResize)
		{
			if (g_RsizeWidth != 0 && g_RsizeHeight != 0)
			{

				GameInstance.lock()->Resize(g_RsizeWidth, g_RsizeHeight);
		
				// ImGui의 디스플레이 사이즈를 강제로 맞춰줌
				if (ImGui::GetCurrentContext()) {
					ImGuiIO& io = ImGui::GetIO();
					io.DisplaySize = ImVec2((float)g_RsizeWidth, (float)g_RsizeHeight);
				}
			}
			g_bPendingResize = false;
		
		}


		//deltaTime

		fTimerAcc += GameInstance.lock()->Compute_TimeDelta(TEXT("Timer_Default"));

		if (fTimerAcc >= 1.f / 60.f)
		{
			pMainApp->Update(GameInstance.lock()->Compute_TimeDelta(TEXT("Timer_60")));
			//pMainApp->LateUpdate();
			pMainApp->Render();


			fTimerAcc = 0.f;
		}
	}

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = /*MAKEINTRESOURCEW(IDC_CLIENT)*/nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.


	RECT rc{ 0, 0, g_iWinSizeX, g_iWinSizeY };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	int windowWidth = rc.right - rc.left;
	int windowHeight = rc.bottom - rc.top;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int centerX = (screenWidth - windowWidth) / 2;
	int centerY = (screenHeight - windowHeight) / 2;

	HWND hWnd = CreateWindowW(szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		centerX,
		centerY,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hWnd = hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
	//g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
	//
	//HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	//	CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	//
	//if (!hWnd)
	//{
	//	return FALSE;
	//}
	//
	//ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);
	//
	//g_hWnd = hWnd;
	//
	//return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
// 
	// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;


	switch (message)
	{
	case WM_SIZE:
	{

		if (wParam == SIZE_MINIMIZED)
			break;
		g_RsizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_RsizeHeight = (UINT)HIWORD(lParam);
		g_bPendingResize = true;
	}
	break;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // alt 눌럿을떄 win 단축키 떄문에 튀는거 막는 코드    
			break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
