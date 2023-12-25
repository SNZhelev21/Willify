#pragma once
#include "Event.hpp"
#include "Parser.hpp"
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

				template <typename Function>
				void SetOnConnect(Function callback);

				template <typename Function>
				void SetOnDisconnect(Function callback);
				
				template <typename Function>
				void SetOnReceive(Function callback);
				
				void SetBlocking(bool blocking);


			private:
				SOCKET listener = INVALID_SOCKET;
				SOCKET newConnection = INVALID_SOCKET;
				WSADATA wsaData;
				sockaddr_in server;
				int serverLen;
				int bufferSize;

				Event<SOCKET> onConnect;
				Event<SOCKET> onDisconnect;
				Event<std::string, SOCKET> onReceive;
				Parser parser;

				void Accept();
				void InitWSA();
		};
	}
}