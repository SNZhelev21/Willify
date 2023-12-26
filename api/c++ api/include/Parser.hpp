#pragma once
#include <iostream>
#include "Event.hpp"
#include <functional>
#include <sstream>
#include <regex>
#include <string>

namespace Core {
	namespace Net {
		class Parser {
			public:
				static void Parse(std::string req, SOCKET connection);
				static Event<std::string, std::string, std::unordered_map<std::string, std::string> > onGet;
				static Event<std::string, std::string, std::unordered_map<std::string, std::string> > onPost;
				static Event<std::string, std::string, std::unordered_map<std::string, std::string> > onPut;
				static Event<std::string, std::string, std::unordered_map<std::string, std::string> > onDelete;

			private:
				static std::string GetRoute(std::string req);
				static std::unordered_map<std::string, std::string> GetParameters(std::string req);
				static std::vector<std::string> Split(std::string str, char delimiter);

				static std::string GetMethod(std::string req);

				static std::string GetBody(std::string req);

				static std::string GetHeader(std::string req, std::string header);

				static std::string GetProtocol(std::string req);

				static std::string GetProtocolVersion(std::string req);
		};
	}
}