 #pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)
inline _string W2S(const wstring& wstr)
{
    if (wstr.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    _string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
    if (!str.empty() && str.back() == '\0') str.pop_back();
    return str;
}

inline wstring S2W(const _string& str)
{
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    wstring w(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &w[0], size);
    if (!w.empty() && w.back() == L'\0') w.pop_back();
    return w;
}


template <typename T>
_wstring ToWStr(T eValue) {
    return _wstring(magic_enum::enum_name<T, wchar_t>(eValue));
}

NS_END