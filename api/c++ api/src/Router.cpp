#include "../include/Router.hpp"

void Core::Net::Router::Handle(Request& req) {
	//std::cout << "Received data: \n" << req << '\n';

	std::string method = req.m_info.method;

	std::vector<returnType> responses;
	returnType response = std::make_tuple(ResponseType::OK, "", std::optional<std::vector<std::string>>(false));

	try {
		if (method == "GET") {
			//std::cout << "get\n";
			responses = this->get[req.m_info.route].Invoke(req);
		}
		else if (method == "POST") {
			responses = this->post[req.m_info.route].Invoke(req);
		}
		else if (method == "PUT") {
			responses = this->put[req.m_info.route].Invoke(req);
		}
		else if (method == "DELETE") {
			responses = this->del[req.m_info.route].Invoke(req);
		}
	}
	catch (std::exception& e) {
		responses = { std::make_tuple(ResponseType::INTERNAL_ERROR, e.what(), std::optional<std::vector<std::string>>(false)) };
	}

	//std::cout << "\033[1;34m[*] Responses size: " << responses.size() << "\033[0m\n";
	for (const auto& res : responses) {
		response = res;
	}
	this->Respond(req, response);
}

void Core::Net::Router::AddRoute(std::string method, std::string path, std::function<returnType(Request&)> callback) {
	for (auto& c : method) {
		c = toupper(c);
	}

	if (method == "GET") {
		this->get[path].Attach(callback);
	}
	else if (method == "POST") {
		this->post[path].Attach(callback);
	}
	else if (method == "PUT") {
		this->put[path].Attach(callback);
	}
	else if (method == "DELETE") {
		this->del[path].Attach(callback);
	}
}

void Core::Net::Router::Respond(Request& req, returnType response) {
	ResponseType type = std::get<0>(response);
	std::string data = std::get<1>(response);

	std::string header = "HTTP/1.1 ";

	if (type == ResponseType::OK || type == ResponseType::CREATED || type == ResponseType::JSON || type == ResponseType::HTML || type == ResponseType::TEXT) {
		header += "200 OK\r\n";

		// CORS
		header += "Access-Control-Allow-Origin: https://www.google.com\r\n";
		header += "Access-Control-Allow-Methods: GET, POST, PUT, DELETE\r\n";
		header += "Access-Control-Allow-Headers: X-PINGOTHER, Content-Type\r\n";

		header += "Content-Type: ";

		if (type == ResponseType::JSON) {
			header += "application/json\r\n";
		}
		else if (type == ResponseType::HTML) {
			header += "text/html\r\n";
		}
		else if (type == ResponseType::TEXT) {
			header += "text/plain\r\n";
		}
	}
	else if (type == ResponseType::BAD_REQUEST) {
		header += "400 Bad Request\r\n";
		header += "Content-Type: text/plain\r\n";
	}
	else if (type == ResponseType::NOT_FOUND) {
		header += "404 Not Found\r\n";
		header += "Content-Type: text/plain\r\n";
	}
	else if (type == ResponseType::INTERNAL_ERROR) {
		header += "500 Internal Server Error\r\n";
		header += "Content-Type: text/plain\r\n";
	}
	else if (type == ResponseType::NOT_IMPLEMENTED) {
		header += "501 Not Implemented\r\n";
		header += "Content-Type: text/plain\r\n";
	}
	else if (type == ResponseType::NOT_AUTHORIZED) {
		header += "401 Unauthorized\r\n";
		header += "Content-Type: text/plain\r\n";
	}
	else if (type == ResponseType::REDIRECT) {
		header += "302 Found\r\n";
		header += "Location: " + data + "\r\n";
	}
	
	header += "Connection: Keep-Alive\r\n";
	header += "Content-Length: " + std::to_string(data.length()) + "\r\n\r\n";
	header += data;

	//std::cout << header << '\n';

	int bytesSent = 0;
	int totalBytesSent = 0;
	while (totalBytesSent < header.size()) {
		bytesSent = send(req.m_info.sender, header.data(), header.size(), 0);
		if (bytesSent < 0) {
			std::cout << "\033[31mFailed to send message...\033[0m\n";
			std::cout << "\033[31mError code: " << WSAGetLastError() << "\033[0m\n";
		}

		totalBytesSent += bytesSent;
	}
}