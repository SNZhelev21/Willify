#include "../include/Parser.hpp"

void Core::Net::Parser::Parse(std::string req, SOCKET connection) {
	std::cout << "Received data: \n" << req << '\n';

	std::string method = Core::Net::Parser::GetMethod(req);

	std::string route = Core::Net::Parser::GetRoute(req);

	// split parameters by equals sign and put them in a map
	std::unordered_map<std::string, std::string> parametersMap = Core::Net::Parser::GetParameters(req);

	std::cout << "Parameters: \n";
	for (auto const& [key, val] : parametersMap) {
		std::cout << key << ": " << val << '\n';
	}

	std::string body = Core::Net::Parser::GetBody(req);
	std::cout << "\033[1;34mBody: " << body << "\033[0m\n";

	if (method == "GET") {
		Core::Net::Parser::onGet.Invoke(body, route, parametersMap);
	}
	else if (method == "POST") {
		Core::Net::Parser::onPost.Invoke(body, route, parametersMap);
	}
	else if (method == "PUT") {
		Core::Net::Parser::onPut.Invoke(body, route, parametersMap);
	}
	else if (method == "DELETE") {
		Core::Net::Parser::onDelete.Invoke(body, route, parametersMap);
	}

}

//Core::Net::Parser::

std::vector<std::string> Core::Net::Parser::Split(std::string str, char delimiter) {
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

std::string Core::Net::Parser::GetRoute(std::string req) {
	std::vector<std::string> split = Split(req, ' ');
	return split[1];
}

std::unordered_map<std::string, std::string> Core::Net::Parser::GetParameters(std::string req) {
	std::vector<std::string> split = Split(req, ' ');
	std::string route = split[1];
	std::regex argsRegex("\\?(.*)");
	std::smatch argsMatch;
	std::regex_search(route, argsMatch, argsRegex);
	std::string parametersString = argsMatch[1];
	std::vector<std::string> parameters = Split(parametersString, '&');

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

std::string Core::Net::Parser::GetMethod(std::string req) {
	std::vector<std::string> split = Split(req, ' ');
	return split[0];
}

std::string Core::Net::Parser::GetBody(std::string req) {
	std::vector<std::string> split = Split(req, '\n');
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

std::string Core::Net::Parser::GetHeader(std::string req, std::string header) {
	std::vector<std::string> split = Split(req, '\n');
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

std::string Core::Net::Parser::GetProtocol(std::string req) {
	std::vector<std::string> split = Split(req, ' ');
	return split[2];
}

std::string Core::Net::Parser::GetProtocolVersion(std::string req) {
	std::vector<std::string> split = Split(req, ' ');
	return split[3];
}

Event<std::string, std::string, std::unordered_map<std::string, std::string> > Core::Net::Parser::onGet;
Event<std::string, std::string, std::unordered_map<std::string, std::string> > Core::Net::Parser::onPost;
Event<std::string, std::string, std::unordered_map<std::string, std::string> > Core::Net::Parser::onPut;
Event<std::string, std::string, std::unordered_map<std::string, std::string> > Core::Net::Parser::onDelete;