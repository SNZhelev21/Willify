#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <map>
#include "include/TcpListener.hpp"

void testFunc(Core::Net::Request& req) {
	std::cout << req << "\n";

	for (const auto& [key, val] : req.m_info.headers) {
		std::cout << "\033[1;34mTEST " << key << ": " << val << "\033[0m\n";
	}

	std::cout << "\033[1;34mRoute: " << req.m_info.route << "\033[0m\n";
	std::string message =
		"HTTP/1.1 200 OK\n"
		"Content-Type: application/json\n"
		"Content-Length: ";
	const std::string response = R"(
{
	"message": "malinki"
}	
	)";

	message.append(std::to_string(response.size()));
	message.append("\n\n");
	message.append(response);

	std::cout << "\033[1;34mResponse: \n" << message << "\033[0m\n";

	int bytesSent = 0;
	int totalBytesSent = 0;
	while (totalBytesSent < message.size()) {
		bytesSent = send(req.m_info.sender, message.data(), message.size(), 0);
		if (bytesSent < 0) {
			std::cout << "\033[31mFailed to send message...\033[0m\n";
			std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";
		}

		totalBytesSent += bytesSent;
	}
	
}

int main()
{
	//std::ios::sync_with_stdio(false);
	std::cout.tie(0);
	std::cin.tie(0);

	try {
		Core::Net::Router router;
		Core::Net::TcpListener server;
		server.CreateSocket();

		auto onReceive = [&router](Core::Net::Request& req) {
			router.Handle(req);
		};

		server.SetOnReceive(onReceive);

		router.AddRoute("GET", "/malinki", testFunc);

		router.AddRoute("GET", "/kapini", [](Core::Net::Request& req) {
			std::cout << req.m_info.route;
		});

		char* szHostName = new char[255];
		gethostname(szHostName, 255);
		hostent* host_entry = gethostbyname(szHostName);
		char* szLocalIP = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);

		std::cout << "\033[1;34mAddr: " << szLocalIP << "\033[0m\n";
		server.Listen(szLocalIP, 45098, 250);
	}
	catch (std::runtime_error& e) {
		std::cout << "\033[1;31mError: " << e.what() << "\033[0m\n";
	}
}
