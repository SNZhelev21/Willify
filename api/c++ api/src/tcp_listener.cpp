#include "../include/tcp_listener.hpp"

net::TcpListener::TcpListener()
{
	this->InitWSA();
}

net::TcpListener::~TcpListener()
{
	this->Close();
}

void net::TcpListener::InitWSA() {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "\033[31mFailed to initialise WSA...\033[0m\n";
		std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";
		throw std::runtime_error("Failed to initialise WSA");
	}
	std::cout << "\033[1;32mInitialised WSA\033[0m\n";
}

void net::TcpListener::CreateSocket() {
	this->listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->listener == INVALID_SOCKET) {
		std::cout << "\033[31mFailed to create socket...\033[0m\n";
		std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";
		throw std::runtime_error("Failed to create socket");
	}

	std::cout << "\033[1;32mCreated socket\033[0m\n";
}

void net::TcpListener::Listen(const char* ip, uint_fast16_t port, uint_fast8_t maxConnections) {
	// Bind socket
	this->server.sin_family = AF_INET;
	this->server.sin_addr.S_un.S_addr = inet_addr(ip);
	this->server.sin_port = htons(port);
	this->serverLen = sizeof(this->server);
	if (bind(this->listener, (SOCKADDR*)& this->server, this->serverLen) != 0) {
		std::cout << "\033[31mFailed to bind socket...\033[0m\n";
		std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";
		throw std::runtime_error("Failed to bind socket");
	}
	std::cout << "\033[1;32mBound socket\033[0m\n";

	// Listen
	int backlog = 20;
	if (listen(this->listener, backlog) != 0) {
		std::cout << "\033[31mFailed to listen...\033[0m\n";
		std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";

		throw std::runtime_error("Failed to listen");
	}
	std::cout << "\033[1;32mStarted listening on " << ip << ':' << port << " with " << (int)maxConnections << " max connections\033[0m\n";

	// Accept
	while (true) {
		this->newConnection = accept(listener, (SOCKADDR*)&this->server, &this->serverLen);
		if (this->newConnection == INVALID_SOCKET) {
			std::cout << "\033[31mFailed to accept new connection...\033[0m\n";
			std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";

			throw std::runtime_error("Failed to accept new connection");
		}

		std::cout << "\033[1;32mAccepted new connection...\033[0m\n";
		std::cout << "\033[1;32mInvoking onConnect...\033[0m\n";

		this->onConnect.Invoke();

		std::vector<char> buffer(4096);
		if (recv(this->newConnection, &buffer[0], buffer.size(), 0) < 0) {
			std::cout << "\033[31mFailed to read client request\033[0m\n";
			std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";

			throw std::runtime_error("Failed to read client request");
		}
		std::string data(buffer.begin(), buffer.end());

		//std::cout << "\033[1;34mClient request: \033[0m\n";
		//std::cout << data << '\n';



		std::cout << "\033[1;32mInvoking onReceive...\033[0m\n";
		this->onReceive.Invoke(data, this->newConnection);

		std::cout << "\033[1;32mInvoking onDisconnect...\033[0m\n";
		this->onDisconnect.Invoke();
		closesocket(this->newConnection);
	}
}

void net::TcpListener::Close()
{
}

void net::TcpListener::SetOnConnect(std::function<void()> callback) {
	this->onConnect.Attach(callback);
}

void net::TcpListener::SetOnDisconnect(std::function<void()> callback) {
	this->onDisconnect.Attach(callback);
}

void net::TcpListener::SetOnReceive(std::function<void(std::string, SOCKET)> callback) {
	this->onReceive.Attach(callback);
}

void net::TcpListener::SetBlocking(bool blocking)
{
}

void net::TcpListener::Accept()
{
}