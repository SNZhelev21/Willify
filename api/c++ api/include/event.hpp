#pragma once
#include <vector>
#include <functional>
#include <string>

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

struct Function {
	std::function<void(std::string, SOCKET)> dataFunction;
	std::function<void()> defaultFunction;
};

class Event {
public:
	template <typename... Args>
	void operator+=(std::function<void(Args...)> callback);
	void operator+=(std::function<void()> callback);


	void operator()(std::string data, SOCKET sock);
	void operator()();


	//template <typename... Args>
	Event* Attach(std::function<void(std::string, SOCKET)> callback);
	Event* Attach(std::function<void()> callback);

	void DetachAll();

	void Invoke(std::string data, SOCKET sock);
	//template <typename... Args>
	void Invoke();

private:
	std::vector<Function> m_Listeners;
};