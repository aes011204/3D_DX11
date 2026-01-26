#pragma once
//#include "Base.h" // 베이스가 있어야 하는지 모르겠으니까 일단 패스
#include "Engine_Define.h"
#include <typeindex>
class CEventBus 
{
	using anyHandler = function<void(const void*)>;

public:
	template<typename T>
	void Subscribe (function<void(const T&)> function)
	{
		//map []로 추가만 가능함 : 없으면 새로 만든다는 의미
		m_handlers[typeid(T)].push_back(
			[function](const void* e) {fn(*static_cast<const T*>(e));}
		);

		// 푸쉬백으로 람다를 넣어서 사용할떄 자동으로 자신의 구조체 형식으로 캐스팅되어 사용
	}

	template<typename T>
	void Publish(T* e)
	{
		auto it = m_handlers.find(typeid(e));
		if (it == m_handlers.end())
		{
			MSG_BOX("No Event in m_handler");
			return;
		}
			
		for(auto& handle :(*it).second())
		{
			handle(e);
		}
	}

private:

	unordered_map<type_index, vector<anyHandler>> m_handlers;
};

