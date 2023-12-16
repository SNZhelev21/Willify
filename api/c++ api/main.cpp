#pragma once
#include <iostream>
#include "include/tcp_listener.hpp"

void onReceive(std::string data, SOCKET connection) {
	std::cout << "Received data: " << data << '\n';
	
	std::string message =
		"HTTP/1.1 200 OK"
		"Content-Type: text/html"
		"Content-Length: ";
	std::string response =
		"<html>"
		"	<body>"
		"		<h1>Hello World<h1>"
		"	<body>"
		"</html>";

	message.append(std::to_string(sizeof(response)));
	message.append("\n\n");
	message.append(response);

	int bytesSent = 0;
	int totalBytesSent = 0;
	while (totalBytesSent < message.size()) {
		bytesSent = send(connection, message.c_str(), message.size(), 0);
		if (bytesSent < 0) {
			std::cout << "\033[31mFailed to send message...\033[0m\n";
			std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";
		}

		totalBytesSent += bytesSent;
	}

	std::cout << "\033[1;32mSent response to client\033[0m\n";
}

int main()
{
	net::TcpListener server;
	server.CreateSocket();
	server.SetOnReceive(onReceive);
	server.Listen("127.0.0.1", 8080, 255);
}