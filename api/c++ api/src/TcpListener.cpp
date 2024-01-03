#include "../include/TcpListener.hpp"

Core::Net::TcpListener::TcpListener()
{
	this->InitWSA();
}

Core::Net::TcpListener::~TcpListener()
{
	this->Close();
}

void Core::Net::TcpListener::InitWSA() {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "\033[31m[-] Failed to initialise WSA...\033[0m\n";
		std::cout << "\033[31m[-] Error code: " << WSAGetLastError() << "\033[0m\n";
		throw std::runtime_error("Failed to initialise WSA");
	}
	std::cout << "\033[1;32m[+] Initialised WSA\033[0m\n";
}

void Core::Net::TcpListener::CreateSocket() {
	this->listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->listener == INVALID_SOCKET) {
		std::cout << "\033[31m[-] Failed to create socket...\033[0m\n";
		std::cout << "\033[31m[-] Error code: " << WSAGetLastError() << "\033[0m\n";
		throw std::runtime_error("Failed to create socket");
	}

	std::cout << "\033[1;32m[+] Created socket\033[0m\n";
}

void Core::Net::TcpListener::Listen(const char* ip, uint_fast16_t port, uint_fast8_t maxConnections) {
	// Bind socket
	this->server.sin_family = AF_INET;
	this->server.sin_addr.S_un.S_addr = inet_addr(ip);
	this->server.sin_port = htons(port);
	this->serverLen = sizeof(this->server);
	if (bind(this->listener, (SOCKADDR*)& this->server, this->serverLen) != 0) {
		std::cout << "\033[31m[-] Failed to bind socket...\033[0m\n";
		std::cout << "\033[31m[-] Error code: " << WSAGetLastError() << "\033[0m\n";
		throw std::runtime_error("Failed to bind socket");
	}
	std::cout << "\033[1;32m[+] Bound socket\033[0m\n";

	// Listen
	int backlog = 20;
	if (listen(this->listener, backlog) != 0) {
		std::cout << "\033[31m[-] Failed to listen...\033[0m\n";
		std::cout << "\033[31m[-] Error code: " << WSAGetLastError() << "\033[0m\n";

		throw std::runtime_error("Failed to listen");
	}
	std::cout << "\033[1;32m[+] Started listening on " << ip << ':' << port << " with " << (int)maxConnections << " max connections\033[0m\n";

	// Accept
	while (true) {
		try {
			this->Accept();
		}
		catch (std::runtime_error& e) {
			std::cout << "\033[31m[-] Error: " << e.what() << "\033[0m\n";
		}
	}
	this->Close();
}

void Core::Net::TcpListener::Close()
{
	closesocket(this->listener);
	WSACleanup();
}

void Core::Net::TcpListener::SetOnConnect(std::function<void(SOCKET)> callback) {
	this->onConnect.Attach(callback);
}

void Core::Net::TcpListener::SetOnDisconnect(std::function<void(SOCKET)> callback) {
	this->onDisconnect.Attach(callback);
}

void Core::Net::TcpListener::SetOnReceive(std::function<void(Request&)> callback) {
	this->onReceive.Attach(callback);
}

void Core::Net::TcpListener::SetBlocking(bool blocking)
{
	u_long mode = blocking ? 0 : 1;
	ioctlsocket(this->listener, FIONBIO, &mode);
}

void Core::Net::TcpListener::Accept()
{
	Request req = Request();
	while (true) {
		this->newConnection = accept(listener, (SOCKADDR*)&this->server, &this->serverLen);
		if (this->newConnection == INVALID_SOCKET) {
			std::cout << "\033[31m[-] Failed to accept new connection...\033[0m\n";
			std::cout << "\033[31m[-] Error code: " << WSAGetLastError() << "\033[0m\n";

			closesocket(this->newConnection);

			throw std::runtime_error("Failed to accept new connection");
		}

		std::cout << "\033[1;32m[+] Accepted new connection...\033[0m\n";
		std::cout << "\033[1;32m[+] Invoking onConnect...\033[0m\n";

		this->onConnect.Invoke(this->newConnection);

		char* buffer = new char[4096];

		memset(buffer, 0, 4096);

 		if (recv(this->newConnection, buffer, 4096, 0) < 0) {
			std::cout << "\033[31m[-] Failed to read client request\033[0m\n";
			std::cout << "\033[31m[-] Error code: " << WSAGetLastError() << "\033[0m\n";

			closesocket(this->newConnection);
			delete[] buffer;

			throw std::runtime_error("Failed to read client request");
		}

		std::cout << "\033[1;32m[+] Received client request\033[0m\n";
		
		std::string data = "";
		data = std::string(buffer);

		if (data == "") {
			std::cout << "\033[31m[-] Failed to read client request\033[0m\n";
			std::cout << "\033[31m[-] Error code: " << WSAGetLastError() << "\033[0m\n";

			closesocket(this->newConnection);
			delete[] buffer;

			throw std::runtime_error("Failed to read client request");
		}

		#ifdef API_DEBUG
			std::cout << "\033[1;34m[*] Request data:\n";
			// split data by new line
			std::vector<std::string> split = Core::Net::Request::Split(data, '\n');
			for (int i = 0; i < split.size(); ++i) {
				std::cout << "	" << split[i] << '\n';
			}
			std::cout << "\033[0m";
		#endif

		req = Request(data, this->newConnection);
		this->onReceive.Invoke(req);

		this->onDisconnect.Invoke(this->newConnection);
		closesocket(this->newConnection);

		delete[] buffer;
	}
}