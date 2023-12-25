#pragma once
#include <vector>
#include <functional>
#include <string>

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

//struct Function {
//	std::function<void(std::string, SOCKET)> dataFunction;
//	std::function<void()> defaultFunction;
//};

template <typename... Args>
class Event {
public:
	using FuncType = std::function<void(Args...)>;

	template <typename Function>
	void Attach(Function func) {
		this->m_Listeners.push_back(func);
	}

	void Invoke(Args... args) {
		for (const auto& func : this->m_Listeners) {
			func(args...);
		}
	}

	void DetachAll() {
		this->m_Listeners.clear();
	}

private:
	std::vector<FuncType> m_Listeners;
};

//template <>
//class Event<> {
//public:
//	using FuncType = std::function<void()>;
//
//	template <typename Function>
//	Event* Attach(Function func);
//
//	void Invoke();
//
//	void DetachAll();
//
//private:
//	std::vector<FuncType> m_Listeners;
//};