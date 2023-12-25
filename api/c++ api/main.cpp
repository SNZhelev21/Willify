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

	Core::Net::TcpListener server;
	server.CreateSocket();
	server.Listen("192.168.1.18", 8080, 250);
}