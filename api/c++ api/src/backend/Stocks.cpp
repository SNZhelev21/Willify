#include "../../include/backend/Stocks.hpp"

httpReturn CreateStock(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::optional<std::vector<std::string>>(false));
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

	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string name = body["name"];

	// Validate name
	if (name == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing stock name", std::optional<std::vector<std::string>>(false));
	}

	if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Stock name can only contain letters, numbers, underscores and spaces", std::optional<std::vector<std::string>>(false));
	}

	pqxx::result res = Core::Database::database.Query("INSERT INTO stocks (name, owner_id) VALUES ('" + name + "', '" + id + "') RETURNING id");

	json stock;
	stock["id"] = res[0]["id"].as<std::string>();
	stock["name"] = name;
	stock["owner_id"] = id;

	return std::make_tuple(Core::Net::ResponseType::CREATED, stock.dump(), std::nullopt);
}

httpReturn GetStocks(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::optional<std::vector<std::string>>(false));
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

	pqxx::result res = Core::Database::database.Query("SELECT * FROM stocks WHERE owner_id = '" + id + "'");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "No stocks found", std::optional<std::vector<std::string>>(false));
	}

	json stocks;

	for (int i = 0; i < res.size(); i++) {
		json stock;
		stock["id"] = res[i]["id"].as<std::string>();
		stock["name"] = res[i]["name"].as<std::string>();
		stock["owner_id"] = res[i]["owner_id"].as<std::string>();
		stocks.push_back(stock);
	}

	return std::make_tuple(Core::Net::ResponseType::OK, stocks.dump(), std::nullopt);
}

httpReturn AdminGetStocks(Core::Net::Request& req) {
	std::string token = Core::Net::Request::GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::optional<std::vector<std::string>>(false));
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

	if (tokenJson["role"].get<std::string>() != "admin") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::optional<std::vector<std::string>>(false));
	}

	json body;

	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string ownerId = body["owner_id"];

	pqxx::result res = Core::Database::database.Query("SELECT * FROM stocks WHERE owner_id = '" + ownerId + "'");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "No stocks found", std::optional<std::vector<std::string>>(false));
	}

	json stocks;

	for (int i = 0; i < res.size(); i++) {
		json stock;
		stock["id"] = res[i]["id"].as<std::string>();
		stock["name"] = res[i]["name"].as<std::string>();
		stock["owner_id"] = res[i]["owner_id"].as<std::string>();
		stocks.push_back(stock);
	}

	return std::make_tuple(Core::Net::ResponseType::OK, stocks.dump(), std::nullopt);
}

httpReturn GetStock(Core::Net::Request& req) {
	std::string token = Core::Net::Request::GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::optional<std::vector<std::string>>(false));
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

	json body;

	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string stockId = body["id"];

	if (stockId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing stock id", std::optional<std::vector<std::string>>(false));
	}

	pqxx::result res = Core::Database::database.Query("SELECT * FROM stocks WHERE id = '" + stockId + "'");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "Stock not found", std::optional<std::vector<std::string>>(false));
	}

	json stock;
	stock["id"] = res[0]["id"].as<std::string>();
	stock["name"] = res[0]["name"].as<std::string>();
	stock["owner_id"] = res[0]["owner_id"].as<std::string>();

	return std::make_tuple(Core::Net::ResponseType::OK, stock.dump(), std::nullopt);
}

httpReturn AdminGetStock(Core::Net::Request& req) {
	std::string token = Core::Net::Request::GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::optional<std::vector<std::string>>(false));
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

	if (tokenJson["role"].get<std::string>() != "admin") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::optional<std::vector<std::string>>(false));
	}

	json body;
	
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string stockId = body["stock_id"];

	if (stockId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing stock id", std::optional<std::vector<std::string>>(false));
	}

	pqxx::result res = Core::Database::database.Query("SELECT * FROM stocks WHERE id = '" + stockId + "'");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "Stock not found", std::optional<std::vector<std::string>>(false));
	}

	json stock;
	stock["id"] = res[0]["id"].as<std::string>();
	stock["name"] = res[0]["name"].as<std::string>();
	stock["owner_id"] = res[0]["owner_id"].as<std::string>();

	return std::make_tuple(Core::Net::ResponseType::OK, stock.dump(), std::nullopt);
}

httpReturn UpdateStock(Core::Net::Request& req) {
	std::string token = Core::Net::Request::GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::optional<std::vector<std::string>>(false));
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

	json body;

	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string stockId = body["stock_id"];
	std::string name = body["name"];

	if (stockId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing stock id", std::optional<std::vector<std::string>>(false));
	}

	if (name == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing stock name", std::optional<std::vector<std::string>>(false));
	}

	if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Stock name can only contain letters, numbers, underscores and spaces", std::optional<std::vector<std::string>>(false));
	}

	pqxx::result res = Core::Database::database.Query("SELECT * FROM stocks WHERE id = '" + stockId + "'");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "Stock not found", std::optional<std::vector<std::string>>(false));
	}

	if (res[0]["owner_id"].as<std::string>() != id) {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "You do not own this stock", std::optional<std::vector<std::string>>(false));
	}

	Core::Database::database.Query("UPDATE stocks SET name = '" + name + "' WHERE id = '" + stockId + "'");

	json stock;
	stock["id"] = stockId;
	stock["name"] = name;
	stock["owner_id"] = id;

	return std::make_tuple(Core::Net::ResponseType::OK, stock.dump(), std::nullopt);
}

httpReturn AdminUpdateStock(Core::Net::Request& req) {
	std::string token = Core::Net::Request::GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::optional<std::vector<std::string>>(false));
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

	if (tokenJson["role"].get<std::string>() != "admin") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::optional<std::vector<std::string>>(false));
	}

	json body;

	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string ownerId = body["owner_id"];
	std::string stockId = body["stock_id"];
	std::string name = body["name"];

	if (ownerId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing owner id", std::optional<std::vector<std::string>>(false));
	}

	if (stockId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing stock id", std::optional<std::vector<std::string>>(false));
	}

	if (name == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing stock name", std::optional<std::vector<std::string>>(false));
	}

	if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Stock name can only contain letters, numbers, underscores and spaces", std::optional<std::vector<std::string>>(false));
	}

	pqxx::result res = Core::Database::database.Query("SELECT * FROM stocks WHERE id = '" + stockId + "'");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "Stock not found", std::optional<std::vector<std::string>>(false));
	}

	Core::Database::database.Query("UPDATE stocks SET name = '" + name + "', owner_id = '" + ownerId + "' WHERE id = '" + stockId + "'");

	json stock;
	stock["id"] = stockId;
	stock["name"] = name;
	stock["owner_id"] = ownerId;

	return std::make_tuple(Core::Net::ResponseType::OK, stock.dump(), std::nullopt);
}

httpReturn DeleteStock(Core::Net::Request& req) {
	std::string token = Core::Net::Request::GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::optional<std::vector<std::string>>(false));
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

	json body;

	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string stockId = body["stock_id"];

	if (stockId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing stock id", std::optional<std::vector<std::string>>(false));
	}

	pqxx::result res = Core::Database::database.Query("SELECT * FROM stocks WHERE id = '" + stockId + "'");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "Stock not found", std::optional<std::vector<std::string>>(false));
	}

	if (res[0]["owner_id"].as<std::string>() != id) {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "You do not own this stock", std::optional<std::vector<std::string>>(false));
	}

	return std::make_tuple(Core::Net::ResponseType::OK, "Stock deleted", std::nullopt);
}

httpReturn AdminDeleteStock(Core::Net::Request& req) {
	std::string token = Core::Net::Request::GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::optional<std::vector<std::string>>(false));
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

	if (tokenJson["role"].get<std::string>() != "admin") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::optional<std::vector<std::string>>(false));
	}

	json body;

	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string stockId = body["stock_id"];

	if (stockId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing stock id", std::optional<std::vector<std::string>>(false));
	}

	pqxx::result res = Core::Database::database.Query("DELETE FROM stocks WHERE id = " + id + ";");

	if (res.affected_rows() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "Stock with id " + id + " not found", std::nullopt);
	}

	return std::make_tuple(Core::Net::ResponseType::OK, "Stock deleted", std::nullopt);
}