#pragma once
#include "Event.hpp"
#include "Router.hpp"
#include <iostream>
#include <functional>
#include <thread>
#include <stdexcept>
#include <string>

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

namespace Core {
	namespace Net {
		// Server class
		class TcpListener {
			public:
				TcpListener();
				~TcpListener();

				void CreateSocket();

				void Listen(const char* ip, uint_fast16_t port, uint_fast8_t maxConnections);
				void Close();

				void SetOnConnect(std::function<void(SOCKET)> callback);

				void SetOnDisconnect(std::function<void(SOCKET)> callback);
				
				void SetOnReceive(std::function<void(Request&)> callback);
				
				void SetBlocking(bool blocking);


			private:
				SOCKET listener = INVALID_SOCKET;
				SOCKET newConnection = INVALID_SOCKET;
				WSADATA wsaData;
				sockaddr_in server;

				//Request req = Request();

				int serverLen;
				int bufferSize;

				Event<void, SOCKET> onConnect;
				Event<void, SOCKET> onDisconnect;
				Event<void, Request&> onReceive;

				void Accept();
				void InitWSA();
		};
	}
}