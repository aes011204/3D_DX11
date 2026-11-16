#include "DInput_Manager.h"

//IMPLEMENT_SINGLETON(CDInput_Manager)

CDInput_Manager::CDInput_Manager()
    : m_inputSDK(nullptr),
      m_keyBoardDev(nullptr),
      m_mouseDev(nullptr),
      m_keyState{},
      m_prevKeyState{},
      m_mouseState{},
      m_prevMouseState{},
      m_hWnd(nullptr)
{}

CDInput_Manager::~CDInput_Manager()
{
    CDInput_Manager::Free();
}

HRESULT CDInput_Manager::Initialize(HINSTANCE hInst, HWND hWnd)
{
    if (FAILED(DirectInput8Create(hInst,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&m_inputSDK,
        nullptr)))
        return E_FAIL;

    if (FAILED(m_inputSDK->CreateDevice(GUID_SysKeyboard, &m_keyBoardDev, nullptr)))
        return E_FAIL;

    m_keyBoardDev->SetDataFormat(&c_dfDIKeyboard);

    // 백그라운드 입력 못받게 막기
    //m_KeyBoardDev->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    m_keyBoardDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    m_keyBoardDev->Acquire();

    if (FAILED(m_inputSDK->CreateDevice(GUID_SysMouse, &m_mouseDev, nullptr)))
        return E_FAIL;

    m_mouseDev->SetDataFormat(&c_dfDIMouse);

    //m_MouseDev->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    m_mouseDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    m_mouseDev->Acquire();

    m_hWnd = hWnd;

    return S_OK;
}

void CDInput_Manager::Update_InputDev()
{
    // 키보드 이전 프레임 상태 백업
    memcpy(m_prevKeyState, m_keyState, sizeof(m_keyState));

    // 마우스 이전 프레임 상태 백업
    m_prevMouseState = m_mouseState;

    // 키보드
    if (FAILED(m_keyBoardDev->GetDeviceState(256, m_keyState)))
    {
        // 포커스 잃었을 때 복구용
        m_keyBoardDev->Acquire();
        m_keyBoardDev->GetDeviceState(256, m_keyState);
    }

    // 마우스
    if (FAILED(m_mouseDev->GetDeviceState(sizeof(m_mouseState), &m_mouseState)))
    {
        m_mouseDev->Acquire();
        m_mouseDev->GetDeviceState(sizeof(m_mouseState), &m_mouseState);
    }
}

void CDInput_Manager::OnGui()
{
    _float2 mousePos = GetDInputMousePos();

    ImGui::Text("ImGui Mouse X: %.2f", mousePos.x);
    ImGui::Text("ImGui Mouse Y: %.2f", mousePos.y);

    // 특정 버튼 클릭 여부 확인 (0:좌, 1:우, 2:휠)
    ImGui::Text("Left Click: %s", ImGui::IsMouseDown(0) ? "Down" : "Up");
}

unique_ptr<CDInput_Manager> CDInput_Manager::Create(HINSTANCE hInst, HWND hWnd)
{
    unique_ptr<CDInput_Manager> instance(new CDInput_Manager());
    if (FAILED(instance->Initialize(hInst, hWnd)))
    {
        MSG_BOX("failed prototype: CDInput_Manager");
    }
    return instance;
}

void CDInput_Manager::Free()
{
    m_keyBoardDev.Reset();
    m_mouseDev.Reset();
    m_inputSDK.Reset();

}
