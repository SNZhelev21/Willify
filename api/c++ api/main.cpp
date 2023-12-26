#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <map>
#include "include/TcpListener.hpp"

void testFunc(std::string req, SOCKET conn) {
	std::string route = Core::Net::Parser::GetRoute(req);

	if (route == "/malinki") {
		std::cout << "\033[1;34mRoute: " << route << "\033[0m\n";
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
			bytesSent = send(conn, message.data(), message.size(), 0);
			if (bytesSent < 0) {
				std::cout << "\033[31mFailed to send message...\033[0m\n";
				std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";
			}

			totalBytesSent += bytesSent;
		}
	}
}

void testFunc2(std::string req, SOCKET conn) {
	std::string route = Core::Net::Parser::GetRoute(req);

	if (route == "/borovinki") {
		std::cout << "\033[1;34mRoute: " << route << "\033[0m\n";
		std::string message =
			"HTTP/1.1 200 OK\n"
			"Content-Type: application/json\n"
			"Content-Length: ";
		const std::string response = R"(
{
	"message": "borovinki"
}	
		)";

		message.append(std::to_string(response.size()));
		message.append("\n\n");
		message.append(response);

		std::cout << "\033[1;34mResponse: \n" << message << "\033[0m\n";

		int bytesSent = 0;
		int totalBytesSent = 0;
		while (totalBytesSent < message.size()) {
			bytesSent = send(conn, message.data(), message.size(), 0);
			if (bytesSent < 0) {
				std::cout << "\033[31mFailed to send message...\033[0m\n";
				std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";
			}

			totalBytesSent += bytesSent;
		}
	}
}

int main()
{
	using Parser = Core::Net::Parser;

	std::ios::sync_with_stdio(false);
	std::cout.tie(0);
	std::cin.tie(0);

	try {
		Core::Net::TcpListener server;
		server.CreateSocket();

		char* szHostName = new char[255];
		gethostname(szHostName, 255);
		hostent* host_entry = gethostbyname(szHostName);
		char* szLocalIP = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);

		Parser::onGet.Attach(testFunc);
		Parser::onGet.Attach(testFunc2);

		std::cout << "\033[1;34mAddr: " << szLocalIP << "\033[0m\n";
		server.Listen("192.168.1.20", 45098, 250);
	}
	catch (std::runtime_error& e) {
		std::cout << "\033[1;31mError: " << e.what() << "\033[0m\n";
	}
}