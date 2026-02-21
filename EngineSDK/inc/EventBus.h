#pragma once
//#include "Base.h" // 베이스가 있어야 하는지 모르겠으니까 일단 패스
#include "Engine_Define.h"
#include <typeindex>

NS_BEGIN(Engine)

class CEventBus 
{
public:
	CEventBus();
	//CEventBus(const CEventBus&) = delete;
	//CEventBus& operator=(const CEventBus&) = delete;
	virtual ~CEventBus();

	using anyHandler = function<void(const void*)>;

	struct QueuedEvent { type_index m_TypeIndex; shared_ptr<void> data; };
public:
	template<typename EventT>
	void Subscribe (function<void(const EventT&)> func)
	{
		//map []로 추가만 가능함 : 없으면 새로 만든다는 의미
		m_handlers[typeid(EventT)].push_back(
			[func](const void* e) {func(*static_cast<const EventT*>(e));}
		);

		// 푸쉬백으로 람다를 넣어서 사용할떄 자동으로 자신의 구조체 형식으로 캐스팅되어 사용
	}

	template<typename EventT>
	void Publish(const EventT& e)
	{
		auto it = m_handlers.find(typeid(EventT));
		if (it == m_handlers.end())
		{
			//MSG_BOX("No Event in m_handler");
			return;
		}
			
		for(auto& handle :it->second)
		{
			handle(&e);
		}
	}

public:
	template<typename EventT>
	void EnQueue(const EventT& e)
	{
		m_EventQueue.push({ typeid(EventT),make_shared<EventT>(e) });
		// make_shared<EventT>(e) : 이벤트 복사본을 안전하게 큐에 보관
	}


	void pump()
	{
		while (!m_EventQueue.empty())
		{
			QueuedEvent event = m_EventQueue.front();
			auto it = m_handlers.find(event.m_TypeIndex);

			if (it != m_handlers.end())
			{
				for (auto& handle : it->second)
				{
					handle(event.data.get());
				}
			}
			m_EventQueue.pop();
		}
	}


private:

	unordered_map<type_index, vector<anyHandler>> m_handlers; // 이밴트들을 저장하는 저장소

	queue<QueuedEvent> m_EventQueue = {};

public:
	static unique_ptr<CEventBus> Create();
	void Free();
};



/* 사용하는 쪽

// 퍼블리쉬(신호주기)
	 // 바로사용
	 struct EvtUIOpen {
	  int uiId;
	  bool modal;
	 };
	 
	 EvtUIOpen ev = {};'// 생성
	 bus.Publish(ev); //bus.Publish(EvtUIOpen ev{}); 임시객체도 가능
	 //큐잉
	 bus.EnQueue(ev);
	 
// 구독
	 CSound
	 bus.Subscribe<EvtUIOpen>([this](EvtUIOpen& e){ this->PlaySound()})
	 sound의 수명이 EventBus보다 길어야 함

	 auto sound = make_shared<CSound>();이렇게 하면 안전함

 */

NS_END