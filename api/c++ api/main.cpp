#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <map>
#include <optional>
#include "include/Database.hpp"
#include "include/TcpListener.hpp"

std::tuple<Core::Net::ResponseType, std::string, std::optional<std::vector<std::string>>> testFunc(Core::Net::Request& req) {
	const std::string response = 
R"({
	"test": "test"
})";

	return std::make_tuple(Core::Net::ResponseType::JSON, response, std::optional<std::vector<std::string>>(false));
}

int main()
{
	//std::ios::sync_with_stdio(false);
	std::cout.tie(0);
	std::cin.tie(0);

	try {

		Core::Database::Database db;
		
		auto res = db.Query("SELECT id, username FROM users");

		for (const auto& row : res) {
			std::cout << row["id"].as<std::string>() << " " << row["username"].as<std::string>() << "\n";
		}

		Core::Net::Router router;
		Core::Net::TcpListener server;
		server.CreateSocket();


		auto onReceive = [&router](Core::Net::Request& req) {
			router.Handle(req);
		};

		server.SetOnReceive(onReceive);

		router.AddRoute("GET", "/malinki", testFunc);

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
