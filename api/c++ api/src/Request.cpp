#include "../include/Request.hpp"

Core::Net::RequestInformation::RequestInformation(std::string& req, SOCKET& sender) :
	sender(sender),
	original(req),
	route(Core::Net::Request::GetRoute(original)),
	method(Core::Net::Request::GetMethod(original)),
	parameters(Core::Net::Request::GetParameters(original)),
	headers(Core::Net::Request::GetHeaders(original)),
	body(Core::Net::Request::GetBody(original))
{};

Core::Net::Request::Request() {
	this->m_info = {};
}

Core::Net::Request::Request(std::string& req, SOCKET& sender) {
	//this->m_info.sender = sender;
	//this->m_info.original = req;
	//this->m_info.route = Core::Net::Request::GetRoute(req);
	//this->m_info.method = Core::Net::Request::GetMethod(req);
	//this->m_info.parameters = Core::Net::Request::GetParameters(req);
	//this->m_info.headers = Core::Net::Request::GetHeaders(req);
	//this->m_info.body = Core::Net::Request::GetBody(req);
	this->m_info = {
		req,
		sender
	};
}

std::vector<std::string> Core::Net::Request::Split(std::string const& str, char delimiter) {
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

std::string Core::Net::Request::GetRoute(std::string const& req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, ' ');
	std::vector<std::string> split2 = Core::Net::Request::Split(split[1], '?');

	return split2[0];
}

std::unordered_map<std::string, std::string> Core::Net::Request::GetParameters(std::string const& req) {
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

std::string Core::Net::Request::GetMethod(std::string const& req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, ' ');
	return split[0];
}

std::string Core::Net::Request::GetBody(std::string const& req) {
	std::string body = "";

	int bodyStartChar = -1;

	if (req.find("\r\n\r\n") != std::string::npos) {
		bodyStartChar = req.find("\r\n\r\n");
	}
	else if (req.find("\n\n") != std::string::npos) {
		bodyStartChar = req.find("\n\n");
	}

	if (bodyStartChar == -1) {
		return "";
	}

	return req.substr(bodyStartChar + 4);
}

std::string Core::Net::Request::GetHeader(std::string const& req, std::string const& header) {
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

	int bodyStartChar = req.size();

	if (req.find("\r\n\r\n") != std::string::npos) {
		bodyStartChar = req.find("\r\n\r\n");
	}
	else if (req.find("\n\n") != std::string::npos) {
		bodyStartChar = req.find("\n\n");
	}

	// remove body
	req = req.substr(0, bodyStartChar);

	auto split = Core::Net::Request::Split(req, '\n');
	std::unordered_map<std::string, std::string> headers;

	for (int i = 1; i < split.size(); ++i) {
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

std::string Core::Net::Request::GetProtocol(std::string const& req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, ' ');
	return split[2];
}

std::string Core::Net::Request::GetProtocolVersion(std::string const& req) {
	std::vector<std::string> split = Core::Net::Request::Split(req, ' ');
	return split[3];
}