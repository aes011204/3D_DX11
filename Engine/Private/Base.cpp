#include "Base.h"


static std::unordered_map<_wstring, uint32_t> s_NameCounters; 

CBase::CBase()
{

	m_Name = L"GameObject";
}

_wstring CBase::MakeDefaultNameFromTypeName(const char* rttiName)
{
    _string n = (rttiName ? rttiName : "");

    const char* prefixes[] = { "class ", "struct " };
    for (auto p : prefixes)
    {
        if (n.rfind(p, 0) == 0)
        {
            n.erase(0, std::strlen(p));
            break;
        }
    }

    // 2) 네임스페이스 제거 (Client::, Engine:: 등)
    size_t pos = n.rfind("::");
    if (pos != _string::npos)
        n = n.substr(pos + 2);

    // 3) 앞에 C 제거 (CPlayer -> Player)
    if (!n.empty() && n[0] == 'C')
        n.erase(0, 1);

    // 4) string -> wstring (ASCII 가정)
    return _wstring(n.begin(), n.end());
}

_wstring CBase::MakeUniqueName(const _wstring& baseName)
{
    auto& counter = s_NameCounters[baseName];
    if (counter == 0)
    {
        counter = 1;
        return baseName;
    }

    _wstring result = baseName + L" (" + std::to_wstring(counter) + L")";
    counter++;
    return result;
}

void CBase::SetDefaultNameFromThisType()
{
    _wstring baseName = MakeDefaultNameFromTypeName(typeid(*this).name());
    if (baseName.empty())
        baseName = L"Object";

    m_Name = MakeUniqueName(baseName);
}
//unsigned int CBase::AddRef()
//{
//    return ++m_iRefCnt;
//}
//
//unsigned int CBase::Release()
//{
//    if (m_iRefCnt == 0)
//    {
//        Free();
//
//        delete  this;
//        
//        return 0; // return m_iRefCnt--; 하면 터짐 위에서 객체를 지웠기 땜
//    }
//    else
//        return m_iRefCnt--;
//
//}
