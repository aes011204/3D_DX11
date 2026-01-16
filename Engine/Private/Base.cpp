#include "Base.h"

CBase::CBase()
{
}

unsigned int CBase::AddRef()
{
    return ++m_iRefCnt;
}

unsigned int CBase::Release()
{
    if (m_iRefCnt == 0)
    {
        Free();

        delete  this;
        
        return 0; // return m_iRefCnt--; ÇÏ¸é ÅÍÁü À§¿¡¼­ °´Ã¼¸¦ Áö¿ü±â ¶«
    }
    else
        return m_iRefCnt--;

}
