#include "../../include/backend/Users.hpp"

httpReturn Register(Core::Net::Request& req) {
	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::nullopt);
	}

	std::string fName = body["first_name"];
	std::string lName = body["last_name"];
	std::string username = body["username"];
	std::string password = body["password"];
	std::string email = body["email"];

	//Validate data
	if (fName == "" || lName == "" || username == "" || password == "" || email == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing required fields", std::nullopt);
	}

	if (fName.length() < 3 || lName.length() < 3 || username.length() < 3 || password.length() < 3 || email.length() < 3) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Fields must be more than 3 characters", std::nullopt);
	}

	if (fName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "First name must only contain letters", std::nullopt);
	}

	if (lName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Last name must only contain letters", std::nullopt);
	}

	if (username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Username must only contain letters and numbers", std::nullopt);
	}

	if (email.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@.") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must only contain letters, numbers, @ and .", std::nullopt);
	}

	if (email.find_first_of("@") == std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must contain @", std::nullopt);
	}

	if (email.find_first_of(".") == std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must contain .", std::nullopt);
	}

	if (password.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!#$%^&*") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Password must only contain letters, numbers, !, #, $, %, ^, & and *", std::nullopt);
	}

	std::string query = "SELECT * FROM users WHERE username = '" + username + "';";

	auto res = Core::Database::database.Query(query);

	if (res.size() > 0) {
		return std::make_tuple(Core::Net::ResponseType::ALREADY_EXISTS, "User with username " + username + " already exists", std::nullopt);
	}

	query = "SELECT last_value FROM users_id_seq";

	res = Core::Database::database.Query(query);

	std::optional<long long int> temp = res[0]["last_value"].as<std::optional<long long int>>();
	long long int id = (temp.value_or(0) + 1) * 52834;

	password = email + username + password + std::to_string(id) + secret;

	std::string hashedSalted = Hash(password);

	query = "INSERT INTO users (id, first_name, last_name, username, email, password) VALUES (DEFAULT, '" + fName + "', '" + lName + "', '" + username + "', '" + email + "', '" + hashedSalted + "');";

	Core::Database::database.Query(query);

	return std::make_tuple(Core::Net::ResponseType::CREATED, "", std::nullopt);
}

httpReturn Login(Core::Net::Request& req) {
	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::nullopt);
	}

	if (!body.contains("username") || !body.contains("password")) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing required fields", std::nullopt);
	}

	std::string username = body["username"];
	std::string password = body["password"];

	std::string query = "SELECT id, password, email, role FROM users WHERE username = '" + username + "';";

	auto res = Core::Database::database.Query(query);

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "User with username " + username + " not found", std::nullopt);
	}

	int id = res[0]["id"].as<int>();
	std::string dbPassword = res[0]["password"].as<std::string>();
	std::string email = res[0]["email"].as<std::string>();
	std::optional<std::string> role = res[0]["role"].as<std::optional<std::string>>();

	password = email + username + password + std::to_string(id * 52834) + secret;

	password = Hash(password);

#ifdef API_DEBUG
	std::cout << "\033[1;34m[*] Password: " << password << "\033[0m\n";
	std::cout << "\033[1;34m[*] DB password: " << dbPassword << "\033[0m\n";
#endif
	if (password != dbPassword) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Incorrect password", std::nullopt);
	}

	auto jwtToken = jwt::create<jwt::traits::nlohmann_json>()
		.set_issuer("auth0")
		.set_type("JWT")
		.set_payload_claim("id", std::to_string(id))
		.set_payload_claim("username", username)
		.set_payload_claim("email", email)
		.set_payload_claim("role", role.value_or(""))
		.set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{ 3600 })
		.set_issued_at(std::chrono::system_clock::now());

	std::string signedToken = jwtToken.sign(jwt::algorithm::rs512{ "", rsaSecret, "", "" });

	return std::make_tuple(Core::Net::ResponseType::OK, signedToken, std::nullopt);
}

httpReturn GetUser(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

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

	if (id == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing id", std::nullopt);
	}
	pqxx::result res = Core::Database::database.Query("SELECT * FROM users WHERE id = " + id + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "User with id " + id + " not found", std::nullopt);
	}

	json user;
	user["id"] = res[0]["id"].as<int>();
	user["first_name"] = res[0]["first_name"].as<std::string>();
	user["last_name"] = res[0]["last_name"].as<std::string>();
	user["username"] = res[0]["username"].as<std::string>();
	user["email"] = res[0]["email"].as<std::string>();
	user["egn"] = res[0]["egn"].as<std::optional<std::string>>().value_or("");
	user["role"] = res[0]["role"].as<std::string>();

	return std::make_tuple(Core::Net::ResponseType::JSON, user.dump(), std::nullopt);
}

httpReturn DeleteUser(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

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

	if (id == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing id", std::nullopt);
	}

	pqxx::result res = Core::Database::database.Query("DELETE FROM users WHERE id = " + id + ";");

	if (res.affected_rows() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "User with id " + id + " not found", std::nullopt);
	}

	return std::make_tuple(Core::Net::ResponseType::OK, "User deleted", std::nullopt);
}

httpReturn UpdateUser(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

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
	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::nullopt);
	}

	std::string fName = body["first_name"];
	std::string lName = body["last_name"];
	std::string username = body["username"];
	std::string email = body["email"];

	//Validate data
	if (fName == "" || lName == "" || username == "" || email == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing required fields", std::nullopt);
	}

	if (fName.length() < 3 || lName.length() < 3 || username.length() < 3 || email.length() < 3) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Fields must be more than 3 characters", std::nullopt);
	}

	if (fName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "First name must only contain letters", std::nullopt);
	}

	if (lName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Last name must only contain letters", std::nullopt);
	}

	if (username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Username must only contain letters and numbers", std::nullopt);
	}

	if (email.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@.") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must only contain letters, numbers, @ and .", std::nullopt);
	}

	if (email.find_first_of("@") == std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must contain @", std::nullopt);
	}

	if (email.find_first_of(".") == std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must contain .", std::nullopt);
	}

	Core::Database::database.Query("UPDATE users SET first_name = '" + fName + "', last_name = '" + lName + "', username = '" + username + "', email = '" + email + "' WHERE id = " + tokenJson["id"].get<std::string>() + ";");

	json user;
	user["id"] = tokenJson["id"];
	user["first_name"] = fName;
	user["last_name"] = lName;
	user["username"] = username;
	user["email"] = email;

	return std::make_tuple(Core::Net::ResponseType::OK, user.dump(), std::nullopt);
}

httpReturn AdminUpdateUser(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

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

	if (tokenJson["role"].get<std::string>() != "admin") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::nullopt);
	}

	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::nullopt);
	}

	std::string fName = body["first_name"];
	std::string lName = body["last_name"];
	std::string username = body["username"];
	std::string email = body["email"];
	std::string id = body["user_id"];
	std::string egn = body["egn"];
	std::string disabled = body["disabled"];

	//Validate data
	if (fName == "" || lName == "" || username == "" || email == "" || id == "" || egn == "" || disabled == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing required fields", std::nullopt);
	}

	if (fName.length() < 3 || lName.length() < 3 || username.length() < 3 || email.length() < 3) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Fields must be more than 3 characters", std::nullopt);
	}

	if (egn.length() != 10) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "EGN must be 10 characters long", std::nullopt);
	}

	if (fName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "First name must only contain letters", std::nullopt);
	}

	if (lName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Last name must only contain letters", std::nullopt);
	}

	if (username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Username must only contain letters and numbers", std::nullopt);
	}

	if (email.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@.") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must only contain letters, numbers, @ and .", std::nullopt);
	}

	if (email.find_first_of("@") == std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must contain @", std::nullopt);
	}

	if (email.find_first_of(".") == std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Email must contain .", std::nullopt);
	}

	if (id.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "User id must only contain numbers", std::nullopt);
	}

	if (egn.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "EGN must only contain numbers", std::nullopt);
	}

	if (disabled != "true" && disabled != "false") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Disabled must be true or false", std::nullopt);
	}

	if (disabled == "true") {
		disabled = "yes";
	}
	else {
		disabled = "no";
	}

	Core::Database::database.Query("UPDATE users SET first_name = '" + fName + "', last_name = '" + lName + "', username = '" + username + "', email = '" + email + "', egn = '" + egn + "', disabled = '" + disabled + "' WHERE id = " + id + ";");

	json user;
	user["id"] = id;
	user["first_name"] = fName;
	user["last_name"] = lName;
	user["username"] = username;
	user["email"] = email;
	user["egn"] = egn;
	user["disabled"] = disabled;

	return std::make_tuple(Core::Net::ResponseType::OK, user.dump(), std::nullopt);
}

httpReturn AdminDeleteUser(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

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

	if (tokenJson["role"].get<std::string>() != "admin") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::nullopt);
	}

	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::nullopt);
	}

	std::string id = body["user_id"];

	//Validate data
	if (id == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing required fields", std::nullopt);
	}

	if (id.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "User id must only contain numbers", std::nullopt);
	}

	Core::Database::database.Query("DELETE FROM users WHERE id = " + id + ";");

	return std::make_tuple(Core::Net::ResponseType::OK, "", std::nullopt);
}

httpReturn AdminGetUser(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

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

	if (tokenJson["role"].get<std::string>() != "admin") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::nullopt);
	}

	json body;
	try {
		body = json::parse(req.m_info.body);
	}
	catch (json::parse_error& e) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, e.what(), std::nullopt);
	}

	std::string id = body["user_id"];

	//Validate data
	if (id == "") {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "Missing required fields", std::nullopt);
	}

	if (id.find_first_not_of("0123456789") != std::string::npos) {
		return std::make_tuple(Core::Net::ResponseType::BAD_REQUEST, "User id must only contain numbers", std::nullopt);
	}

	pqxx::result res = Core::Database::database.Query("SELECT * FROM users WHERE id = " + id + ";");

	if (res.size() == 0) {
		return std::make_tuple(Core::Net::ResponseType::NOT_FOUND, "User with id " + id + " not found", std::nullopt);
	}

	json user;
	user["id"] = res[0]["id"].as<int>();
	user["first_name"] = res[0]["first_name"].as<std::string>();
	user["last_name"] = res[0]["last_name"].as<std::string>();
	user["username"] = res[0]["username"].as<std::string>();
	user["email"] = res[0]["email"].as<std::string>();

	return std::make_tuple(Core::Net::ResponseType::JSON, user.dump(), std::nullopt);
}

httpReturn AdminGetUsers(Core::Net::Request& req) {
	std::string token = req.GetHeader(req.m_info.original, "Authorization");

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

	if (tokenJson["role"].get<std::string>() != "admin") {
		return std::make_tuple(Core::Net::ResponseType::NOT_AUTHORIZED, "Only admins can access this route", std::nullopt);
	}

	pqxx::result res = Core::Database::database.Query("SELECT * FROM users;");

	json users;
	for (int i = 0; i < res.size(); i++) {
		json user;
		user["id"] = res[i]["id"].as<int>();
		user["first_name"] = res[i]["first_name"].as<std::string>();
		user["last_name"] = res[i]["last_name"].as<std::string>();
		user["username"] = res[i]["username"].as<std::string>();
		user["email"] = res[i]["email"].as<std::string>();
		user["egn"] = res[i]["egn"].as<std::optional<std::string>>().value_or("");
		user["role"] = res[i]["role"].as<std::string>();

		users.push_back(user);
	}

	return std::make_tuple(Core::Net::ResponseType::JSON, users.dump(), std::nullopt);
}