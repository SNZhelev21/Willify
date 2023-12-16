#pragma once
#include "event.hpp"
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

namespace net {
	// Server class
	class TcpListener {
		public:
			TcpListener();
			~TcpListener();

			void CreateSocket();

			void Listen(const char* ip, uint_fast16_t port, uint_fast8_t maxConnections);
			void Close();

			void SetOnConnect(std::function<void()> callback);
			void SetOnDisconnect(std::function<void()> callback);
			void SetOnReceive(std::function<void(std::string, SOCKET)> callback);
			void SetBlocking(bool blocking);
			void Accept();


		private:
			SOCKET listener = INVALID_SOCKET;
			SOCKET newConnection = INVALID_SOCKET;
			WSADATA wsaData;
			sockaddr_in server;
			int serverLen;
			int bufferSize;

			Event onConnect;
			Event onDisconnect;
			Event onReceive;

			void InitWSA();
	};
}