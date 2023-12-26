#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <map>
#include "include/TcpListener.hpp"


int main()
{
	std::ios::sync_with_stdio(false);
	std::cout.tie(0);
	std::cin.tie(0);

	try {
		Core::Net::TcpListener server;
		server.CreateSocket();

		char* szHostName = new char;
		gethostname(szHostName, 255);
		hostent* host_entry = gethostbyname(szHostName);
		char* szLocalIP = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);

		std::cout << "\033[1;34mAddr: " << szLocalIP << "\033[0m\n";
		server.Listen(szLocalIP, 8080, 250);
	}
	catch (std::runtime_error& e) {
		std::cout << "\033[1;31mError: " << e.what() << "\033[0m\n";
	}
}