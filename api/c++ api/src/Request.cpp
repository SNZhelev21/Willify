#include "../include/Request.hpp"

Core::Net::Request::Request() {
	this->m_info.sender = INVALID_SOCKET;
	this->m_info.original = "";
	this->m_info.route = "";
	this->m_info.method = "";
	this->m_info.parameters = std::unordered_map<std::string, std::string>();
	this->m_info.headers = std::unordered_map<std::string, std::string>();
	this->m_info.body = "";

}

Core::Net::Request::Request(std::string& req, SOCKET& sender) {
	this->m_info.sender = sender;
	this->m_info.original = req;
	this->m_info.route = Core::Net::Request::GetRoute(req);
	this->m_info.method = Core::Net::Request::GetMethod(req);
	this->m_info.parameters = Core::Net::Request::GetParameters(req);
	this->m_info.headers = Core::Net::Request::GetHeaders(req);
	this->m_info.body = Core::Net::Request::GetBody(req);
}

std::vector<std::string> Core::Net::Request::Split(std::string str, char delimiter) {
	std::vector<std::string> split;
	std::stringstream ss(str);
	std::string word;
	while (std::getline(ss, word, delimiter)) {
		if (!word.empty()) {
			split.push_back(word);
		}
	}
	return split;
}

std::string Core::Net::Request::GetRoute(std::string req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, ' ');
	return split[1];
}

std::unordered_map<std::string, std::string> Core::Net::Request::GetParameters(std::string req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, ' ');
	std::string route = split[1];
	std::regex argsRegex("\\?(.*)");
	std::smatch argsMatch;
	std::regex_search(route, argsMatch, argsRegex);
	std::string parametersString = argsMatch[1];
	std::vector<std::string> parameters = Core::Net::Request::Split(parametersString, '&');

	std::unordered_map<std::string, std::string> parametersMap;
	for (std::string parameter : parameters) {
		std::stringstream parameterStream(parameter);
		std::string key;
		std::string value;
		std::getline(parameterStream, key, '=');
		std::getline(parameterStream, value, '=');
		parametersMap[key] = value;
	}

	return parametersMap;
}

std::string Core::Net::Request::GetMethod(std::string req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, ' ');
	return split[0];
}

std::string Core::Net::Request::GetBody(std::string req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, '\n');
	std::string body = "";

	int connLenLine = -1;

	for (int i = 0; i < split.size(); ++i) {
		if (split[i].find("Content-Length") != std::string::npos) {
			connLenLine = i;
			break;
		}
	}

	if (connLenLine == -1) {
		return "";
	}

	std::string connLenLineStr = split[connLenLine];
	std::regex connLenRegex("Content-Length: (.*)");
	std::smatch connLenMatch;
	std::regex_search(connLenLineStr, connLenMatch, connLenRegex);
	std::string connLenStr = connLenMatch[1];
	int connLen = std::stoi(connLenStr);

	for (int i = connLenLine + 1; i < split.size(); ++i) {
		body.append(split[i] + '\n');
	}

	return body;
}

std::string Core::Net::Request::GetHeader(std::string req, std::string header) {
	std::vector<std::string> split = Core::Net::Request::Split(req, '\n');
	std::string headerLine = "";

	for (int i = 0; i < split.size(); ++i) {
		if (split[i].find(header) != std::string::npos) {
			headerLine = split[i];
			break;
		}
	}

	if (headerLine == "") {
		return "";
	}

	std::regex headerRegex(header + ": (.*)");
	std::smatch headerMatch;
	std::regex_search(headerLine, headerMatch, headerRegex);
	std::string headerStr = headerMatch[1];

	return headerStr;
}

std::unordered_map<std::string, std::string> Core::Net::Request::GetHeaders(std::string req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, '\n');
	std::unordered_map<std::string, std::string> headers;

	std::cout << "\033[1;35m" << req.length() << "\033[0m\n";

	for (int i = 1; i < split.size(); ++i) {
		// std::regex

		//std::regex headerRegex("(.+): (.+)");
		//std::smatch headerMatch;
		//std::regex_search(split[i], headerMatch, headerRegex);
		//std::string header = headerMatch[1];
		//std::string value = headerMatch[2];

		// ctre
		auto headerMatch = ctre::match<"(.*): (.*)">(split[i]);
		auto matched = headerMatch.matched();
		if (matched.to_string() != "") {
			std::string header = matched.get<1>().to_string();
			std::string value = matched.get<2>().to_string();

			headers[header] = value;
		}
	}

	return headers;
}

std::string Core::Net::Request::GetProtocol(std::string req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, ' ');
	return split[2];
}

std::string Core::Net::Request::GetProtocolVersion(std::string req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, ' ');
	return split[3];
}