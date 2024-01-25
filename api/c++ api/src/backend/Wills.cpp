#include "../../include/backend/Wills.hpp"

httpReturn CreateWill(Core::Net::Request& req)
{
	std::string token = Core::Net::Request::GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::nullopt);
	}

	jwt::verifier<jwt::default_clock, jwt::traits::nlohmann_json> verifier = jwt::verify<jwt::traits::nlohmann_json>().allow_algorithm(jwt::algorithm::rs512{ "", rsaSecret, "", "" }).with_issuer("auth0");
	auto decodedToken = jwt::decode<jwt::traits::nlohmann_json>(token);

	std::error_code ec;
	verifier.verify(decodedToken, ec);

	if (ec) {
		std::cout << "\033[1;31m[-] Error: " << ec.message() << "\033[0m\n";
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, ec.message(), std::nullopt);
	}

	json tokenJson = decodedToken.get_payload_json();
	std::string id = tokenJson["id"];

	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::nullopt);
	}

	std::string stockId = body["stock_id"];
	std::string beneficiaryName = body["beneficiary_name"];
	std::string quantity = body["quantity"];
	std::string beneficiaryRelation = body["beneficiary_relation"];

	if (stockId == "" || beneficiaryName == "" || quantity == "" || beneficiaryRelation == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing fields", std::nullopt);
	}

	// Validate data
	if (stockId.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid stock id", std::nullopt);
	}

	if (beneficiaryName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid beneficiary name", std::nullopt);
	}

	if (beneficiaryRelation.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid beneficiary relation", std::nullopt);
	}

	if (quantity.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid quantity", std::nullopt);
	}

	// Check if stock exists
	pqxx::result res = Core::Database::database.Query("SELECT id FROM stocks WHERE id = " + stockId + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Stock does not exist", std::nullopt);
	}

	// Check if user has stock
	res = Core::Database::database.Query("SELECT id FROM stocks WHERE id = " + stockId + " AND owner_id = '" + id + "';");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "You do not own this stock", std::nullopt);
	}

	res = Core::Database::database.Query("SELECT quantity FROM wills WHERE stock_id = " + stockId + ";");

	int totalQuantity = 0;

	for (auto row : res) {
		totalQuantity += row["quantity"].as<int>();
	}

	totalQuantity += std::stoi(quantity);

	res = Core::Database::database.Query("SELECT quantity FROM stocks WHERE id = " + stockId + ";");

	int stockQuantity = res[0]["quantity"].as<int>();

	if (totalQuantity > stockQuantity) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Not enough asset quantity", std::nullopt);
	}

	res = Core::Database::database.Query("SELECT id FROM wills WHERE stock_id = " + stockId + " AND beneficiary_name = '" + beneficiaryName + "' AND beneficiary_relation = '" + beneficiaryRelation + "'; ");

	if (res.size() != 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Will already exists", std::nullopt);
	}

	res = Core::Database::database.Query("INSERT INTO wills (stock_id, beneficiary_name, beneficiary_relation, quantity) VALUES (" + stockId + ", '" + beneficiaryName + "', '" + beneficiaryRelation + "', " + quantity + ") RETURNING id, created_at; ");

	if (res.affected_rows() == 0) {
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, "Failed to create will", std::nullopt);
	}

	json willJson;
	willJson["id"] = res[0]["id"].as<std::string>();
	willJson["stock_id"] = stockId;
	willJson["beneficiary_name"] = beneficiaryName;
	willJson["beneficiary_relation"] = beneficiaryRelation;
	willJson["done_at"] = "";
	willJson["created_at"] = res[0]["created_at"].as<std::string>();
	willJson["quantity"] = quantity;

	return std::make_tuple(Core::Net::ResponseType::JSON, willJson.dump(), std::nullopt);
}

httpReturn GetWills(Core::Net::Request& req) {
	std::string token = Core::Net::Request::GetHeader(req.m_info.original, "Authorization");

	// Remove Bearer
	token.erase(0, 7);

	if (token == "") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Missing token", std::nullopt);
	}

	jwt::verifier<jwt::default_clock, jwt::traits::nlohmann_json> verifier = jwt::verify<jwt::traits::nlohmann_json>().allow_algorithm(jwt::algorithm::rs512{ "", rsaSecret, "", "" }).with_issuer("auth0");
	auto decodedToken = jwt::decode<jwt::traits::nlohmann_json>(token);

	std::error_code ec;
	verifier.verify(decodedToken, ec);

	if (ec) {
		std::cout << "\033[1;31m[-] Error: " << ec.message() << "\033[0m\n";
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, ec.message(), std::nullopt);
	}

	json tokenJson = decodedToken.get_payload_json();
	std::string id = tokenJson["id"];

	pqxx::result res = Core::Database::database.Query("SELECT * FROM wills WHERE stock_id IN (SELECT id FROM stocks WHERE owner_id = '" + id + "');");

	json willsJson;

	for (auto row : res) {
		json willJson;
		willJson["id"] = row["id"].as<std::string>();
		willJson["stock_id"] = row["stock_id"].as<std::string>();
		willJson["beneficiary_name"] = row["beneficiary_name"].as<std::string>();
		willJson["beneficiary_relation"] = row["beneficiary_relation"].as<std::string>();
		willJson["done_at"] = row["done_at"].as<std::optional<std::string>>().value_or("");
		willJson["created_at"] = row["created_at"].as<std::string>();
		willJson["quantity"] = row["quantity"].as<std::string>();

		willsJson.push_back(willJson);
	}
	return std::make_tuple(Core::Net::ResponseType::JSON, willsJson.dump(), std::nullopt);
}

httpReturn GetWill(Core::Net::Request& req) {
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

	std::string willId = req.m_info.parameters["will_id"];

	if (willId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing fields", std::optional<std::vector<std::string>>(false));
	}

	// Validate data
	if (willId.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid stock id", std::nullopt);
	}

	// Check if will exists
	pqxx::result res = Core::Database::database.Query("SELECT * FROM wills WHERE id = " + willId + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Will does not exist", std::nullopt);
	}

	// Check if user has stock
	res = Core::Database::database.Query("SELECT id FROM stocks WHERE id = " + res[0]["stock_id"].as<std::string>() + " AND owner_id = '" + id + "';");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "You do not own this stock", std::nullopt);
	}

	res = Core::Database::database.Query("SELECT * FROM wills WHERE id = " + willId + ";");

	json willJson;
	willJson["stock_id"] = res[0]["stock_id"].as<std::string>();
	willJson["beneficiary_name"] = res[0]["beneficiary_name"].as<std::string>();
	willJson["beneficiary_relation"] = res[0]["beneficiary_relation"].as<std::string>();
	willJson["done_at"] = res[0]["done_at"].as<std::optional<std::string>>().value_or("");
	willJson["created_at"] = res[0]["created_at"].as<std::string>();
	willJson["quantity"] = res[0]["quantity"].as<float>();

	return std::make_tuple(Core::Net::ResponseType::JSON, willJson.dump(), std::nullopt);
}

httpReturn UpdateWill(Core::Net::Request& req) {
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

	std::string willId = body["will_id"];
	std::string stockId = body["stock_id"];
	std::string beneficiaryName = body["beneficiary_name"];
	std::string beneficiaryEgn = body["beneficiary_egn"];

	if (willId == "" || beneficiaryName == "" || beneficiaryEgn == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing fields", std::optional<std::vector<std::string>>(false));
	}

	// Validate data
	if (willId.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid stock id", std::nullopt);
	}

	if (stockId.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid stock id", std::nullopt);
	}

	if (beneficiaryEgn.find_first_not_of("0123456789") != std::string::npos || (beneficiaryEgn.length() != 10 && beneficiaryEgn.length() != 9)) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid beneficiary egn", std::nullopt);
	}

	if (beneficiaryName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid beneficiary name", std::nullopt);
	}

	// Check if stock exists
	pqxx::result res = Core::Database::database.Query("SELECT id FROM stocks WHERE id = " + stockId + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Stock does not exist", std::nullopt);
	}

	res = Core::Database::database.Query("SELECT id FROM wills WHERE id = " + willId + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Will does not exist", std::nullopt);
	}

	// Check if user has stock
	res = Core::Database::database.Query("SELECT id FROM stocks WHERE id = " + stockId + " AND owner_id = '" + id + "';");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "You do not own this stock", std::nullopt);
	}

	// Check if beneficiary exists
	cpr::Response beneficiaryRes = cpr::Get(cpr::Url{ "https://www.migaccount.com/api_checksum?bulstat=" + beneficiaryEgn });

	json beneficiaryJson = json::parse(beneficiaryRes.text);

	if (beneficiaryJson["is_egn"] != true && beneficiaryJson["bulstat"].is_null() != false) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid beneficiary egn", std::nullopt);
	}

	res = Core::Database::database.Query("UPDATE wills SET beneficiary_name = '" + beneficiaryName + "', beneficiary_egn = '" + beneficiaryEgn + "', stock_id = " + stockId + " WHERE id = " + willId + " RETURNING id, done_at, created_at;");

	if (res.affected_rows() == 0) {
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, "Failed to update will", std::nullopt);
	}

	json willJson;
	willJson["id"] = res[0]["id"].as<std::string>();
	willJson["stock_id"] = stockId;
	willJson["beneficiary_name"] = beneficiaryName;
	willJson["beneficiary_egn"] = beneficiaryEgn;
	willJson["will_id"] = willId;
	willJson["done_at"] = res[0]["done_at"].as<std::optional<std::string>>().value_or("");
	willJson["created_at"] = res[0]["created_at"].as<std::string>();

	return std::make_tuple(Core::Net::ResponseType::JSON, willJson.dump(), std::nullopt);
}

httpReturn DeleteWill(Core::Net::Request& req) {
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

	std::string willId = body["will_id"];

	if (willId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing fields", std::optional<std::vector<std::string>>(false));
	}

	// Validate data

	if (willId.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid will id", std::nullopt);
	}

	// Check if will exists
	pqxx::result res = Core::Database::database.Query("SELECT * FROM wills WHERE id = " + willId + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Will does not exist", std::nullopt);
	}

	// Check if user has stock
	res = Core::Database::database.Query("SELECT id FROM stocks WHERE id = " + res[0]["stock_id"].as<std::string>() + " AND owner_id = '" + id + "';");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "You do not own this stock", std::nullopt);
	}

	res = Core::Database::database.Query("DELETE FROM wills WHERE id = " + willId + ";");

	if (res.affected_rows() == 0) {
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, "Failed to delete will", std::nullopt);
	}

	return std::make_tuple(Core::Net::ResponseType::OK, "", std::nullopt);
}

httpReturn DeleteWills(Core::Net::Request& req) {
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

	pqxx::result res = Core::Database::database.Query("DELETE FROM wills WHERE stock_id IN (SELECT id FROM stocks WHERE owner_id = '" + id + "');");

	json willsJson;

	willsJson["deleted_wills"] = std::to_string(res.affected_rows());

	return std::make_tuple(Core::Net::ResponseType::JSON, willsJson.dump(), std::nullopt);
}

httpReturn AdminGetWills(Core::Net::Request& req) {
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
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::nullopt);
	}

	if (req.m_info.parameters["owner_id"] != "") {
		json body;
		try {
			body = json::parse(req.m_info.body);
		}
		catch (json::parse_error& e) {
			return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
		}

		std::string ownerId = body["owner_id"];

		if (ownerId == "") {
			return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing fields", std::nullopt);
		}

		// Validate data
		if (ownerId.find_first_not_of("0123456789") != std::string::npos) {
			return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid owner id", std::nullopt);
		}

		pqxx::result res = Core::Database::database.Query("SELECT * FROM wills WHERE stock_id IN (SELECT id FROM stocks WHERE owner_id = '" + ownerId + "');");

		json willsJson;
		for (auto row : res) {
			json willJson;
			willJson["id"] = row["id"].as<std::string>();
			willJson["stock_id"] = row["stock_id"].as<std::string>();
			willJson["beneficiary_name"] = row["beneficiary_name"].as<std::string>();
			willJson["beneficiary_relation"] = row["beneficiary_relation"].as<std::string>();
			willJson["done_at"] = row["done_at"].as<std::optional<std::string>>().value_or("");
			willJson["created_at"] = row["created_at"].as<std::string>();
			willJson["quantity"] = row["quantity"].as<std::string>();
			willJson["owner_id"] = ownerId;

			willsJson.push_back(willJson);
		}

		return std::make_tuple(Core::Net::ResponseType::JSON, willsJson.dump(), std::nullopt);
	}

	pqxx::result res = Core::Database::database.Query("SELECT * FROM wills;");

	json willsJson;
	for (auto row : res) {
		json willJson;
		willJson["id"] = row["id"].as<std::string>();
		willJson["stock_id"] = row["stock_id"].as<std::string>();
		willJson["beneficiary_name"] = row["beneficiary_name"].as<std::string>();
		willJson["beneficiary_relation"] = row["beneficiary_relation"].as<std::string>();
		willJson["done_at"] = row["done_at"].as<std::optional<std::string>>().value_or("");
		willJson["created_at"] = row["created_at"].as<std::string>();
		willJson["quantity"] = row["quantity"].as<std::string>();

		willsJson.push_back(willJson);
	}

	return std::make_tuple(Core::Net::ResponseType::JSON, willsJson.dump(), std::nullopt);
}

httpReturn AdminGetWill(Core::Net::Request& req) {
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
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::nullopt);
	}

	std::string willId = req.m_info.parameters["will_id"];

	if (willId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing fields", std::nullopt);
	}

	// Validate data
	if (willId.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid will id", std::nullopt);
	}

	// Check if will exists
	pqxx::result res = Core::Database::database.Query("SELECT * FROM wills WHERE id = " + willId + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Will does not exist", std::nullopt);
	}

	res = Core::Database::database.Query("SELECT * FROM wills WHERE id = " + willId + ";");

	json willJson;
	willJson["stock_id"] = res[0]["stock_id"].as<std::string>();
	willJson["beneficiary_name"] = res[0]["beneficiary_name"].as<std::string>();
	willJson["beneficiary_relation"] = res[0]["beneficiary_relation"].as<std::string>();
	willJson["done_at"] = res[0]["done_at"].as<std::optional<std::string>>().value_or("");
	willJson["created_at"] = res[0]["created_at"].as<std::string>();
	willJson["quantity"] = res[0]["quantity"].as<float>();

	return std::make_tuple(Core::Net::ResponseType::JSON, willJson.dump(), std::nullopt);
}

httpReturn AdminUpdateWill(Core::Net::Request& req) {
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
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::nullopt);
	}

	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string willId = body["will_id"];

	if (willId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing fields", std::nullopt);
	}

	// Validate data
	if (willId.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid will id", std::nullopt);
	}

	// Check if will exists
	pqxx::result res = Core::Database::database.Query("SELECT * FROM wills WHERE id = " + willId + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Will does not exist", std::nullopt);
	}

	std::string stockId = body["stock_id"];
	std::string beneficiaryName = body["beneficiary_name"];
	std::string beneficiaryEgn = body["beneficiary_egn"];

	// yyyy-mm-dd (can be empty)
	std::optional<std::string> doneAt = std::nullopt;
	if (body["done_at"].is_null() == false) {
		doneAt.value() = body["done_at"];
	}

	if (stockId == "" || beneficiaryName == "" || beneficiaryEgn == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing fields", std::nullopt);
	}

	// Validate data
	if (stockId.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid stock id", std::nullopt);
	}

	if (beneficiaryEgn.find_first_not_of("0123456789") != std::string::npos || (beneficiaryEgn.length() != 10 && beneficiaryEgn.length() != 9)) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid beneficiary egn", std::nullopt);
	}

	if (beneficiaryName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid beneficiary name", std::nullopt);
	}

	if (doneAt.has_value() && (doneAt.value().find_first_not_of("0123456789-") != std::string::npos || doneAt.value().at(4) != '-' || doneAt.value().at(7) != '-' || doneAt.value().length() != 10)) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid done at", std::nullopt);
	}

	// Check if stock exists
	res = Core::Database::database.Query("SELECT id FROM stocks WHERE id = " + stockId + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Stock does not exist", std::nullopt);
	}

	// Check if beneficiary exists
	cpr::Response beneficiaryRes = cpr::Get(cpr::Url{ "https://www.migaccount.com/api_checksum?bulstat=" + beneficiaryEgn });
	json beneficiaryJson = json::parse(beneficiaryRes.text);

	if (beneficiaryJson["is_egn"] != true && beneficiaryJson["bulstat"].is_null() != false) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid beneficiary egn", std::nullopt);
	}

	if (doneAt.has_value()) {
		if (doneAt.value() == "") {
			res = Core::Database::database.Query("UPDATE wills SET beneficiary_name = '" + beneficiaryName + "', beneficiary_egn = '" + beneficiaryEgn + "', stock_id = " + stockId + ", done_at = NULL WHERE id = " + willId + ";");
		}
		else {
			res = Core::Database::database.Query("UPDATE wills SET beneficiary_name = '" + beneficiaryName + "', beneficiary_egn = '" + beneficiaryEgn + "', stock_id = " + stockId + ", done_at = '" + doneAt.value() + "' WHERE id = " + willId + ";");
		}
	}
	else {
		res = Core::Database::database.Query("UPDATE wills SET beneficiary_name = '" + beneficiaryName + "', beneficiary_egn = '" + beneficiaryEgn + "', stock_id = " + stockId + " WHERE id = " + willId + ";");
	}

	if (res.affected_rows() == 0) {
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, "Failed to update will", std::nullopt);
	}

	json willJson;
	willJson["stock_id"] = stockId;
	willJson["beneficiary_name"] = beneficiaryName;
	willJson["beneficiary_egn"] = beneficiaryEgn;
	willJson["done_at"] = doneAt.value_or("");

	return std::make_tuple(Core::Net::ResponseType::JSON, willJson.dump(), std::nullopt);
}

httpReturn AdminDeleteWill(Core::Net::Request& req) {
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
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::nullopt);
	}

	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string willId = body["will_id"];

	if (willId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing fields", std::nullopt);
	}

	// Validate data
	if (willId.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid will id", std::nullopt);
	}

	// Check if will exists
	pqxx::result res = Core::Database::database.Query("SELECT * FROM wills WHERE id = " + willId + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Will does not exist", std::nullopt);
	}

	res = Core::Database::database.Query("DELETE FROM wills WHERE id = " + willId + ";");

	if (res.affected_rows() == 0) {
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, "Failed to delete will", std::nullopt);
	}

	return std::make_tuple(Core::Net::ResponseType::OK, "", std::nullopt);
}

httpReturn DoWill(Core::Net::Request& req) {
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
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::nullopt);
	}

	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::optional<std::vector<std::string>>(false));
	}

	std::string willId = body["will_id"];

	if (willId == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing fields", std::nullopt);
	}

	// Validate data
	if (willId.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Invalid will id", std::nullopt);
	}

	// Check if will exists
	pqxx::result res = Core::Database::database.Query("SELECT * FROM wills WHERE id = " + willId + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Will does not exist", std::nullopt);
	}

	// Check if user has stock
	res = Core::Database::database.Query("SELECT * FROM stocks WHERE id = " + res[0]["stock_id"].as<std::string>() + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Stock does not exist", std::nullopt);
	}

	// Mark will as done with current date (yyyy-mm-dd)
	res = Core::Database::database.Query("UPDATE wills SET done_at = CURRENT_DATE WHERE id = " + willId + " RETURNING quantity, stock_id;");

	if (res.affected_rows() == 0) {
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, "Failed to delete will", std::nullopt);
	}

	res = Core::Database::database.Query("UPDATE stocks SET quantity = quantity - " + res[0]["quantity"].as<std::string>() + " WHERE id = " + res[0]["stock_id"].as<std::string>() + ";");

	if (res.affected_rows() == 0) {
		return std::make_tuple(Core::Net::ResponseType::INTERNAL_ERROR, "Failed to delete will", std::nullopt);
	}

	return std::make_tuple(Core::Net::ResponseType::OK, "", std::nullopt);
}