#pragma once

#include "include/TcpListener.hpp"
#include "include/Database.hpp"
#include "include/json.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <chrono>
#include <sstream>
#include <optional>
#include <openssl/sha.h>
#undef max
#undef min
#include <jwt-cpp/jwt.h>

using httpReturn = std::tuple<Core::Net::ResponseType, std::string, std::optional<std::vector<std::string>>>;
using json = nlohmann::json;

static std::string const secret = "v6j9V4BfSx2GrdkqSaj8KlD1ujT3JzAhmkfgPSoHYkvYKdjomBYrbhq2WKH78bzVY3syflrljlAkE6F9GuPzbwp6tURXeEwBl0gIsMwixH57crfaW2acA3noMPnTkurdJy86LPB6ILiUFvrYsPT90zXycrZJE6ZXXzDmTS7J3lFqEsNQMqUgIns46KJPptNzhLiLE9jOuQ1cgLtYsrHyaS5umk5feQg34PZ4AV8RcJYbBHKGSSeWyxPzJHL02FY6Isgg7HFJPlwcDZsIpFD5utDA5mride1goSnWyelGehleKt4lh9h37psK2hmUJWn9Dr8DypICVkFKthUHfaegig2WwcEsA17TWWo5LTXeHdGPfVDXMNzr5HM5rIJgXQsR";

std::string Hash(std::string message) {
	unsigned char hash[SHA512_DIGEST_LENGTH];
	SHA512((const unsigned char*)message.c_str(), message.length(), hash);
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
		ss << std::setw(2) << static_cast<int>(hash[i]);

	return ss.str();
}

httpReturn Register(Core::Net::Request& req) {
	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string fName = body["first_name"];
	std::string lName = body["last_name"];
	std::string username = body["username"];
	std::string password = body["password"];
	std::string email = body["email"];

	//Validate data
	if (fName == "" || lName == "" || username == "" || password == "" || email == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing required fields", std::optional<std::vector<std::string>>(false));
	}

	if (fName.length() < 3 || lName.length() < 3 || username.length() < 3 || password.length() < 3 || email.length() < 3) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Fields must be more than 3 characters", std::optional<std::vector<std::string>>(false));
	}

	if (fName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "First name must only contain letters", std::optional<std::vector<std::string>>(false));
	}

	if (lName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Last name must only contain letters", std::optional<std::vector<std::string>>(false));
	}

	if (username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Username must only contain letters and numbers", std::optional<std::vector<std::string>>(false));
	}

	if (email.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@.") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must only contain letters, numbers, @ and .", std::optional<std::vector<std::string>>(false));
	}

	if (email.find_first_of("@") == std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must contain @", std::optional<std::vector<std::string>>(false));
	}

	if (email.find_first_of(".") == std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must contain .", std::optional<std::vector<std::string>>(false));
	}
	
	if (password.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!#$%^&*") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Password must only contain letters, numbers, !, #, $, %, ^, & and *", std::optional<std::vector<std::string>>(false));
	}

	std::string query = "SELECT * FROM users WHERE username = '" + username + "';";

	auto res = Core::Database::database.Query(query);

	if (res.size() > 0) {
		return std::make_tuple(Core::Net::ResponseType::ALREADY_EXISTS, "User with username " + username + " already exists", std::optional<std::vector<std::string>>(false));
	}

	query = "SELECT id FROM users ORDER BY id DESC LIMIT 1;";

	res = Core::Database::database.Query(query);

	std::optional<int> temp = res[0]["id"].as<std::optional<long long int>>();
	long long int id = 1 * 52834;
	if (temp.has_value()) {
		id = (temp.value() + 1) * 52834;
	}

	password = email + username + password + std::to_string(id) + secret;

	std::string hashedSalted = Hash(password);

	std::cout << "\033[1;34m[*] Password: " << hashedSalted << "\033[0m\n";

	query = "INSERT INTO users (id, first_name, last_name, username, email, password) VALUES (DEFAULT, '" + fName + "', '" + lName + "', '" + username + "', '" + email + "', '" + hashedSalted + "');";

	Core::Database::database.Query(query);

	return std::make_tuple(Core::Net::ResponseType::OK, "", std::optional<std::vector<std::string>>(false));
}

httpReturn Login(Core::Net::Request& req) {
	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	if (!body.contains("username") || !body.contains("password")) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing required fields", std::optional<std::vector<std::string>>(false));
	}

	std::string username = body["username"];
	std::string password = body["password"];

	std::string query = "SELECT id, password, email FROM users WHERE username = '" + username + "';";

	auto res = Core::Database::database.Query(query);

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "User with username " + username + " not found", std::optional<std::vector<std::string>>(false));
	}

	int id = res[0]["id"].as<int>();
	std::string dbPassword = res[0]["password"].as<std::string>();
	std::string email = res[0]["email"].as<std::string>();

	password = email + username + password + std::to_string(id * 52834) + secret;

	password = Hash(password);

	#ifdef API_DEBUG
		std::cout << "\033[1;34m[*] Password: " << password << "\033[0m\n";
		std::cout << "\033[1;34m[*] DB password: " << dbPassword << "\033[0m\n";
	#endif
	if (password != dbPassword) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Incorrect password", std::optional<std::vector<std::string>>(false));
	}

	std::string jwtToken = jwt::create()
		.set_issuer("auth0")
		.set_type("JWT")
		.set_payload_claim("id", jwt::claim(std::to_string(id)))
		.set_payload_claim("username", jwt::claim(username))
		.set_payload_claim("email", jwt::claim(email))
		.set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{ 3600 })
		.set_issued_at(std::chrono::system_clock::now())
		.sign(jwt::algorithm::hs512{ secret });

	return std::make_tuple(Core::Net::ResponseType::OK, jwtToken, std::optional<std::vector<std::string>>(false));
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

		router.AddRoute("POST", "/register", Register);
		router.AddRoute("POST", "/login", Login);

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