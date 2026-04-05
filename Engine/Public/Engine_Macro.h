
#ifndef Engine_Macro_h__
#define Engine_Macro_h__

namespace Engine
{
    /* ------------------------------------------ */
    /*                캐스팅 매크로                */
    /* ------------------------------------------ */

#define ETOI(ENUMCLASS)         static_cast<unsigned int>(ENUMCLASS)
// ↓ C++ 17 이상
#define WSTR_TO_STR(WSTR)       std::filesystem::path(WSTR).string()

/* ------------------------------------------ */
/*            weak_ptr Lock 매크로             */
/* ------------------------------------------ */

// weak_ptr가 가리키는 객체를 안전하게 참조(Reference Count 처리)하기 위해 
// lock()을 통해 shared_ptr로 승격 후 사용
#define LOCK_WP(weak_ptr, var_name, ...)                                        \
    auto var_name = (weak_ptr).lock();                                          \
    if (!var_name)                                                              \
    {                                                                           \
        LOG_ERROR(#weak_ptr " Lock Failed");                                    \
        return __VA_ARGS__;                                                     \
    }

/* ------------------------------------------ */
/*               CRT Debug Heap               */
/* ------------------------------------------ */
//
//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#include <stdlib.h>
//
//#ifndef DBG_NEW
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#define new DBG_NEW
//#endif
//#endif
//
/* ------------------------------------------ */
/*          편의성 메시지 박스 매크로           */
/* ------------------------------------------ */

#ifdef _DEBUG
#define MSG_BOX(_message)       MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
#else
#define MSG_BOX(_message)       ((void)0)
#endif

/* ------------------------------------------ */
/*                 로깅 매크로                 */
/* ------------------------------------------ */

#define LOG_TRACE(...)          SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...)          SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_INFO(...)           SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...)           SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...)          SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...)       SPDLOG_CRITICAL(__VA_ARGS__)

#define LOG_FAILED(hr, msg, ...)                                                \
    if (FAILED(hr)) {                                                           \
        LOG_ERROR(msg, ##__VA_ARGS__);                                          \
    }

#define LOG_FAILED_RETURN(hr, msg, ...)                                         \
    if (FAILED(hr)) {                                                           \
        LOG_ERROR(msg, ##__VA_ARGS__);                                          \
        return E_FAIL;                                                          \
    }

/* ------------------------------------------ */
/*           네임스페이스 관련 매크로           */
/* ------------------------------------------ */

// static_assert(true, "") : 더미 코드
#define NS_BEGIN(NAMESPACE)     namespace NAMESPACE { static_assert(true, "");
#define NS_END                  }

// DLL import/export 제어 매크로
#ifdef  ENGINE_EXPORTS
#define ENGINE_DLL              _declspec(dllexport)
#else
#define ENGINE_DLL              _declspec(dllimport)
#endif

/* ------------------------------------------ */
/*              NULL 검사 매크로               */
/* ------------------------------------------ */

// _ptr이 nullptr이면 __VA_ARGS__로 지정한 값을 반환 (비어있으면 void 반환)
#define CHECK_NULL(_ptr, ...)                                                   \
{                                                                               \
    if ((_ptr) == 0) {                                                          \
        LOG_ERROR("NULL Pointer Detected!");                                    \
        return __VA_ARGS__;                                                     \
    }                                                                           \
}

/* ------------------------------------------ */
/*            HRESULT 검사 매크로              */
/* ------------------------------------------ */

// HRESULT가 실패면 __VA_ARGS__로 지정한 값을 반환 (비어있으면 void 반환)
#define CHECK_FAILED(_hr, ...)                                                  \
{                                                                               \
    if (((HRESULT)(_hr)) < 0) {                                                 \
        LOG_ERROR("Check Failed!");                                             \
        return __VA_ARGS__;                                                     \
    }                                                                           \
}

/* ------------------------------------------ */
/*          싱글톤 클래스 구조 매크로           */
/* ------------------------------------------ */

// 복사를 금지하여 싱글톤 등에서 안전한 사용을 보장
#define NO_COPY(CLASSNAME)                                                      \
			private:                                                            \
			CLASSNAME(const CLASSNAME&) = delete;                               \
			CLASSNAME& operator = (const CLASSNAME&) = delete;

// 클래스를 싱글톤으로 선언할 때 사용한다. 헤더에서 호출
#define DECLARE_SINGLETON(CLASSNAME)                                            \
			NO_COPY(CLASSNAME)                                                  \
			private:                                                            \
			static std::shared_ptr<CLASSNAME>   m_spInstance;                   \
			public:                                                             \
			static std::shared_ptr<CLASSNAME>   GetInstance( void );            \
			static unsigned int DestroyInstance( void );

// DECLARE_SINGLETON과 짝을 이루며 cpp에서 정의 부분을 제공
#define IMPLEMENT_SINGLETON(CLASSNAME)                                          \
            std::shared_ptr<CLASSNAME> CLASSNAME::m_spInstance = nullptr;       \
            std::shared_ptr<CLASSNAME>  CLASSNAME::GetInstance( void ) {        \
                if(nullptr == m_spInstance) {                                   \
                    m_spInstance = std::shared_ptr<CLASSNAME>(new CLASSNAME,    \
                    [](CLASSNAME* p) {                                          \
                        if (nullptr == p)                                       \
                            return;                                             \
                        p->Free();                                              \
                        delete p;                                               \
                    });                                                         \
                }                                                               \
                return m_spInstance;                                            \
            }                                                                   \
            unsigned int CLASSNAME::DestroyInstance( void ) {                   \
                if(nullptr != m_spInstance)	{                                   \
                    m_spInstance.reset();                                       \
                }                                                               \
                return 0;                                                       \
            }

}

#define LOG_F(level, format, ...) \
    CLog_Manager::GetInstance()->Add_Log_F(level, format, ##__VA_ARGS__)

#define LOG(level, message) \
    CLog_Manager::GetInstance()->Add_Log(level, message)

#define PI 3.14159265359f
#define PI2 6.28318530718f

#endif // Engine_Macro_h__
