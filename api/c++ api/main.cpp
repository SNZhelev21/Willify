#pragma once

#include "include/TcpListener.hpp"
#include "include/nlohmann json/json.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include "include/Backend/Users.hpp"
#include "include/Backend/Stocks.hpp"

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

		router.AddRoute("POST", "/register", Register);
		router.AddRoute("POST", "/login", Login);
		router.AddRoute("GET", "/user", GetUser);
		router.AddRoute("DELETE", "/user", DeleteUser);
		router.AddRoute("PUT", "/user", UpdateUser);
		router.AddRoute("GET", "/admin/user", AdminGetUser);
		router.AddRoute("GET", "/admin/users", AdminGetUsers);
		router.AddRoute("PUT", "/admin/user", AdminUpdateUser);
		router.AddRoute("DELETE", "/admin/user", AdminDeleteUser);
		router.AddRoute("GET", "/stock", GetStock);
		router.AddRoute("GET", "/stocks", GetStocks);
		router.AddRoute("POST", "/stock", CreateStock);
		router.AddRoute("PUT", "/stock", UpdateStock);
		router.AddRoute("DELETE", "/stock", DeleteStock);
		router.AddRoute("GET", "/admin/stock", AdminGetStock);
		router.AddRoute("GET", "/admin/stocks", AdminGetStocks);
		router.AddRoute("PUT", "/admin/stock", AdminUpdateStock);
		router.AddRoute("DELETE", "/admin/stock", AdminDeleteStock);

		char* szHostName = new char[255];
		gethostname(szHostName, 255);
		hostent* host_entry = gethostbyname(szHostName);
		char* szLocalIP = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);

		std::cout << "\033[1;34m[*] Addr: " << szLocalIP << "\033[0m\n";
		server.Listen(szLocalIP, 45098, 250);
	}
	catch (std::runtime_error& e) {
		std::cout << "\033[1;31m[-] Error: " << e.what() << "\033[0m\n";
	}
}