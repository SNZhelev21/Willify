#pragma once

#include "include/TcpListener.hpp"
#include "include/Database.hpp"
#include "include/nlohmann json/json.hpp"
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
#include <jwt-cpp/traits/nlohmann-json/traits.h>

using httpReturn = std::tuple<Core::Net::ResponseType, std::string, std::optional<std::vector<std::string>>>;
using json = nlohmann::json;

static std::string const secret = "v6j9V4BfSx2GrdkqSaj8KlD1ujT3JzAhmkfgPSoHYkvYKdjomBYrbhq2WKH78bzVY3syflrljlAkE6F9GuPzbwp6tURXeEwBl0gIsMwixH57crfaW2acA3noMPnTkurdJy86LPB6ILiUFvrYsPT90zXycrZJE6ZXXzDmTS7J3lFqEsNQMqUgIns46KJPptNzhLiLE9jOuQ1cgLtYsrHyaS5umk5feQg34PZ4AV8RcJYbBHKGSSeWyxPzJHL02FY6Isgg7HFJPlwcDZsIpFD5utDA5mride1goSnWyelGehleKt4lh9h37psK2hmUJWn9Dr8DypICVkFKthUHfaegig2WwcEsA17TWWo5LTXeHdGPfVDXMNzr5HM5rIJgXQsR";
static std::string const rsaSecret = R"(-----BEGIN RSA PRIVATE KEY-----
MIICXQIBAAKBgQCmq4p7kuM9EgA19vaTfv+4nyylr0mi28aUiZV2ueUBu3alEyy9
tWXBVlvpUJQsUgXPlitz0oYQGwRRjH9/A+caeXHSlVl0EmAOrTKxK2ElXlZizg/Z
sDgWHKtJXFviZYmc7iZLlinD7/7xG+J7WzRHqBE7jQ/ksHCUzW1TkgR3bwIDAQAB
AoGANIhQA0gWgKq6T0gSiwXV8yqUh3p64q6T5kCBUCUilcuxBzHGgdr5ghlGqgDg
WyHVPAtQkdaSs+PYFh4LF40y22aSs9z8X2miJQ2qoNOuiw0/u/ur8Xk9oC4lVQ5B
phZLv3duGq8WIruos+b8Z6JaEoGdqMC8XkPV4bk5kwPIFGUCQQDU+N4J6GUUyIhp
LiTxpsyEOER9D/bfbpd0szu+ZlMaO4DjbpC68ifsRVQbiA2TSUGvXPebyFHNR0K8
0lKy4F1DAkEAyFfigqCM77RhNMjGhOzb3PCTpm7VkjgEqqO4oS3NT3V5J+pJWdQN
CwNW8k7VYO8rrOrwiaCrHqWcdbrxotDkZQJBAL07xE8ZZ3doF40dq0Xs55w0Qua8
4Kb3JSxUdalgUkbkyElNXNw/fqm5kFpGMYnFdc/T009gVw8HWo1cdI7sPPECQQCY
ghZi/BbtbnHE/cXpcGlaQiUTwDGCUnRy9ZjZ0YUGJNXq5bWa1QpsxMkOQaa7WLX1
tw8Opak99jIpiPrix4PlAkBjLP3KvV0BzUgPP3P1Tlj3hJDQJxKZxmrpZKRJ/GA2
bOkXOU193F+nMGkcpe+QPZvLa2ROfNpz35KQ1nB3tTyR
-----END RSA PRIVATE KEY-----)";

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

	query = "SELECT last_value FROM users_id_seq";

	res = Core::Database::database.Query(query);

	std::optional<long long int> temp = res[0]["last_value"].as<std::optional<long long int>>();
	long long int id = (temp.value_or(0) + 1) * 52834;

	password = email + username + password + std::to_string(id) + secret;

	std::string hashedSalted = Hash(password);

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

	auto jwtToken = jwt::create<jwt::traits::nlohmann_json>()
		.set_issuer("auth0")
		.set_type("JWT")
		.set_payload_claim("id", std::to_string(id))
		.set_payload_claim("username", username)
		.set_payload_claim("email", email)
		.set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{ 3600 })
		.set_issued_at(std::chrono::system_clock::now());

	std::string signedToken = jwtToken.sign(jwt::algorithm::rs512{"", rsaSecret, "", ""});

	return std::make_tuple(Core::Net::ResponseType::OK, signedToken, std::optional<std::vector<std::string>>(false));
}

httpReturn GetUser(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing token", std::optional<std::vector<std::string>>(false));
	}

	jwt::verifier<jwt::default_clock, jwt::traits::nlohmann_json> verifier = jwt::verify<jwt::traits::nlohmann_json>().allow_algorithm(jwt::algorithm::rs512{ "", rsaSecret, "", "" }).with_issuer("auth0");
	auto decodedToken = jwt::decode<jwt::traits::nlohmann_json>(token);

	std::error_code ec;
	verifier.verify(decodedToken, ec);

	if (ec) {
		std::cout << "\033[1;31m[-] Error: " << ec.message() << "\033[0m\n";
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, ec.message(), std::optional<std::vector<std::string>>(false));
	}

	json tokenJson = decodedToken.get_payload_json();
	std::string id = tokenJson["id"];

	if (id == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing id", std::optional<std::vector<std::string>>(false));
	}
	pqxx::result res = Core::Database::database.Query("SELECT * FROM users WHERE id = " + id + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "User with id " + id + " not found", std::optional<std::vector<std::string>>(false));
	}

	json user;
	user["id"] = res[0]["id"].as<int>();
	user["first_name"] = res[0]["first_name"].as<std::string>();
	user["last_name"] = res[0]["last_name"].as<std::string>();
	user["username"] = res[0]["username"].as<std::string>();
	user["email"] = res[0]["email"].as<std::string>();
	//user["egn"] = res[0]["egn"].as<std::string>();

	return std::make_tuple(Core::Net::ResponseType::OK, user.dump(), std::optional<std::vector<std::string>>(false));
}

httpReturn DeleteUser(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing token", std::optional<std::vector<std::string>>(false));
	}

	jwt::verifier<jwt::default_clock, jwt::traits::nlohmann_json> verifier = jwt::verify<jwt::traits::nlohmann_json>().allow_algorithm(jwt::algorithm::rs512{ "", rsaSecret, "", "" }).with_issuer("auth0");
	auto decodedToken = jwt::decode<jwt::traits::nlohmann_json>(token);

	std::error_code ec;
	verifier.verify(decodedToken, ec);

	if (ec) {
		std::cout << "\033[1;31m[-] Error: " << ec.message() << "\033[0m\n";
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, ec.message(), std::optional<std::vector<std::string>>(false));
	}

	json tokenJson = decodedToken.get_payload_json();
	std::string id = tokenJson["id"];

	if (id == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing id", std::optional<std::vector<std::string>>(false));
	}

	pqxx::result res = Core::Database::database.Query("DELETE FROM users WHERE id = " + id + ";");

	if (res.affected_rows() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "User with id " + id + " not found", std::optional<std::vector<std::string>>(false));
	}

	return std::make_tuple(Core::Net::ResponseType::OK, "", std::optional<std::vector<std::string>>(false));
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
		router.AddRoute("GET", "/user", GetUser);
		router.AddRoute("DELETE", "/user", DeleteUser);

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