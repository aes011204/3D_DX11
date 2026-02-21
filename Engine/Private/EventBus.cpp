#include "EventBus.h"


CEventBus::CEventBus() 
{

}

CEventBus::~CEventBus()
{
    Free();
}

unique_ptr<CEventBus> CEventBus::Create()
{
    return unique_ptr<CEventBus>(new CEventBus());
}

void CEventBus::Free()
{
    
}
