#pragma once
#include <iostream>
#include <functional>
#include <sstream>
#include <regex>
#include <string>
#include <unordered_map>
#include "Event.hpp"
#include "Request.hpp"

namespace Core {
	namespace Net {
		class Router {
			public:
				void Handle(Request& req);

				void AddRoute(std::string method, std::string path, std::function<void(Request&)> callback);

			private:
				std::unordered_map<std::string, Event<Core::Net::Request&>> get;
				std::unordered_map<std::string, Event<Core::Net::Request&>> post;
				std::unordered_map<std::string, Event<Core::Net::Request&>> put;
				std::unordered_map<std::string, Event<Core::Net::Request&>> del;
		};
	}
}